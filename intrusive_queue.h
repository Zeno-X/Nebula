// Copyright © 2016 zendo (734181759@qq.com). All rights reserved.

#pragma once
#include "intrusive_hook.h"

namespace zendo
{
    
    template<typename T, bool CacheSize, int Tag = 0>
    class intrusive_queue;

    template<typename T, int Tag>
    class intrusive_queue<T, false, Tag> : protected detail::hook_access
    {
    public:
        using hook_type = forward_hook<Tag>;
        using hook_ptr = hook_type*;
        using value_type = T;
        using value_ptr = value_type*;
        static_assert(std::is_base_of<hook_type, T>::value, "");
    private:
        hook_ptr head_{nullptr};
        hook_ptr tail_{nullptr};
    public:
        intrusive_queue() = default;
        intrusive_queue(const intrusive_queue&) = delete;
        intrusive_queue& operator=(const intrusive_queue&) = delete;
        intrusive_queue(intrusive_queue&& rhs)
        : head_(rhs.head_)
        , tail_(rhs.tail_)
        {
            rhs.head_ = nullptr;
            rhs.tail_ = nullptr;
        }
        
        void swap(intrusive_queue& lhs)
        {
            std::swap(head_, lhs.head_);
            std::swap(tail_, lhs.tail_);
        }
        
        bool empty() const
        {
            return head_ == nullptr;
        }
        
        value_ptr front() const
        {
            return static_cast<value_ptr>(head_);
        }
        
        value_ptr back() const
        {
            return static_cast<value_ptr>(tail_);
        }
        
        void enqueue(hook_ptr element)
        {
            if(element && next(element) == nullptr)
            {
                if( tail_ )
                {
                    next(tail_) = element;
                }
                else
                {
                    head_ = element;
                }
                tail_ = element;
            }
        }
        
        void enqueue(intrusive_queue& lhs)
        {
            if(lhs.head_)
            {
                if( tail_ )
                {
                    next(tail_) = lhs.head_;
                }
                else
                {
                    head_ = lhs.head_;
                }
                tail_ = lhs.tail_;
                lhs.head_ = nullptr;
                lhs.tail_ = nullptr;
            }
        }
        
        value_ptr dequeue()
        {
            if(head_)
            {
                hook_ptr ret = head_;
                head_ = next(head_);
                if(!head_)
                {
                    tail_ = nullptr;
                }
                next(ret) = nullptr;
            }
            return static_cast<value_ptr>(nullptr);
        }
        
        template<typename Handler> void for_each(const Handler& handler)
        {
            hook_ptr tmp = head_;
            while(tmp)
            {
                hook_ptr next_one = next(tmp);
                handler(static_cast<value_ptr>(tmp));
                tmp = next_one;
            }
        }
        
        template<typename Handler> void consume_all(const Handler& handler)
        {
            if( head_ )
            {
                while( head_ )
                {
                    hook_ptr next_one = next(head_);
                    next(head_) = nullptr;
                    handler(static_cast<value_ptr>(head_));
                    head_ = next_one;
                }
                tail_ = nullptr;
            }
        }
        
        void clear(bool clearup = false)
        {
            if(clearup)
            {
                while( head_ )
                {
                    hook_ptr next_one = next(head_);
                    next(head_) = nullptr;
                    head_ = next_one;
                }
            }
            else
            {
                head_ = nullptr;
            }
            tail_ = nullptr;
        }
        
    };
    
    
    template<typename T, int Tag>
    class intrusive_queue<T, true, Tag> : protected detail::hook_access
    {
    public:
        using hook_type = forward_hook<Tag>;
        using hook_ptr = hook_type*;
        using value_type = T;
        using value_ptr = value_type*;
        static_assert(std::is_base_of<hook_type, T>::value, "");
    private:
        hook_ptr head_{nullptr};
        hook_ptr tail_{nullptr};
        size_t size_{0};
    public:
        intrusive_queue() = default;
        intrusive_queue(const intrusive_queue&) = delete;
        intrusive_queue& operator=(const intrusive_queue&) = delete;
        intrusive_queue(intrusive_queue&& rhs)
        : head_(rhs.head_)
        , tail_(rhs.tail_)
        , size_(rhs.size_)
        {
            rhs.head_ = nullptr;
            rhs.tail_ = nullptr;
            rhs.size_ = 0;
        }
        
        void swap(intrusive_queue& lhs)
        {
            std::swap(head_, lhs.head_);
            std::swap(tail_, lhs.tail_);
            std::swap(size_, lhs.size_);
        }
        
        bool empty() const
        {
            return head_ == nullptr;
        }
        
        size_t size() const
        {
            return size_;
        }

        value_ptr front() const
        {
            return static_cast<value_ptr>(head_);
        }
        
        value_ptr back() const
        {
            return static_cast<value_ptr>(tail_);
        }
        
        void enqueue(hook_ptr element)
        {
            if(element && next(element) == nullptr)
            {
                if( tail_ )
                {
                    next(tail_) = element;
                }
                else
                {
                    head_ = element;
                }
                tail_ = element;
                ++size_;
            }
        }
        
        void enqueue(intrusive_queue& lhs)
        {
            if(lhs.head_)
            {
                if( tail_ )
                {
                    next(tail_) = lhs.head_;
                }
                else
                {
                    head_ = lhs.head_;
                }
                tail_ = lhs.tail_;
                size_ += lhs.size_;
                lhs.head_ = nullptr;
                lhs.tail_ = nullptr;
                lhs.size_ = 0;
            }
        }
        
        value_ptr dequeue()
        {
            if( head_ )
            {
                hook_ptr ret = head_;
                head_ = next(head_);
                if( !head_ )
                {
                    tail_ = nullptr;
                }
                next(ret) = nullptr;
                --size_;
            }
            return static_cast<value_ptr>(nullptr);
        }
        
        template<typename Handler> void for_each(const Handler& handler)
        {
            hook_ptr tmp = head_;
            while( tmp )
            {
                hook_ptr next_one = next(tmp);
                handler(static_cast<value_ptr>(tmp));
                tmp = next_one;
            }
        }
        
        template<typename Handler> void consume_all(const Handler& handler)
        {
            if( head_ )
            {
                while( head_ )
                {
                    hook_ptr next_one = next(head_);
                    next(head_) = nullptr;
                    --size_;
                    handler(static_cast<value_ptr const>(head_));
                    head_ = next_one;
                }
                tail_ = nullptr;
            }
        }
        
        void clear(bool clearup = false)
        {
            if(clearup)
            {
                while( head_ )
                {
                    hook_ptr next_one = next(head_);
                    next(head_) = nullptr;
                    head_ = next_one;
                }
            }
            else
            {
                head_ = nullptr;
            }
            tail_ = nullptr;
            size_ = 0;
        }
        
    };
    
}
