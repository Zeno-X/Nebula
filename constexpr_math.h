// Copyright © 2016 zendo (734181759@qq.com). All rights reserved.

#pragma once
#include <type_traits>

namespace zendo
{
    
    template<typename T>
    constexpr T constexpr_max(T a)
    {
        return a;
    }
    
    template<typename T, typename U, typename...Args>
    constexpr auto constexpr_max(T a, U b, Args...args)
    {
        return (a < b) ? constexpr_max(b, args...) : constexpr_max(a, args...);
    }
    
    template<typename T>
    constexpr T constexpr_min(T a)
    {
        return a;
    }
    
    template<typename T, typename U, typename...Args>
    constexpr auto constexpr_min(T a, U b, Args...args)
    {
        return (a > b) ? constexpr_min(b, args...) : constexpr_min(a, args...);
    }
    
    template<typename T, typename U, typename W>
    constexpr auto constexpr_clamp(T a, U b, W c)
    {
        return (a < b) ? b : (c < a) ? c : a;
    }
    
    namespace detail
    {
        template<typename T, typename = void>
        struct constexpr_abs_helper{};
        
        template<typename T>
        struct constexpr_abs_helper<T, typename std::enable_if<std::is_floating_point<T>::value>::type>
        {
            static constexpr T invoke(T val)
            {
                return (val < static_cast<T>(0)) ? (-val) : val;
            }
        };
        
        template<typename T>
        struct constexpr_abs_helper<T, typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value && std::is_unsigned<T>::value>::type>
        {
            static constexpr T invoke(T val)
            {
                return val;
            }
        };
        
        template<typename T>
        struct constexpr_abs_helper<T, typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value && std::is_signed<T>::value>::type>
        {
            static constexpr typename std::make_unsigned<T>::type invoke(T val)
            {
                return typename std::make_unsigned<T>::type((val < static_cast<T>(0)) ? (-val) : val);
            }
        };
    }
    
    template<typename T>
    constexpr auto constexpr_abs(T val)-> decltype(detail::constexpr_abs_helper<T>::invoke(val))
    {
        return detail::constexpr_abs_helper<T>::invoke(val);
    }
    
    template<typename T>
    constexpr T constexpr_square(T val)
    {
        return val * val;
    }
    
    template<typename T>
    constexpr T constexpr_cube(T val)
    {
        return val * val * val;
    }
    
    namespace detail
    {
        template<typename T>
        constexpr T constexpr_log2_internal(T a, T b)
        {
            return (b == static_cast<T>(1)) ? a : constexpr_log2_internal(a + static_cast<T>(1), b / static_cast<T>(2));
        }
        
        template<typename T>
        constexpr T constexpr_log2_ceil_internal(T a, T b)
        {
            return a + static_cast<T>(static_cast<T>(1) << a < b ? 1 : 0);
        }
    }
    
    template<typename T>
    constexpr T constexpr_log2(T val)// log2(val)
    {
        return detail::constexpr_log2_internal(static_cast<T>(0), val);
    }
    
    template<typename T>
    constexpr T constexpr_log2_ceil(T val)
    {
        return detail::constexpr_log2_ceil_internal(constexpr_log2(val), val);
    }
    
    template<typename T>
    constexpr T constexpr_ceil(T val, T round)
    {
        return round == static_cast<T>(0) ? val : ((val + (val < static_cast<T>(0) ? static_cast<T>(0) : round - static_cast<T>(1))) / round) * round;
    }
    
    template<typename T>
    constexpr T constexpr_pow(T base, std::size_t exp)
    {
        return (exp == 0) ? static_cast<T>(1) : (exp == 1) ? base : constexpr_square(constexpr_pow(base, exp / 2)) * (exp % 2 ? base : static_cast<T>(1));
    }
    
    template<typename T>
    constexpr bool constexpr_is_power_of_2(T value)
    {
        static_assert(std::is_integral<T>::value, "Argument must be a integral type.");
        return value && !(value & (value - 1));
    }
    
    template<typename T>
    constexpr T constexpr_round_up_power_of_2(T value)
    {
        static_assert(std::is_integral<T>::value && (sizeof(T) == 4), "Argument must be a 32-bit integer type.");
        --value;
        value |= value >> 1;
        value |= value >> 2;
        value |= value >> 4;
        value |= value >> 8;
        value |= value >> 16;
        return ++value;
    }
    
    template <typename T>
    constexpr T constexpr_add_overflow_clamped(T a, T b)
    {
        using L = std::numeric_limits<T>;
        using M = std::intmax_t;
        static_assert(!std::is_integral<T>::value || sizeof(T) <= sizeof(M), "Integral type too large!");
        return !std::is_integral<T>::value ? a + b : sizeof(T) < sizeof(M)
        ? T(constexpr_clamp(M(a) + M(b), M(L::min()), M(L::max()))) : !(a < 0) ? a + constexpr_min(b, T(L::max() - a)) : !(b < 0) ? a + b : a + constexpr_max(b, T(L::min() - a));
    }
    
    template <typename T>
    constexpr T constexpr_sub_overflow_clamped(T a, T b)
    {
        using L = std::numeric_limits<T>;
        using M = std::intmax_t;
        static_assert(!std::is_integral<T>::value || sizeof(T) <= sizeof(M), "Integral type too large!");
        return !std::is_integral<T>::value ? a - b : std::is_unsigned<T>::value ? (a < b ? 0 : a - b) : sizeof(T) < sizeof(M)
        ? T(constexpr_clamp(M(a) - M(b), M(L::min()), M(L::max()))) : (a < 0) == (b < 0) ? a - b : L::min() < b ? constexpr_add_overflow_clamped(a, T(-b)) : a < 0 ? a - b : L::max();
    }
    
    namespace detail
    {
        template<typename Char>
        constexpr size_t constexpr_strlen_internal(const Char* str, size_t len)
        {
            return (*str == Char(0)) ? len : constexpr_strlen_internal(str + 1, len + 1);
        }
    }
    
    
    template<typename Char>
    constexpr size_t constexpr_strlen(const Char* str)
    {
        return detail::constexpr_strlen_internal(str, 0);
    }
    
    template <>
    constexpr size_t constexpr_strlen(const char* str)
    {
#if defined(__clang__)
        return __builtin_strlen(str);
#elif defined(_MSC_VER) || defined(__CUDACC__)
        return detail::constexpr_strlen_internal(str, 0);
#else
        return std::strlen(str);
#endif
    }
    
}
