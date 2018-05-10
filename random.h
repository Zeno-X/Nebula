// Copyright © 2016 zendo (734181759@qq.com). All rights reserved.

#pragma once
#include <random>
#include <type_traits>

namespace zendo
{
    
    template<size_t Bytes>
    class random_context
    {
    public:
        static_assert(Bytes <= 8, "Bits should be less than or equal to 64 bits");
        using mt19937_type = typename std::conditional<Bytes <= 4, std::mt19937, std::mt19937_64>::type;
        using unsigned_integer_type = typename std::conditional<Bytes <= 4, uint32_t, uint64_t>::type;
    protected:
        std::random_device& device_;
        mt19937_type engine_;// Mersenne Twister
    public:
        random_context() = delete;
        random_context(std::random_device& device)
        : device_(device)
        , engine_(device_())
        {
        }
        
        std::random_device& get_device()
        {
            return device_;
        }
        
        mt19937_type& get_engine()
        {
            return engine_;
        }
        
        size_t get_bytes() const
        {
            return Bytes;
        }
        
    };
        
        namespace detail
        {
            
            template<typename T>
            class unint_random
            {
            public:
                static_assert(sizeof(T) <= 8, "The result of sizeof(T) should be <= 8");
                using context_type = random_context<sizeof(T)>;
                using unsigned_integer_type = typename context_type::unsigned_integer_type;
                using mt19937_type = typename context_type::mt19937_type;
            protected:
                context_type& ctx_;
            public:
                unint_random() = delete;
                
                unint_random(context_type& ctx)
                : ctx_(ctx)
                {
                }
                
                std::random_device& get_device()
                {
                    return ctx_.get_device();
                }
                
                mt19937_type& get_engine()
                {
                    return ctx_.get_engine();
                    
                }
                
                // [0, 2^bits)
                T get()
                {
                    return get_engine()();
                }
                
                // [0, scope)
                unsigned_integer_type get(unsigned_integer_type scope)
                {
                    return get() % scope;
                }
                
                // [lower_bound, upper_bound)
                T get(unsigned_integer_type lower_bound, unsigned_integer_type upper_bound)
                {
                    return get() % (upper_bound - lower_bound) + lower_bound;
                }
                
            };
            
            template<typename T>
            class floating_random
            {
            public:
                static_assert(sizeof(T) <= 8, "The result of sizeof(T) should be <= 8");
                using context_type = random_context<sizeof(T)>;
                using mt19937_type = typename context_type::mt19937_type;
                using floating_point_type = T;
                using distribution_type = std::uniform_real_distribution<T>;
            protected:
                context_type& ctx_;
            public:
                floating_random() = delete;
                floating_random(context_type& ctx) : ctx_(ctx) { }
                
                std::random_device& get_device()
                {
                    return ctx_.get_device();
                }
                
                mt19937_type& get_engine()
                {
                    return ctx_.get_engine();
                }
                
                // [0.0, 1.0)
                floating_point_type get()
                {
                    return distribution_type()(get_engine());
                }
                
                // [0, upper_bound)
                floating_point_type get(floating_point_type upper_bound)
                {
                    return distribution_type(floating_point_type(0), upper_bound)(get_engine());
                }
                
                // [lower_bound, upper_bound)
                floating_point_type get(floating_point_type lower_bound, floating_point_type upper_bound)
                {
                    return distribution_type(lower_bound, upper_bound)(get_engine());
                }
                
            };
            
        }
        
        //template<typename T> using random = typename std::conditional<std::is_integral<T>::value,
        //    typename std::enable_if<std::is_unsigned<T>::value, detail::unint_random<T>>::type,
        //    typename std::enable_if<std::is_floating_point<T>::value, detail::floating_random<T>>::type>::type;
        
        template<typename T> using random = typename std::conditional<std::is_integral<T>::value, detail::unint_random<T>, detail::floating_random<T> >::type;
        
        }
        
        //void test_random(size_t iterations)
        //{
        //    std::random_device rd;
        //    random_context<4> ctx(rd);
        //
        //    random<float> floatdm(ctx);
        //    random<uint32_t> intrdm(ctx);
        //
        //    for( size_t i = 0; i < iterations; ++i )
        //    {
        //        float flt = floatdm.get(1, 2);
        //        uint32_t inter = intrdm.get(1, 2);
        //    }
        //}
        
        
