// Copyright © 2016 zendo (734181759@qq.com). All rights reserved.

#pragma once
#include <type_traits>
#include <algorithm>

namespace zendo
{
    
    namespace detail
    {
        class hook_access
        {
        protected:
            template<typename Hook> static Hook*& next(Hook* hook)
            {
                return hook->next_;
            }
            
            template<typename Hook> static Hook*& prev(Hook* hook)
            {
                return hook->prev_;
            }
            
        };
        
        class rb_hook_access
        {
        protected:
            template<typename Hook> static size_t level(Hook* hook)
            {
                Hook* p = parent(hook);
                return p ? (level(p) + 1) : 1;
            }
            
            template<typename Hook> static unsigned long color(Hook* hook)
            {
                return hook->link_.parent_color_ & 1;
            }
            
            template<typename Hook> static bool is_red(Hook* hook)
            {
                return !(hook->link_.parent_color_ & 1);
            }
            
            template<typename Hook> static bool is_black(Hook* hook)
            {
                return hook->link_.parent_color_ & 1;
            }
            
            template<typename Hook> static bool is_left_child(Hook* hook)
            {
                Hook* p = parent(hook);
                return p && (p->left_ == hook);
            }
            
            template<typename Hook> static bool is_right_child(Hook* hook)
            {
                Hook* p = parent(hook);
                return p && (p->right_ == hook);
            }
            
            template<typename Hook> static bool is_leaf(Hook* hook)
            {
                return (!(hook->left_)) && (!(hook->right_));
            }
            
            template<typename Hook> static bool is_root(Hook* hook)
            {
                return !parent(hook);
            }
            
            template<typename Hook> static Hook*& right(Hook* hook)
            {
                return hook->right_;
            }
            
            template<typename Hook> static Hook*& left(Hook* hook)
            {
                return hook->left_;
            }
            
            template<typename Hook> static Hook*& parent(Hook* hook)
            {
                return reinterpret_cast<Hook*>(hook->link_.parent_color_ & ~3);
            }
            
            template<typename Hook> static Hook* grandparent(Hook* hook)
            {
                Hook* p = parent(hook);
                return p?(parent(p)):nullptr;
            }
            
            template<typename Hook> static Hook* grandparent_frome_parent(Hook* parent_hook)
            {
                return parent(parent_hook);
            }
            
            template<typename Hook> static Hook* sibling(Hook* hook)
            {
                Hook* p = parent(hook);
                return p?(sibling_from_parent(p)):nullptr;
            }
            
            template<typename Hook> static Hook* sibling_from_parent(Hook* parent_hook)
            {
                return (parent_hook->left_)?(parent_hook->right_):(parent_hook->left_);
            }
            
            template<typename Hook> static Hook* uncle(Hook* hook)
            {
                Hook* p = parent(hook);
                if(p)
                {
                    Hook* gp = grandparent_frome_parent(p);
                    if(gp)
                    {
                        return (p == gp->right_)?(gp->left_):(gp->right_);
                    }
                }
                return nullptr;
            }
            
            template<typename Hook> static Hook* uncle_from_parent(Hook* parent_hook)
            {
                Hook* gp = parent(parent_hook);
                return gp?((parent == gp->right_)? (gp->left_) : (gp->right_)):nullptr;
            }
            
            template<typename Hook> static Hook* min(Hook* hook)
            {
                while( hook && hook->left_ )
                {
                    hook = hook->left_;
                }
                return hook;
            }
            
            template<typename Hook> static Hook* max(Hook* hook)
            {
                while( hook && hook->right_ )
                {
                    hook = hook->right_;
                }
                return hook;
            }
            
            template<typename Hook> static void set_red(Hook* hook)
            {
                do
                {
                    hook->link_.parent_color_ &= ~1;
                }
                while (0);
            }
            
            template<typename Hook> static void set_black(Hook* hook)
            {
                do
                {
                    hook->link_.parent_color_ |= 1;
                } while (0);
            }
            
            template<typename Hook> static void set_parent(Hook* hook, Hook* parent_hook)
            {
                hook->link_.parent_color_ = (hook->link_.parent_color_ & 3) | reinterpret_cast<unsigned long>(parent_hook);
            }
            
            template<typename Hook> static void clear(Hook* hook) { hook->link_.parent_color_ = 0; }
            template<typename Hook> static unsigned long get_color(Hook* hook) { return hook->link_.parent_color_ & 1; }
        };
    }
    

    
    template<int Tag = 0>
    class forward_hook
    {
        forward_hook* next_{nullptr};
        friend class detail::hook_access;
    };
    
    template<int Tag = 0>
    class bidirectional_hook
    {
        bidirectional_hook* prev_{nullptr};
        bidirectional_hook* next_{nullptr};
        friend class detail::hook_access;
    };
    
    
    template<typename Key, int Tag = 0>
    class rb_hook
    {
    public:
        using key_type = Key;
    private:
        friend class detail::hook_access;
        struct alignas(sizeof(long)) link
        {
            unsigned long parent_color_{0};
            rb_hook* right_{nullptr};
            rb_hook* left_{nullptr};
        }link_;
        key_type key_;
    public:
        template<typename...Args>
        rb_hook(Args&&...args)
        : key_(std::forward<Args>(args)...)
        {
        }
    };

}
