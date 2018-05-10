// Copyright © 2016 zendo (734181759@qq.com). All rights reserved.

#pragma once
#include <cstddef>
#include <cstdint>
#include <atomic>
#include <assert.h>

#ifdef _MSC_VER
#include <intrin.h>
#endif


namespace zendo
{
    
    
    // POD type
    class spin_lock
    {
    private:
        uint8_t impl_{0};
        
        std::atomic<uint8_t>* payload()
        {
            return reinterpret_cast<std::atomic<uint8_t>*>(&impl_);
        }
        
        bool cas(uint8_t compare, uint8_t new_val)
        {
            return std::atomic_compare_exchange_strong_explicit(payload(), &compare, new_val, std::memory_order_acquire, std::memory_order_relaxed);
        }
        
    public:
        void reset()
        {
            payload()->store(0);
        }
        
        bool try_lock()
        {
            return cas(0,1);
        }
        
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
            assert(payload()->load());
        }
        
        void unlock()
        {
            assert(payload()->load());
            payload()->store(0, std::memory_order_release);
        }
        
    };
    
}
