// Copyright © 2016 zendo (734181759@qq.com). All rights reserved.

#pragma once
#include <atomic>
#include <string>
#include <cstddef>
#include <cstdint>
#include <assert.h>
#ifdef _MSC_VER
#include <intrin.h>
#endif

namespace zendo
{
    
    template<bool ThreadSafe = false>
    class fixed_align_allocator
    {
    protected:
        struct fake_lock
        {
            using guard_type = std::lock_guard<fake_lock>;
            fake_lock() = default;
            constexpr void lock()noexcept{}
            constexpr void unlock()noexcept{}
        };
        
        struct spin_lock
        {
            uint8_t impl_{0};
            std::atomic<uint8_t>* payload() { return reinterpret_cast<std::atomic<uint8_t>*>(&impl_); }
            bool cas(uint8_t compare, uint8_t new_val)
            { return std::atomic_compare_exchange_strong_explicit(payload(), &compare, new_val, std::memory_order_acquire, std::memory_order_relaxed); }
            
            void lock()
            {
                uint32_t spin_count = 0;
                do
                {
                    while (payload()->load())
                    {
                        if(spin_count < 4000)
                        {
                            ++spin_count;
#if defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))
                            ::_mm_pause();
#elif defined(__i386__) || defined(__x86_64__)
                            asm volatile("pause");
#elif defined(__aarch64__) || defined(__arm__)
                            asm volatile("yield");
#elif defined(__powerpc64__)
                            asm volatile("or 27,27,27");
#endif
                        }
                        else
                        {
                            struct timespec ts = {0, 50000};
                            nanosleep(&ts, nullptr);
                        }
                    }
                }while(!cas(0,1));
            }
            
            void unlock()
            {
                payload()->store(0, std::memory_order_release);
            }
            
        };
        
        using lock_type = typename std::conditional<ThreadSafe, spin_lock, fake_lock>::type;
        struct block;
#pragma pack(push, 1)
        struct section { union { struct block* block_; struct section* link_; }; char data_[1]; };
#pragma pack(pop)
        
        struct block
        {
            lock_type lock_;
            struct section* free_;
            const size_t size_;
            block(size_t size) : lock_(), free_(nullptr), size_(size) {}
            
            ~block()
            {
                while (free_)
                {
                    section* next = free_->link_;
                    free(free_);
                    free_ = next;
                }
            }
            
            void gabage_collect()
            {
                lock_.lock();
                section* targets = free_;
                free_ = nullptr;
                lock_.unlock();
                while (targets)
                {
                    struct section* next_stn = targets->link_;
                    free(targets);
                    targets = next_stn;
                }
            }
            
            section* allocate()
            {
                section* stn;
                lock_.lock();
                if (free_)
                {
                    stn = free_;
                    free_ = free_->link_;
                    lock_.unlock();
                }
                else
                {
                    lock_.unlock();
                    stn = static_cast<section*>(malloc(sizeof(void*) + size_));
                }
                stn->block_ = this;
                return stn;
            }
            
            void deallocate(section* stn)
            {
                lock_.lock();
                stn->link_ = free_;
                free_ = stn;
                lock_.unlock();
            }
        };
        
        static constexpr size_t offset_data(){ return (size_t)((unsigned char*)(&(((section*)0x1)->data_)) - (unsigned char*)(section*)0x1); }
        static section* extract_section(void* data) { return reinterpret_cast<section*>(reinterpret_cast<char* const>(data) - offset_data()); }
        static block* extract_block(void* data)
        {
            section* stn = extract_section(data);
            if(stn)
            {
                return stn->block_;
            }
            return static_cast<block*>(nullptr);
        }
        
        struct chunk
        {
            const size_t align_, max_align_, variety_;
            struct block* blocks_;
            
            size_t find_index(size_t size) const { return (size + (align_ - 1)) / align_ - 1; }
            
            chunk(size_t align, size_t variety) : align_(align), max_align_(align * variety), variety_(variety), blocks_(variety ? static_cast<block*>(malloc(sizeof(block)*variety)) : nullptr)
            {
                if(blocks_)
                {
                    for (size_t i = 0; i < variety_; ++i)
                    {
                        new (&blocks_[i]) block(align_*(i + 1));
                    }
                }
            }
            
            ~chunk()
            {
                if (blocks_)
                {
                    for (size_t i = 0; i < variety_; ++i)
                    {
                        blocks_[i].~block();
                    }
                    free(blocks_);
                }
            }
            
            size_t get_max_align() const { return max_align_; }
            
            void gabage_collect(size_t begin_index = 0)
            {
                for (size_t i = begin_index; i < variety_; ++i)
                {
                    blocks_[i].gabage_collect();
                }
            }
            
            void* allocate(size_t size)
            {
                return blocks_[find_index(size)].allocate()->data_;
            }
            
            static bool deallocate(void* data)
            {
                section* stn = extract_section(data);
                if (stn)
                {
                    stn->block_->deallocate(stn);
                    return true;
                }
                return false;
            }
        }* free_lists_;
        const size_t align_, count_,variety_;
        
    public:
        fixed_align_allocator() = delete;
        fixed_align_allocator(size_t align, size_t count, size_t variety)
        : align_(align), count_(count), variety_(variety)
        , free_lists_(static_cast<chunk*>(malloc(sizeof(chunk)* count)))
        {
            if (free_lists_)
            {
                new (&free_lists_[0]) chunk(align_, variety_);
                for (size_t i = 1; i < count_; ++i)
                {
                    new (&free_lists_[i]) chunk(free_lists_[i - 1].get_max_align() * 2, variety_);
                }
            }
        }
        
        ~fixed_align_allocator()
        {
            if (free_lists_)
            {
                for (size_t i = 0; i < count_; ++i)
                {
                    free_lists_[i].~chunk();
                }
                free(free_lists_);
            }
        }
        
        size_t get_max_align(size_t index) const { return free_lists_[index].get_max_align(); }
        size_t get_max_align() const { return free_lists_[count_ - 1].get_max_align(); }
        static size_t extract_size(void* data)
        {
            block* blk = extract_block(data);
            if(blk)
            {
                return blk->size_;
            }
            return 0;
        }
        
        void gabage_collect(uint32_t begin_index = 0)
        {
            for (uint32_t i = begin_index; i < count_; ++i)
            {
                free_lists_[begin_index].gabage_collect();
            }
        }
        
        void* allocate(size_t size)
        {
            for (size_t i = 0; i < count_; ++i)
            {
                if (size <= free_lists_[i].get_max_align())
                {
                    return free_lists_[i].allocate(size);
                }
            }
            section* stn = static_cast<struct section*>(malloc(sizeof(void*) + size));
            stn->block_ = nullptr;
            return stn->data_;
        }
        
        static void deallocate(void* data)
        {
            section* stn = extract_section(data);
            if (stn)
            {
                block* blk = stn->block_;
                if(blk)
                {
                    blk->deallocate(stn);
                    return;
                }
                free(stn);
            }
        }
        
    };
    
}
