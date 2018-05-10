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
	inline bool atoi(const char* str, Integer& value)
	{
		static_assert(std::is_integral<Integer>::value, "");
		bool sign = false;
		if (str[0] == '-')
		{
			sign = true;
			++str;
		}
		char ch = *str;
		if (!isdigit(ch))
        {
            return false;
        }
		Integer ret = ch - '0';
        for(ch = *(++str); ch; ch = *(++str))
        {
            if (!isdigit(ch))
            {
                break;
            }
            ret = ret * 10 + (ch - '0');
        }
		value = sign ? -ret : ret;
		return true;
	}

	template<typename Integer>
	inline bool atoi(const char* str, size_t len, Integer& value)
	{
		static_assert(std::is_integral<Integer>::value, "");
		bool sign = false;
		if( str[0] == '-' )
		{
			sign = true;
			++str;
			--len;
		}
		Integer ret = 0;
		switch( len )
		{
			case 20:    ret += (str[len - 20] - '0') * 10000000000000000000UL;
			case 19:    ret += (str[len - 19] - '0') * 1000000000000000000UL;
			case 18:    ret += (str[len - 18] - '0') * 100000000000000000UL;
			case 17:    ret += (str[len - 17] - '0') * 10000000000000000UL;
			case 16:    ret += (str[len - 16] - '0') * 1000000000000000UL;
			case 15:    ret += (str[len - 15] - '0') * 100000000000000UL;
			case 14:    ret += (str[len - 14] - '0') * 10000000000000UL;
			case 13:    ret += (str[len - 13] - '0') * 1000000000000UL;
			case 12:    ret += (str[len - 12] - '0') * 100000000000UL;
			case 11:    ret += (str[len - 11] - '0') * 10000000000UL;
			case 10:    ret += (str[len - 10] - '0') * 1000000000;
			case  9:    ret += (str[len - 9] - '0') * 100000000;
			case  8:    ret += (str[len - 8] - '0') * 10000000;
			case  7:    ret += (str[len - 7] - '0') * 1000000;
			case  6:    ret += (str[len - 6] - '0') * 100000;
			case  5:    ret += (str[len - 5] - '0') * 10000;
			case  4:    ret += (str[len - 4] - '0') * 1000;
			case  3:    ret += (str[len - 3] - '0') * 100;
			case  2:    ret += (str[len - 2] - '0') * 10;
			case  1:    ret += (str[len - 1] - '0');
				value = sign ? -ret : ret;
				return true;
			default:
				return false;
		}
	}

}
