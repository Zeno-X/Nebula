// Copyright © 2016 zendo (734181759@qq.com). All rights reserved.

#pragma once
#include <memory>

namespace zendo
{
    
    namespace details
    {
        
        template<size_t> struct atomic_struct_raw;
        template<> struct atomic_struct_raw<1>{ using type = uint8_t;  };
        template<> struct atomic_struct_raw<2>{ using type = uint16_t; };
        template<> struct atomic_struct_raw<3>{ using type = uint32_t; };
        template<> struct atomic_struct_raw<4>{ using type = uint32_t; };
        template<> struct atomic_struct_raw<5>{ using type = uint64_t; };
        template<> struct atomic_struct_raw<6>{ using type = uint64_t; };
        template<> struct atomic_struct_raw<7>{ using type = uint64_t; };
        template<> struct atomic_struct_raw<8>{ using type = uint64_t; };
        
        inline std::memory_order default_failure_memory_order(std::memory_order order)
        {
            switch (order)
            {
                case std::memory_order_acq_rel:
                    return std::memory_order_acquire;
                case std::memory_order_release:
                    return std::memory_order_relaxed;
                default:
                    return order;
            }
        }
        
    }

    template<typename T, template<typename> class Atomic = std::atomic>
    class atomic_struct
    {
    public:
        using value_type = T;
        using raw_type = typename details::atomic_struct_raw<sizeof(T)>::type;
        using atomic_type = Atomic<raw_type>;
        static_assert(alignof(value_type) <= alignof(raw_type), "");
        static_assert(sizeof(value_type) <= sizeof(raw_type), "");
        static_assert(std::is_trivial<value_type>::value || std::is_trivially_copyable<T>::value, "");
    private:
        atomic_type data_;
        static raw_type encode(value_type value) noexcept
        {
            raw_type ret = 0;
            memcpy(&ret, &value, sizeof(value_type));
            return ret;
        }
        
        static value_type decode(raw_type raw) noexcept
        {
            value_type ret;
            memcpy(&ret, &raw, sizeof(value_type));
            return ret;
        }
    public:
        atomic_struct() = default;
        ~atomic_struct() = default;
        atomic_struct(const atomic_struct&) = delete;
        atomic_struct& operator= (const atomic_struct&) = delete;
        
        constexpr atomic_struct(value_type value) noexcept
        : data_(encode(value))
        {
        }
        
        bool is_lock_free() const noexcept
        {
            return data_.is_lock_free();
        }
        
        bool compare_exchange_strong(value_type& expected, value_type value, std::memory_order order = std::memory_order_seq_cst) noexcept
        {
            return compare_exchange_strong(expected, value, order, details::default_failure_memory_order(order));
        }
        
        bool compare_exchange_strong(value_type& expected, value_type value, std::memory_order success, std::memory_order failure) noexcept
        {
            raw_type data = encode(expected);
            if(data_.compare_exchange_strong(data, encode(value), success, failure))
            {
                return true;
            }
            expected = decode(data);
            return false;
        }
        
        bool compare_exchange_weak(value_type& expected, value_type value, std::memory_order order = std::memory_order_seq_cst) noexcept
        {
            return compare_exchange_weak(expected, value, order, details::default_failure_memory_order(order));
        }
        
        bool compare_exchange_weak(value_type& expected, value_type value, std::memory_order success, std::memory_order failure) noexcept
        {
            raw_type data = encode(expected);
            if(data_.compare_exchange_weak(data, encode(value), success, failure))
            {
                return true;
            }
            expected = decode(data);
            return false;
        }
        
        value_type exchange(value_type value, std::memory_order order = std::memory_order_seq_cst) noexcept
        {
            return decode(data_.exchange(encode(value), order));
        }
        
        operator value_type() const noexcept
        {
            return decode(data_);
        }
        
        value_type load(std::memory_order order = std::memory_order_seq_cst) const noexcept
        {
            return decode(data_.load(order));
        }
        
        value_type operator= (value_type value) noexcept
        {
            return decode(data_ = encode(value));
        }
        
        void store(value_type value, std::memory_order order = std::memory_order_seq_cst) noexcept
        {
            data_.store(encode(value), order);
        }
        
    };
    
}
