// Copyright © 2016 zendo (734181759@qq.com). All rights reserved.

#pragma once
#include "intrusive_hook.h"

namespace zendo
{
    
    template<typename T, bool CacheSize, int Tag = 0>
    class intrusive_list;
    
    template<typename T, int Tag>
    class intrusive_list<T, false, Tag> : protected detail::hook_access
    {
    public:
        using hook_type = bidirectional_hook<Tag>;
        using hook_ptr = hook_type*;
        using value_type = T;
        using value_ptr = value_type*;
        static_assert(std::is_base_of<hook_type, T>::value, "");
    private:
        hook_ptr head_{nullptr};
        hook_ptr tail_{nullptr};
        value_ptr unlink(hook_ptr element)
        {
            hook_ptr prev_one = prev(element);
            hook_ptr next_one = next(element);
            if (prev_one)
            {
                next(prev_one) = next_one;
            }
            else
            {
                head_ = next(head_);
            }
            if (next_one)
            {
                prev(next_one) = prev_one;
            }
            else
            {
                tail_ = prev(tail_);
            }
            return next_one;
        }
    public:
        intrusive_list() = default;
        intrusive_list(const intrusive_list&) = delete;
        intrusive_list& operator=(const intrusive_list&) = delete;
        intrusive_list(intrusive_list&& rhs)
        : head_(rhs.head_)
        , tail_(rhs.tail_)
        {
            rhs.head_ = nullptr;
            rhs.tail_ = nullptr;
        }
        
        void swap(intrusive_list& lhs)
        {
            std::swap(head_, lhs.head_);
            std::swap(tail_, lhs.tail_);
        }
        
        void swap(hook_ptr element1, hook_ptr element2)
        {
            if(element1 == element2)
            {
                return;
            }
            if(element1 && element2)
            {
                const hook_ptr prev1 = prev(element1);
                const hook_ptr next1 = next(element1);
                if(prev1 == element2)
                {
                    const hook_ptr prev2 = prev(element2);
                    if(prev2)
                    {
                        next(prev2) = element1;
                    }
                    else
                    {
                        head_ = element1;
                    }
                    if(next1)
                    {
                        prev(next1) = element2;
                    }
                    else
                    {
                        tail_ = element2;
                    }
                    prev(element1) = prev2;
                    next(element1) = element2;
                    prev(element2) = element1;
                    next(element2) = next1;
                    return;
                }
                if(next1 == element2)
                {
                    const hook_ptr next2 = next(element2);
                    if(prev1)
                    {
                        next(prev1) = element2;
                    }
                    else
                    {
                        head_ = element2;
                    }
                    if(next2)
                    {
                        prev(next2) = element1;
                    }
                    else
                    {
                         tail_ = element1;
                    }
                    prev(element2) = prev1;
                    next(element2) = element1;
                    prev(element1) = element2;
                    next(element1) = next2;
                    return;
                }
                const hook_ptr prev2 = prev(element2);
                const hook_ptr next2 = next(element2);
                if (prev1)
                {
                    next(prev1) = element2;
                }
                else
                {
                    head_ = element2;
                }
                if (next1)
                {
                    prev(next1) = element2;
                }
                else
                {
                    tail_ = element2;
                }
                if (prev2)
                {
                    next(prev2) = element1;
                }
                else
                {
                    head_ = element1;
                }
                if (next2)
                {
                    prev(next2) = element1;
                }
                else
                {
                    tail_ = element1;
                }
                prev(element2) = prev1;
                next(element2) = next1;
                prev(element1) = prev2;
                next(element1) = next2;
            }
        }
        
        bool empty() const
        {
            return head_ == nullptr;
        }
        
        void erase(hook_ptr element)
        {
            if(element)
            {
                const hook_ptr prev_one = prev(element);
                const hook_ptr next_one = next(element);
                if (prev_one)
                {
                    next(prev_one) = next_one;
                    prev(element) = nullptr;
                }
                else
                {
                    head_ = next(head_);
                }
                if (next_one)
                {
                    prev(next_one) = prev_one;
                    next(element) = nullptr;
                }
                else
                {
                    tail_ = prev(tail_);
                }
            }
            else if(element == head_)
            {
                head_ = tail_ = nullptr;
            }
        }
        
        void push_back(hook_ptr element)
        {
            if(element && next(element) == nullptr && (prev(element) == nullptr))
            {
                prev(element) = tail_;
                if (tail_)
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
        
        void push_back(intrusive_list& lhs)
        {
            if(lhs.head_)
            {
                if(tail_)
                {
                    prev(lhs.head_) = tail_;
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
        
        void push_front(hook_ptr element)
        {
            if(element && prev(element) == nullptr && (next(element) == nullptr))
            {
                next(element) = head_;
                if (head_)
                {
                    prev(head_) = element;
                }
                else
                {
                    tail_ = element;
                }
                head_ = element;
            }
        }
        
        void push_front(intrusive_list& lhs)
        {
            if(lhs.tail_)
            {
                if(head_)
                {
                    next(lhs.tail_) = head_;
                    prev(head_) = lhs.tail_;
                }
                else
                {
                    tail_ = lhs.tail_;
                }
                head_ = lhs.head_;
                lhs.head_ = nullptr;
                lhs.tail_ = nullptr;
            }
        }
        
        void insert_back(const hook_ptr where, hook_ptr element)
        {
            if(element && (next(element)==nullptr) && (prev(element) == nullptr))
            {
                if(where == tail_)
                {
                    prev(element) = tail_;
                    if (tail_)
                    {
                        next(tail_) = element;
                    }
                    else
                    {
                        head_ = element;
                    }
                    tail_ = element;
                }
                else if(where == head_)
                {
                    next(element) = head_;
                    prev(head_) = element;
                    head_ = element;
                }
                else
                {
                    hook_ptr next_one = next(where);
                    next(where) = element;
                    prev(element) = where;
                    next(element) = next_one;
                    prev(next_one) = element;
                }
            }
        }
        
        void insert_back(const hook_ptr where, intrusive_list& lhs)
        {
            if(lhs.head_)
            {
                  if(where == tail_)
                  {
                      prev(lhs.head_) = tail_;
                      if (tail_)
                      {
                          next(tail_) = lhs.head_;
                      }
                      else
                      {
                          head_ = lhs.head_;
                      }
                      tail_ = lhs.tail_;
                  }
                  else if(where == head_)
                  {
                      next(lhs.tail_) = head_;
                      prev(head_) = lhs.tail_;
                      head_ = lhs.head_;
                  }
                else
                {
                    hook_ptr next_one = next(where);
                    next(where) = lhs.head_;
                    prev(lhs.head_) = where;
                    next(lhs.tail_) = next_one;
                    prev(next_one) = lhs.tail_;
                }
                lhs.head_ = lhs.tail_ = nullptr;
            }
        }
        
        void insert_front(const hook_ptr where, hook_ptr element)
        {
            if (where == element)
            {
                return;
            }
            hook_ptr prev_one = prev(where);
            next(element) = where;
            prev(element) = prev_one;
            prev(where) = element;
            if (prev_one)
            {
                next(prev_one) = element;
            }
            else
            {
                head_ = element;
            }
        }
        
        void insert_front(const hook_ptr where, intrusive_list& lhs)
        {
            if (lhs.tail_)
            {
                if(where == head_)
                {
                    next(lhs.tail_) = head_;
                    if(head_)
                    {
                        prev(head_) = lhs.tail_;
                    }
                    else
                    {
                        tail_ = lhs.tail_;
                    }
                    head_ = lhs.head_;
                }
                else if(where == tail_)
                {
                    next(lhs.tail_) = tail_;
                    prev(tail_) = lhs.tail_;
                    tail_ = lhs.tail_;
                }
                else
                {
                    hook_ptr prev_one = prev(where);
                    prev(where) = lhs.tail_;
                    next(lhs.tail_) = where;
                    prev(lhs.head_) = prev_one;
                    next(prev_one) = lhs.head_;
                }
                lhs.head_ = lhs.tail_ = nullptr;
            }
        }
        
        value_ptr pop_front()
        {
            if(head_)
            {
                hook_ptr ret = head_;
                head_ = next(head_);
                if (head_)
                {
                    prev(head_) = nullptr;
                }
                else
                {
                    tail_ = nullptr;
                }
                next(ret) = nullptr;
                return static_cast<value_ptr>(ret);
            }
            return static_cast<value_ptr>(nullptr);
        }

        value_ptr pop_back()
        {
            if(tail_)
            {
                hook_ptr ret = tail_;
                tail_ = prev(tail_);
                if (tail_)
                {
                    next(tail_) = nullptr;
                }
                else
                {
                    head_ = nullptr;
                }
                prev(ret) = nullptr;
                return static_cast<value_ptr>(nullptr);
            }
            return static_cast<value_ptr>(nullptr);
        }
        
        template<typename Condition>
        intrusive_list remove_all_if(const Condition& condition)
        {
            intrusive_list ret_list;
            hook_ptr element = head_;
            while (element)
            {
                if (condition(static_cast<value_ptr>(element)))
                {
                    hook_ptr next_one = unlink(element);
                    ret_list.push_back(element);
                    element = next_one;
                }
                else
                {
                    element = next(element);
                }
            }
            return ret_list;
        }
        
        template<typename Condition>
        value_ptr remove_one_if_forward(const Condition& condition)
        {
            hook_ptr element = head_;
            while (element)
            {
                if (condition(static_cast<value_ptr>(element)))
                {
                    unlink(element);
                    return static_cast<value_ptr>(element);
                }
                element = next(element);
            }
            return static_cast<value_ptr>(nullptr);
        }
        
        template<typename Condition>
        value_ptr remove_one_if_retreat(const Condition& condition)
        {
            hook_ptr element = tail_;
            while (element)
            {
                if (condition(static_cast<value_ptr>(element)))
                {
                    unlink(element);
                    return static_cast<value_ptr>(element);
                }
                element = prev(element);
            }
            return static_cast<value_ptr>(nullptr);
        }
        
        template<typename Predicate>
        void for_each(const Predicate& predicate)
        {
            hook_ptr element = head_;
            while (element)
            {
                hook_ptr next_one = next(element);
                predicate(static_cast<value_ptr>(element));
                element = next_one;
            }
        }
        
        template<typename Condition, typename Predicate>
        void for_each(const Condition& condition, const Predicate& predicate)
        {
            hook_ptr element = head_;
            while (element)
            {
                hook_ptr next_one = next(element);
                if (condition(static_cast<value_ptr>(element)))
                {
                    predicate(static_cast<value_ptr>(element));
                }
                element = next_one;
            }
        }
        
        void reverse()
        {
            for (hook_ptr element = head_; ; )
            {
                hook_ptr next_one = next(element);
                next(element) = prev(element);
                prev(element) = next_one;
                if (next_one == nullptr)
                {
                    head_ = tail_;
                    tail_ = head_;
                    break;
                }
                element = next_one;
            }
        }
        
        template<typename Predicate>
        void consume_all(const Predicate& predicate)
        {
            if (!head_)
            {
                return;
            }
            while (head_)
            {
                hook_ptr next_one = next(head_);
                next(head_) = nullptr;
                prev(head_) = nullptr;
                predicate(static_cast<value_ptr>(head_));
                head_ = next_one;
            }
            tail_ = nullptr;
        }
        
        void clear(bool clearup = false)
        {
            if (!head_)
            {
                 return;
            }
            if (clearup)
            {
                hook_ptr next_one;
                while (head_)
                {
                    next_one = next(head_);
                    prev(head_) = nullptr;
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
    class intrusive_list<T, true, Tag> : protected detail::hook_access
    {
    public:
        using hook_type = bidirectional_hook<Tag>;
        using hook_ptr = hook_type*;
        using value_type = T;
        using value_ptr = value_type*;
        static_assert(std::is_base_of<hook_type, T>::value, "");
    private:
        hook_ptr head_{nullptr};
        hook_ptr tail_{nullptr};
        size_t size_{0};
        value_ptr unlink(hook_ptr element)
        {
            hook_ptr prev_one = prev(element);
            hook_ptr next_one = next(element);
            if (prev_one)
            {
                next(prev_one) = next_one;
            }
            else
            {
                head_ = next(head_);
            }
            if (next_one)
            {
                prev(next_one) = prev_one;
            }
            else
            {
                tail_ = prev(tail_);
            }
            return next_one;
        }
    public:
        intrusive_list() = default;
        intrusive_list(const intrusive_list&) = delete;
        intrusive_list& operator=(const intrusive_list&) = delete;
        intrusive_list(intrusive_list&& rhs)
        : head_(rhs.head_)
        , tail_(rhs.tail_)
        , size_(rhs.size_)
        {
            rhs.head_ = nullptr;
            rhs.tail_ = nullptr;
            rhs.size_ = 0;
        }
        
        void swap(intrusive_list& lhs)
        {
            std::swap(head_, lhs.head_);
            std::swap(tail_, lhs.tail_);
            std::swap(size_, lhs.size_);
        }
        
        void swap(hook_ptr element1, hook_ptr element2)
        {
            if(element1 == element2)
            {
                return;
            }
            if(element1 && element2)
            {
                const hook_ptr prev1 = prev(element1);
                const hook_ptr next1 = next(element1);
                if(prev1 == element2)
                {
                    const hook_ptr prev2 = prev(element2);
                    if(prev2)
                    {
                        next(prev2) = element1;
                    }
                    else
                    {
                        head_ = element1;
                    }
                    if(next1)
                    {
                        prev(next1) = element2;
                    }
                    else
                    {
                        tail_ = element2;
                    }
                    prev(element1) = prev2;
                    next(element1) = element2;
                    prev(element2) = element1;
                    next(element2) = next1;
                    return;
                }
                if(next1 == element2)
                {
                    const hook_ptr next2 = next(element2);
                    if(prev1)
                    {
                        next(prev1) = element2;
                    }
                    else
                    {
                        head_ = element2;
                    }
                    if(next2)
                    {
                        prev(next2) = element1;
                    }
                    else
                    {
                        tail_ = element1;
                    }
                    prev(element2) = prev1;
                    next(element2) = element1;
                    prev(element1) = element2;
                    next(element1) = next2;
                    return;
                }
                const hook_ptr prev2 = prev(element2);
                const hook_ptr next2 = next(element2);
                if (prev1)
                {
                    next(prev1) = element2;
                }
                else
                {
                    head_ = element2;
                }
                if (next1)
                {
                    prev(next1) = element2;
                }
                else
                {
                    tail_ = element2;
                }
                if (prev2)
                {
                    next(prev2) = element1;
                }
                else
                {
                    head_ = element1;
                }
                if (next2)
                {
                    prev(next2) = element1;
                }
                else
                {
                    tail_ = element1;
                }
                prev(element2) = prev1;
                next(element2) = next1;
                prev(element1) = prev2;
                next(element1) = next2;
            }
        }
        
        bool empty() const
        {
            return head_ == nullptr;
        }
        
        void erase(hook_ptr element)
        {
            if(element)
            {
                const hook_ptr prev_one = prev(element);
                const hook_ptr next_one = next(element);
                if (prev_one)
                {
                    next(prev_one) = next_one;
                    prev(element) = nullptr;
                }
                else
                {
                    head_ = next(head_);
                }
                if (next_one)
                {
                    prev(next_one) = prev_one;
                    next(element) = nullptr;
                }
                else
                {
                    tail_ = prev(tail_);
                }
                --size_;
            }
            else if(element == head_)
            {
                head_ = tail_ = nullptr;
            }
        }
        
        void push_back(hook_ptr element)
        {
            if(element && next(element) == nullptr && (prev(element) == nullptr))
            {
                prev(element) = tail_;
                if (tail_)
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
        
        void push_back(intrusive_list& lhs)
        {
            if(lhs.head_)
            {
                if(tail_)
                {
                    prev(lhs.head_) = tail_;
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
        
        void push_front(hook_ptr element)
        {
            if(element && prev(element) == nullptr && (next(element) == nullptr))
            {
                next(element) = head_;
                if (head_)
                {
                    prev(head_) = element;
                }
                else
                {
                    tail_ = element;
                }
                head_ = element;
                ++size_;
            }
        }
        
        void push_front(intrusive_list& lhs)
        {
            if(lhs.tail_)
            {
                if(head_)
                {
                    next(lhs.tail_) = head_;
                    prev(head_) = lhs.tail_;
                }
                else
                {
                    tail_ = lhs.tail_;
                }
                head_ = lhs.head_;
                size_ += lhs.size_;
                lhs.head_ = nullptr;
                lhs.tail_ = nullptr;
                lhs.size_ = 0;
            }
        }
        
        void insert_back(const hook_ptr where, hook_ptr element)
        {
            if(element && (next(element)==nullptr) && (prev(element) == nullptr))
            {
                if(where == tail_)
                {
                    prev(element) = tail_;
                    if (tail_)
                    {
                        next(tail_) = element;
                    }
                    else
                    {
                        head_ = element;
                    }
                    tail_ = element;
                }
                else if(where == head_)
                {
                    next(element) = head_;
                    prev(head_) = element;
                    head_ = element;
                }
                else
                {
                    hook_ptr next_one = next(where);
                    next(where) = element;
                    prev(element) = where;
                    next(element) = next_one;
                    prev(next_one) = element;
                }
                ++size_;
            }
        }
        
        void insert_back(const hook_ptr where, intrusive_list& lhs)
        {
            if(lhs.head_)
            {
                if(where == tail_)
                {
                    prev(lhs.head_) = tail_;
                    if (tail_)
                    {
                        next(tail_) = lhs.head_;
                    }
                    else
                    {
                        head_ = lhs.head_;
                    }
                    tail_ = lhs.tail_;
                }
                else if(where == head_)
                {
                    next(lhs.tail_) = head_;
                    prev(head_) = lhs.tail_;
                    head_ = lhs.head_;
                }
                else
                {
                    hook_ptr next_one = next(where);
                    next(where) = lhs.head_;
                    prev(lhs.head_) = where;
                    next(lhs.tail_) = next_one;
                    prev(next_one) = lhs.tail_;
                }
                size_ += lhs.size_;
                lhs.head_ = lhs.tail_ = nullptr;
                lhs.size_ = 0;
            }
        }
        
        void insert_front(const hook_ptr where, hook_ptr element)
        {
            if (where == element)
            {
                return;
            }
            hook_ptr prev_one = prev(where);
            next(element) = where;
            prev(element) = prev_one;
            prev(where) = element;
            if (prev_one)
            {
                next(prev_one) = element;
            }
            else
            {
                head_ = element;
            }
            ++size_;
        }
        
        void insert_front(const hook_ptr where, intrusive_list& lhs)
        {
            if (lhs.tail_)
            {
                if(where == head_)
                {
                    next(lhs.tail_) = head_;
                    if(head_)
                    {
                        prev(head_) = lhs.tail_;
                    }
                    else
                    {
                        tail_ = lhs.tail_;
                    }
                    head_ = lhs.head_;
                }
                else if(where == tail_)
                {
                    next(lhs.tail_) = tail_;
                    prev(tail_) = lhs.tail_;
                    tail_ = lhs.tail_;
                }
                else
                {
                    hook_ptr prev_one = prev(where);
                    prev(where) = lhs.tail_;
                    next(lhs.tail_) = where;
                    prev(lhs.head_) = prev_one;
                    next(prev_one) = lhs.head_;
                }
                size_ += lhs.size_;
                lhs.head_ = lhs.tail_ = nullptr;
                lhs.size_ = 0;
            }
        }
        
        value_ptr pop_front()
        {
            if(head_)
            {
                hook_ptr ret = head_;
                head_ = next(head_);
                if (head_)
                {
                    prev(head_) = nullptr;
                }
                else
                {
                    tail_ = nullptr;
                }
                next(ret) = nullptr;
                --size_;
                return static_cast<value_ptr>(ret);
            }
            return static_cast<value_ptr>(nullptr);
        }
        
        value_ptr pop_back()
        {
            if(tail_)
            {
                hook_ptr ret = tail_;
                tail_ = prev(tail_);
                if (tail_)
                {
                    next(tail_) = nullptr;
                }
                else
                {
                    head_ = nullptr;
                }
                prev(ret) = nullptr;
                --size_;
                return static_cast<value_ptr>(nullptr);
            }
            return static_cast<value_ptr>(nullptr);
        }
        
        template<typename Condition>
        intrusive_list remove_all_if(const Condition& condition)
        {
            intrusive_list ret_list;
            hook_ptr element = head_;
            while (element)
            {
                if (condition(static_cast<value_ptr>(element)))
                {
                    hook_ptr next_one = unlink(element);
                    --size_;
                    ret_list.push_back(element);
                    element = next_one;
                }
                else
                {
                    element = next(element);
                }
            }
            return ret_list;
        }
        
        template<typename Condition>
        value_ptr remove_one_if_forward(const Condition& condition)
        {
            hook_ptr element = head_;
            while (element)
            {
                if (condition(static_cast<value_ptr>(element)))
                {
                    unlink(element);
                    --size_;
                    return static_cast<value_ptr>(element);
                }
                element = next(element);
            }
            return static_cast<value_ptr>(nullptr);
        }
        
        template<typename Condition>
        value_ptr remove_one_if_retreat(const Condition& condition)
        {
            hook_ptr element = tail_;
            while (element)
            {
                if (condition(static_cast<value_ptr>(element)))
                {
                    unlink(element);
                    --size_;
                    return static_cast<value_ptr>(element);
                }
                element = prev(element);
            }
            return static_cast<value_ptr>(nullptr);
        }
        
        template<typename Predicate>
        void for_each(const Predicate& predicate)
        {
            hook_ptr element = head_;
            while (element)
            {
                hook_ptr next_one = next(element);
                predicate(static_cast<value_ptr>(element));
                element = next_one;
            }
        }
        
        template<typename Condition, typename Predicate>
        void for_each(const Condition& condition, const Predicate& predicate)
        {
            hook_ptr element = head_;
            while (element)
            {
                hook_ptr next_one = next(element);
                if (condition(static_cast<value_ptr>(element)))
                {
                    predicate(static_cast<value_ptr>(element));
                }
                element = next_one;
            }
        }
        
        void reverse()
        {
            for (hook_ptr element = head_; ; )
            {
                hook_ptr next_one = next(element);
                next(element) = prev(element);
                prev(element) = next_one;
                if (next_one == nullptr)
                {
                    hook_ptr head = head_;
                    head_ = tail_;
                    tail_ = head_;
                    break;
                }
                element = next_one;
            }
        }
        
        template<typename Predicate>
        void consume_all(const Predicate& predicate)
        {
            if (!head_)
            {
                return;
            }
            while (head_)
            {
                hook_ptr next_one = next(head_);
                next(head_) = nullptr;
                prev(head_) = nullptr;
                --size_;
                predicate(static_cast<value_ptr>(head_));
                head_ = next_one;
            }
            tail_ = nullptr;
        }
        
        void clear(bool clearup = false)
        {
            if (!head_)
            {
                return;
            }
            if (clearup)
            {
                hook_ptr next_one;
                while (head_)
                {
                    next_one = next(head_);
                    prev(head_) = nullptr;
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
