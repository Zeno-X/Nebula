// Copyright © 2016 zendo (734181759@qq.com). All rights reserved.
// support lambda, std::functin, std::bind, std::packaged_task, function pointer

#pragma once
#include <type_traits>
#include <memory>

namespace zendo
{
    
    template<typename T, typename Alloc>
    class compressed_allocator
    {
    private:
        Alloc& alloc_;
        
    public:
        compressed_allocator(Alloc& alloc)
        : alloc_(alloc)
        {
        }
        
        T* allocate(size_t n)
        {
            return static_cast<T*>(alloc_.allocate(sizeof(T)*n));
        }
        
        template<typename... Args>
        T* construct(void* ptr, Args&&... args )
        {
            return new(ptr)T(std::forward<Args>(args)...);
        }
        
        void destroy(T* ptr)
        {
            ptr->~T();
        }
        
        void deallocate(T* ptr)
        {
            alloc_.deallocate(ptr);
        }
        
        void deallocate(T* ptr, size_t)
        {
            alloc_.deallocate(ptr);
        }
    };
    
}
