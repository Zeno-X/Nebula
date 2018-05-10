// Copyright © 2016 zendo (734181759@qq.com). All rights reserved.

#pragma once
#include <type_traits>
#include <stdint.h>

namespace zendo
{

	template<typename FlaotingPoint>
	inline bool atod(const char* str, FlaotingPoint& value)
	{
		static_assert(std::is_floating_point<FlaotingPoint>::value, "");
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
		FlaotingPoint ret = ch - '0';
		while ((ch = *(++str)))
		{
			if (!isdigit(ch))
            {
                break;
            }
			ret = ret * 10 + (ch - '0');
		}
		if (ch == '.')
		{
			FlaotingPoint decimals = 0.1;
			while ((ch = *(++str)))
			{
				if (!isdigit(ch))
                {
                    break;
                }
				ret += (ch - '0') * decimals;
				decimals *= 0.1;
			}
		}
		value = sign ? -ret : ret;
		return true;
	}

}
