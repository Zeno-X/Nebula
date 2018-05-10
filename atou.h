// Copyright © 2016 zendo (734181759@qq.com). All rights reserved.

#pragma once
#include <type_traits>
#include <stdint.h>

#if defined(_MSC_VER)
#pragma warning(disable:4244)
#endif

namespace zendo
{
    
    template<typename Integer>
    inline bool atou(const char* str, Integer& value)
    {
        static_assert(std::is_integral<Integer>::value, "");
        char ch = *str;
        if (!isdigit(ch))
        {
            return false;
        }
        value = ch - '0';
        for(ch = *(++str); ch; ch = *(++str))
        {
            if (!isdigit(ch))
            {
                break;
            }
            value = value * 10 + (ch - '0');
        }
        return true;
    }
    
    template<typename Integer>
    inline bool atou(const char* str, size_t len, Integer& value)
    {
        static_assert(std::is_integral<Integer>::value, "");
        if (!isdigit(*str))
        {
            return false;
        }
        value = 0;
        switch( len )
        {
            case 20:    value += (str[len - 20] - '0') * 10000000000000000000UL;
            case 19:    value += (str[len - 19] - '0') * 1000000000000000000UL;
            case 18:    value += (str[len - 18] - '0') * 100000000000000000UL;
            case 17:    value += (str[len - 17] - '0') * 10000000000000000UL;
            case 16:    value += (str[len - 16] - '0') * 1000000000000000UL;
            case 15:    value += (str[len - 15] - '0') * 100000000000000UL;
            case 14:    value += (str[len - 14] - '0') * 10000000000000UL;
            case 13:    value += (str[len - 13] - '0') * 1000000000000UL;
            case 12:    value += (str[len - 12] - '0') * 100000000000UL;
            case 11:    value += (str[len - 11] - '0') * 10000000000UL;
            case 10:    value += (str[len - 10] - '0') * 1000000000;
            case  9:    value += (str[len - 9] - '0')  * 100000000;
            case  8:    value += (str[len - 8] - '0')  * 10000000;
            case  7:    value += (str[len - 7] - '0')  * 1000000;
            case  6:    value += (str[len - 6] - '0')  * 100000;
            case  5:    value += (str[len - 5] - '0')  * 10000;
            case  4:    value += (str[len - 4] - '0')  * 1000;
            case  3:    value += (str[len - 3] - '0')  * 100;
            case  2:    value += (str[len - 2] - '0')  * 10;
            case  1:    value += (str[len - 1] - '0');
                return true;
            default:
                return false;
        }
    }
    
}

