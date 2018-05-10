//// Copyright © 2016 zendo (734181759@qq.com). All rights reserved.
//// support lambda, std::functin, std::bind, std::packaged_task, function pointer
//
//#pragma once
//#include <type_traits>
//#include <memory>
//#include "traits.h"
//#include "compressed_allocator.h"
//
//namespace zendo
//{
//
//    namespace detail
//    {
//        ZENDO_HAS_MEMBER(allocate);
//    }
//
//
//    template <typename Signature> class flexible_function;
//
//    template <typename R, typename... Args>
//    class flexible_function<R(Args...)>
//    {
//
//        class std_allocator
//        {
//        private:
//            std::allocator<uint8_t> underlying_;
//        public:
//            void* allocate(size_t size)
//            {
//                return static_cast<void*>(underlying_.allocate(size));
//            }
//
//            void deallocate(void* ptr)
//            {
//                underlying_.deallocate(static_cast<uint8_t*>(ptr), 0);
//            }
//        };
//
//        template<typename> struct is_std_allocator:std::false_type{};
//        template<typename T> struct is_std_allocator<std::allocator<T>>:std::true_type{};
//
//        template<typename AmbiguousAllocator>
//        class allocator_traits
//        {
//        public:
//            using type = typename std::conditional<is_std_allocator<AmbiguousAllocator>::value, std_allocator, AmbiguousAllocator>::type;
//        };
//
//
//    public:
//        using executor_type = R(*)(void*, Args...);
//        using allocate_handler = void(*)(flexible_function*, void*, size_t);
//    private:
//        allocate_handler allocate_handler_;
//        executor_type executor_;
//        void* allocator_;
//        size_t size_;
//        void* data_;
//
//        template<typename Alloactor>
//        static void call_alloc(Alloactor& allocator, flexible_function* self, void* data, size_t size)
//        {
//            if(size == 0)
//            {
//                if(self->data_)
//                {
//                    allocator.deallocate(self->data_);
//                    self->data_ = nullptr;
//                    self->size_ = 0;
//                }
//            }
//            else if(self->size_ < size)
//            {
//                if(self->data_)
//                {
//                    allocator.deallocate(self->data_);
//                    self->size_ = 0;
//                }
//                if(data)
//                {
//                    self->data_ = allocator.allocate(size);
//                    if(self->data_)
//                    {
//                        memcpy(self->data_, data, size);
//                        self->size_ = size;
//                    }
//                }
//            }
//            else if(data && self->data_)
//            {
//                memcpy(self->data_, data, size);
//            }
//        }
//
//
//        template<typename Alloactor, typename Function>
//        static void call_alloc(Alloactor& allocator, flexible_function* self, Function* func, size_t size)
//        {
//            using unref_type = typename std::remove_reference<Function>::type;
//            if(size == 0)
//            {
//                if(self->data_)
//                {
//                    static_cast<unref_type*>(self->data_)->~unref_type();
//                    allocator.deallocate(self->data_);
//                    self->data_ = nullptr;
//                    self->size_ = 0;
//                }
//            }
//            else if(self->size_ < size)
//            {
//                if(self->data_)
//                {
//                    static_cast<unref_type*>(self->data_)->~unref_type();
//                    allocator.deallocate(self->data_);
//                    self->size_ = 0;
//                }
//                if(func)
//                {
//                    self->data_ = allocator.allocate(size);
//                    if(self->data_)
//                    {
//                        new (self->data_) unref_type(std::move(*static_cast<unref_type*>(func)));
//                        self->size_ = size;
//                    }
//                }
//            }
//            else if(func && self->data_)
//            {
//                new (self->data_) unref_type(std::move(*static_cast<unref_type*>(func)));
//            }
//        }
//
//        void construct(std::integral_constant<int,0>&)
//        {
//        }
//
//    public:
//        flexible_function()
//        : executor_(nullptr)
//        , allocator_(nullptr)
//        , size_(0)
//        , data_(nullptr)
//        {
//            allocate_handler_ = [](flexible_function* self, void* data, size_t size)
//            {
//                std_allocator allocator;
//                call_alloc(allocator, self, data, size);
//            };
//        }
//
//        flexible_function(flexible_function&& rhs)
//        : allocate_handler_(rhs.allocate_handler_)
//        , executor_(rhs.executor_)
//        , allocator_(rhs.allocator_)
//        , size_(rhs.size_)
//        , data_(rhs.data_)
//        {
//            rhs.size_ = 0;
//            rhs.data_ = nullptr;
//        }
//
//        flexible_function(const flexible_function& lhs)
//        : allocate_handler_(lhs.allocate_handler_)
//        , executor_(lhs.executor_)
//        , allocator_(lhs.allocator_)
//        {
//            allocate_handler_(this, lhs.data_, lhs.size_);
//        }
//
//        template<typename Alloactor >
//        flexible_function(Alloactor& allocator)
//        : executor_(nullptr)
//        , allocator_(is_std_allocator<Alloactor>::value ? nullptr : static_cast<void*>(&allocator))
//        , size_(0)
//        , data_(nullptr)
//        {
//            using allocator_type = typename std::remove_reference<Alloactor>::type;
//            allocate_handler_ = [](flexible_function* self, void* data, size_t size){
//                if(self->allocator_)
//                {
//                    allocator_type& allocator = *(static_cast<allocator_type*>(self->allocator_));
//                    call_alloc(allocator, self, data, size);
//                }
//                else
//                {
//                    std_allocator allocator;
//                    call_alloc(allocator, self, data, size);
//                }
//            };
//        }
//
//        template<typename Function>
//        flexible_function(Function&& function)
//        : allocator_(nullptr)
//        , size_(sizeof(Function))
//        {
//            using unref_type = typename std::remove_reference<Function>::type;
//            data_ = new unref_type(std::move(function));
//            allocate_handler_ = [](flexible_function* self, void* data, size_t size){
//                std_allocator allocator;
//                call_alloc(allocator, self, static_cast<unref_type*>(data), size);
//            };
//
//            executor_ = [] (void* data, Args...args)->R {
//                return (static_cast<unref_type*>(data))->operator()(args...);
//            };
//        }
//
//        template<typename Function, typename Alloactor>
//        flexible_function(Function&& function, Alloactor& allocator)
//        : allocator_(is_std_allocator<Alloactor>::value ? nullptr : static_cast<void*>(&allocator))
//        , size_(sizeof(Function))
//        , data_(allocator.allocate(size_))
//        {
//            using unref_type = typename std::remove_reference<Function>::type;
//            new(data_)unref_type(std::move(function));
//            allocate_handler_ = [](flexible_function* self, void* data, size_t size){
//                if(self->allocator_)
//                {
//                    call_alloc(*static_cast<Alloactor*>(self->allocator_), self, static_cast<Function*>(data), size);
//                }
//                else
//                {
//                    std_allocator allocator;
//                    call_alloc(allocator, self, static_cast<Function*>(data), size);
//                }
//            };
//
//            executor_ = [] (void* data, Args...args)->R {
//                return static_cast<unref_type*>(data)->operator()(args...);
//            };
//        }
//
//        ~flexible_function()
//        {
//            allocate_handler_(this, nullptr, 0);
//        }
//
//    };
//
//}

