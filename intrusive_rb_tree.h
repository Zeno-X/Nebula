// Copyright © 2016 zendo (734181759@qq.com). All rights reserved.

#pragma once
#include "intrusive_hook.h"

namespace zendo
{
    
    namespace detail
    {
        
        template<typename Hook>
        class intrusive_rb_iterator : protected detail::rb_hook_access
        {
            using hook_ptr = Hook*;
            hook_ptr nonius_;
            
            void inc()
            {
                if( nonius_ )
                {
                    hook_ptr r = right(nonius_);
                    if( r )
                    {
                        nonius_ = min(r);
                    }
                    else if( is_left_child(nonius_) )
                    {
                        nonius_ = parent(nonius_);
                    }
                    else
                    {
                        while( nonius_->is_right_child() )
                        {
                            nonius_ = parent(nonius_);
                        }
                        nonius_ = parent(nonius_);
                    }
                }
            }
            
            void dec()
            {
                if( nonius_ )
                {
                    hook_ptr l = left(nonius_);
                    if( l )
                    {
                        nonius_ = max(l);
                    }
                    else if( is_right_child(nonius_) )
                    {
                        nonius_ = parent(nonius_);
                    }
                    else
                    {
                        while( nonius_->is_left_child() )
                            nonius_ = nonius_->parent_;
                        nonius_ = nonius_->parent_;
                    }
                }
            }
        };
        
        template<typename Key, typename T, bool CacheSize, int Tag = 0>
        class intrusive_rb_tree;
        
        template<typename Key, typename T, int Tag>
        class intrusive_rb_tree<Key, T, false, Tag> : protected detail::rb_hook_access
        {
        public:
            using value_type = T;
            using value_ptr = value_type*;
            using key_type = Key;
            using hook_type = rb_hook<Key, Tag>;
            using hook_ptr = hook_type*;
            static_assert(std::is_base_of<hook_type, T>::value, "");
        private:
        };
        
    }
    
}
