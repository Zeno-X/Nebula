// Copyright © 2016 zendo (734181759@qq.com). All rights reserved.
// support lambda, std::functin, std::bind, std::packaged_task, function pointer

#pragma once
#include <type_traits>
#include <memory>

namespace zendo
{
    
    template <typename Signature, size_t StorageSize = 64>
    class fixed_function;
    
    template <typename R, typename... Args, size_t StorageSize>
    class fixed_function<R(Args...), StorageSize>
    {
    public:
        using func_ptr_type = R(*)(Args...);
        using method_type = R(*)(void*, func_ptr_type, Args...);
        using alloc_type = void(*)(void*, void*);
        using storage_type = typename std::aligned_storage<StorageSize, sizeof(size_t)>::type;
        
    private:
        union data_type
        {
            storage_type storage_;
            func_ptr_type function_ptr_;
        } data_;
        method_type method_ptr_;
        alloc_type alloc_ptr_;
        
    public:
        fixed_function() noexcept
        : method_ptr_(nullptr)
        , alloc_ptr_(nullptr)
        {
            data_.function_ptr_ = nullptr;
        }
        
        fixed_function(const fixed_function& other) noexcept
        : method_ptr_(other.method_ptr_)
        , alloc_ptr_(other.alloc_ptr_)
        {
            data_.function_ptr_ = other.data_.function_ptr_;
        }
        
        template<typename Function>
        fixed_function(Function&& object) noexcept
        : fixed_function()
        {
            //typename std::remove_cv<typename std::remove_reference<Function>::type>::type;
            using unref_type = typename std::remove_reference<Function>::type;
            static_assert(sizeof(unref_type) < StorageSize, "");
            static_assert(std::is_move_constructible<unref_type>::value, "");
            
            method_ptr_ = [] (void* object_ptr, func_ptr_type, Args...args)->R {
                return static_cast<unref_type*>(object_ptr)->operator()(args...);
            };
            
            alloc_ptr_ = [] (void* storage_ptr, void* object_ptr) {
                if( object_ptr )
                    new (storage_ptr) unref_type(std::move(*static_cast<unref_type*>(object_ptr)));
                else
                    static_cast<unref_type*>(storage_ptr)->~unref_type();
            };
            alloc_ptr_(&data_.storage_, &object);
        }
        
        template<typename Ret, typename...Params>
        fixed_function(Ret(*func_ptr)(Params...)) noexcept
        : fixed_function()
        {
            data_.function_ptr_ = func_ptr;
            method_ptr_ = [] (void*, func_ptr_type f_ptr, Args...args)->R {
                return static_cast<Ret(*)(Params...)>(f_ptr)(args...);
            };
        }
        
        fixed_function(fixed_function&& rhs) noexcept
        : fixed_function()
        {
            move_from_other(rhs);
        }
        
        ~fixed_function()
        {
            if( alloc_ptr_ )
            {
                alloc_ptr_(&data_.storage_, nullptr);
            }
        }
        
        fixed_function& operator=(const fixed_function& lhs) noexcept
        {
            method_ptr_ = lhs.method_ptr_;
            alloc_ptr_ = lhs.alloc_ptr_;
            data_.function_ptr_ = lhs.data_.function_ptr_;
            return *this;
        }
        
        fixed_function& operator=(fixed_function&& rhs) noexcept
        {
            move_from_other(rhs);
            return *this;
        }
        
        fixed_function& operator=(std::nullptr_t) noexcept
        {
            if( alloc_ptr_ )
            {
                alloc_ptr_(&data_.storage_, nullptr);
            }
            alloc_ptr_ = nullptr;
            data_.function_ptr_ = nullptr;
            method_ptr_ = nullptr;
            return *this;
        }
        
        R operator()(Args...args)
        {
            if (!method_ptr_)
            {
                throw std::bad_function_call();
            }
            return method_ptr_(&data_.storage_, data_.function_ptr_, args...);
        }
        
        constexpr static size_t standard_layout_size()
        {
            return sizeof(fixed_function);
        }
        
    private:
        void move_from_other(fixed_function& lhs) noexcept
        {
            if( this == &lhs )
            {
                return;
            }
            if( alloc_ptr_ )
            {
                alloc_ptr_(&data_.storage_, nullptr);
                alloc_ptr_ = nullptr;
            }
            else
            {
                data_.function_ptr_ = nullptr;
            }
            method_ptr_ = lhs.method_ptr_;
            lhs.method_ptr_ = nullptr;
            if( lhs.alloc_ptr_ )
            {
                alloc_ptr_ = lhs.alloc_ptr_;
                alloc_ptr_(&data_.storage_, &lhs.data_.storage_);
            }
            else
            {
                data_.function_ptr_ = lhs.data_.function_ptr_;
            }
        }
        
    };
    
}

//using namespace zendo;
//struct test_func
//{
//    int func(int num)
//    {
//        std::cout << "bind number func, args : " << num << std::endl;
//        return 0;
//    }
//};
//
//int func(int num)
//{
//    std::cout << "func ptr, args : " << num << std::endl;
//    return 0;
//}
//
//void test_fixed_function()
//{
//    test_func test_f;
//    fixed_function<int(int), 64> fixed_func = [] (int num) { std::cout << "lambda, args : " << num << std::endl; return 1; };
//    fixed_func(11);
//    fixed_func = std::bind(&test_func::func, &test_f, std::placeholders::_1);
//    fixed_func(22);
//    fixed_func = &func;
//    fixed_func(33);
//}

