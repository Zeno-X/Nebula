// Copyright © 2016 zendo (734181759@qq.com). All rights reserved.

#pragma once
#include "intrusive_hook.h"

namespace zendo
{
    
    template<typename T, bool CacheSize, int Tag = 0>
    class intrusive_stack;
    
    template<typename T, int Tag>
    class intrusive_stack<T, false, Tag> : protected detail::hook_access
    {
    public:
        using hook_type = forward_hook<Tag>;
        using hook_ptr = hook_type*;
        using value_type = T;
        using value_ptr = value_type*;
        static_assert(std::is_base_of<hook_type, T>::value, "");
    private:
        hook_ptr top_{nullptr};
    public:
        intrusive_stack() = default;
        intrusive_stack(const intrusive_stack&) = delete;
        intrusive_stack(intrusive_stack&& rhs)
        : top_(rhs.top_)
        {
            rhs.top_ = nullptr;
        }
        
        void swap(intrusive_stack& lhs)
        {
            const hook_ptr top_one = top_;
            top_ = lhs.top_;
            lhs.top_ = top_one;
        }
        
        bool empty() const
        {
            return top_ == nullptr;
        }
        
        void push(hook_ptr element)
        {
            if(element && (next(element) == nullptr))
            {
                next(element) = top_;
                top_ = element;
            }
        }
        
        value_ptr pop()
        {
            hook_ptr ret = top_;
            if(top_)
            {
                top_ = next(top_);
            }
            return static_cast<value_type*>(ret);
        }
        
    };
    
    template<typename T, int Tag>
    class intrusive_stack<T, true, Tag> : protected detail::hook_access
    {
    public:
        using hook_type = forward_hook<Tag>;
        using hook_ptr = hook_type*;
        using value_type = T;
        using value_ptr = value_type*;
        static_assert(std::is_base_of<hook_type, T>::value, "");
    private:
        hook_ptr top_{nullptr};
        size_t size_{0};
    public:
        intrusive_stack() = default;
        intrusive_stack(const intrusive_stack&) = delete;
        intrusive_stack(intrusive_stack&& rhs)
        : top_(rhs.top_)
        , size_(rhs.size_)
        {
            rhs.top_ = nullptr;
            rhs.size_ = 0;
        }
        
        void swap(intrusive_stack& lhs)
        {
            std::swap(top_, lhs.top_);
            std::swap(size_, lhs.size_);
        }
        
        bool empty() const
        {
            return top_ == nullptr;
        }
        
        void push(hook_ptr element)
        {
             if(element && (next(element) == nullptr))
             {
                 next(element) = top_;
                 top_ = element;
                 ++size_;
             }
        }
        
        value_ptr pop()
        {
            hook_ptr ret = top_;
            if(top_)
            {
                top_ = next(top_);
                --size_;
            }
            return static_cast<value_type*>(ret);
        }
        
    };
    
}

