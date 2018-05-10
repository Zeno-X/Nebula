// Copyright © 2016 zendo (734181759@qq.com). All rights reserved.

#pragma once
#include <stdint.h>

namespace zendo
{

	inline const char* get_digits_lut()
	{
		static const char digits_lut_table[200] =
		{
			'0', '0', '0', '1', '0', '2', '0', '3', '0', '4', '0', '5', '0', '6', '0', '7', '0', '8', '0', '9',
			'1', '0', '1', '1', '1', '2', '1', '3', '1', '4', '1', '5', '1', '6', '1', '7', '1', '8', '1', '9',
			'2', '0', '2', '1', '2', '2', '2', '3', '2', '4', '2', '5', '2', '6', '2', '7', '2', '8', '2', '9',
			'3', '0', '3', '1', '3', '2', '3', '3', '3', '4', '3', '5', '3', '6', '3', '7', '3', '8', '3', '9',
			'4', '0', '4', '1', '4', '2', '4', '3', '4', '4', '4', '5', '4', '6', '4', '7', '4', '8', '4', '9',
			'5', '0', '5', '1', '5', '2', '5', '3', '5', '4', '5', '5', '5', '6', '5', '7', '5', '8', '5', '9',
			'6', '0', '6', '1', '6', '2', '6', '3', '6', '4', '6', '5', '6', '6', '6', '7', '6', '8', '6', '9',
			'7', '0', '7', '1', '7', '2', '7', '3', '7', '4', '7', '5', '7', '6', '7', '7', '7', '8', '7', '9',
			'8', '0', '8', '1', '8', '2', '8', '3', '8', '4', '8', '5', '8', '6', '8', '7', '8', '8', '8', '9',
			'9', '0', '9', '1', '9', '2', '9', '3', '9', '4', '9', '5', '9', '6', '9', '7', '9', '8', '9', '9'
		};
		return digits_lut_table;
	}

	inline char* itoa(uint32_t value, char* buffer)
	{
		const char* digits_lut_table = get_digits_lut();
		if( value < 10000 )
		{
			const uint32_t d1 = (value / 100) << 1;
			const uint32_t d2 = (value % 100) << 1;
			if( value >= 1000 )
            {
                *buffer++ = digits_lut_table[d1];
            }
			if( value >= 100 )
            {
                *buffer++ = digits_lut_table[d1 + 1];
            }
			if( value >= 10 )
            {
                *buffer++ = digits_lut_table[d2];
            }
			*buffer++ = digits_lut_table[d2 + 1];
		}
		else if( value < 100000000 )
		{
			// value = bbbbcccc
			const uint32_t b = value / 10000;
			const uint32_t c = value % 10000;
			const uint32_t d1 = (b / 100) << 1;
			const uint32_t d2 = (b % 100) << 1;
			const uint32_t d3 = (c / 100) << 1;
			const uint32_t d4 = (c % 100) << 1;
			if( value >= 10000000 )
            {
                *buffer++ = digits_lut_table[d1];
            }
			if( value >= 1000000 )
            {
                *buffer++ = digits_lut_table[d1 + 1];
            }
			if( value >= 100000 )
            {
                *buffer++ = digits_lut_table[d2];
            }
			buffer[0] = digits_lut_table[d2 + 1];
			buffer[1] = digits_lut_table[d3];
			buffer[2] = digits_lut_table[d3 + 1];
			buffer[3] = digits_lut_table[d4];
			buffer[4] = digits_lut_table[d4 + 1];
			buffer += 5;
		}
		else
		{
			// value = aabbbbcccc in decimal
			const uint32_t a = value / 100000000; // 1 to 42
			value %= 100000000;
			if( a >= 10 )
			{
				const unsigned i = a << 1;
				*buffer++ = digits_lut_table[i];
				*buffer++ = digits_lut_table[i + 1];
			}
			else
            {
                *buffer++ = static_cast<char>('0' + static_cast<char>(a));
            }
			const uint32_t b = value / 10000; // 0 to 9999
			const uint32_t c = value % 10000; // 0 to 9999
			const uint32_t d1 = (b / 100) << 1;
			const uint32_t d2 = (b % 100) << 1;
			const uint32_t d3 = (c / 100) << 1;
			const uint32_t d4 = (c % 100) << 1;
			buffer[0] = digits_lut_table[d1];
			buffer[1] = digits_lut_table[d1 + 1];
			buffer[2] = digits_lut_table[d2];
			buffer[3] = digits_lut_table[d2 + 1];
			buffer[4] = digits_lut_table[d3];
			buffer[5] = digits_lut_table[d3 + 1];
			buffer[6] = digits_lut_table[d4];
			buffer[7] = digits_lut_table[d4 + 1];
			buffer += 8;
		}
		return buffer;
	}

	inline char* itoa(int32_t value, char* buffer)
	{
		if( value < 0 )
		{
			*buffer++ = '-';
			value = -value;
		}
		return itoa(static_cast<uint32_t>(value), buffer);
	}

	inline char* itoa(uint64_t value, char* buffer)
	{
		const char* digits_lut_table = get_digits_lut();
		static const uint64_t kTen8 = 100000000;
		static const uint64_t kTen9 = 1000000000;
		static const uint64_t kTen10 = 10000000000UL;
		static const uint64_t kTen11 = 100000000000UL;
		static const uint64_t kTen12 = 1000000000000UL;
		static const uint64_t kTen13 = 10000000000000UL;
		static const uint64_t kTen14 = 100000000000000UL;
		static const uint64_t kTen15 = 1000000000000000UL;
		static const uint64_t kTen16 = 10000000000000000UL;
		if( value < kTen8 )
		{
			uint32_t v = static_cast<uint32_t>(value);
			if( v < 10000 )
			{
				const uint32_t d1 = (v / 100) << 1;
				const uint32_t d2 = (v % 100) << 1;
				if( v >= 1000 )
                {
                    *buffer++ = digits_lut_table[d1];
                }
				if( v >= 100 )
                {
                    *buffer++ = digits_lut_table[d1 + 1];
                }
				if( v >= 10 )
                {
                    *buffer++ = digits_lut_table[d2];
                }
				*buffer++ = digits_lut_table[d2 + 1];
			}
			else
			{
				// value = bbbbcccc
				const uint32_t b = v / 10000;
				const uint32_t c = v % 10000;
				const uint32_t d1 = (b / 100) << 1;
				const uint32_t d2 = (b % 100) << 1;
				const uint32_t d3 = (c / 100) << 1;
				const uint32_t d4 = (c % 100) << 1;
				if( value >= 10000000 )
                {
                    *buffer++ = digits_lut_table[d1];
                }
				if( value >= 1000000 )
                {
                    *buffer++ = digits_lut_table[d1 + 1];
                }
				if( value >= 100000 )
                {
                    *buffer++ = digits_lut_table[d2];
                }
				buffer[0] = digits_lut_table[d2 + 1];
				buffer[1] = digits_lut_table[d3];
				buffer[2] = digits_lut_table[d3 + 1];
				buffer[3] = digits_lut_table[d4];
				buffer[4] = digits_lut_table[d4 + 1];
				buffer += 5;
			}
		}
		else if( value < kTen16 )
		{
			const uint32_t v0 = static_cast<uint32_t>(value / kTen8);
			const uint32_t v1 = static_cast<uint32_t>(value % kTen8);
			const uint32_t b0 = v0 / 10000;
			const uint32_t c0 = v0 % 10000;
			const uint32_t d1 = (b0 / 100) << 1;
			const uint32_t d2 = (b0 % 100) << 1;
			const uint32_t d3 = (c0 / 100) << 1;
			const uint32_t d4 = (c0 % 100) << 1;
			const uint32_t b1 = v1 / 10000;
			const uint32_t c1 = v1 % 10000;
			const uint32_t d5 = (b1 / 100) << 1;
			const uint32_t d6 = (b1 % 100) << 1;
			const uint32_t d7 = (c1 / 100) << 1;
			const uint32_t d8 = (c1 % 100) << 1;
			if( value >= kTen15 )
            {
                *buffer++ = digits_lut_table[d1];
            }
			if( value >= kTen14 )
            {
                *buffer++ = digits_lut_table[d1 + 1];
            }
			if( value >= kTen13 )
            {
                *buffer++ = digits_lut_table[d2];
            }
			if( value >= kTen12 )
            {
                *buffer++ = digits_lut_table[d2 + 1];
            }
			if( value >= kTen11 )
            {
                *buffer++ = digits_lut_table[d3];
            }
			if( value >= kTen10 )
            {
                *buffer++ = digits_lut_table[d3 + 1];
            }
			if( value >= kTen9 )
            {
                *buffer++ = digits_lut_table[d4];
            }
			if( value >= kTen8 )
            {
                *buffer++ = digits_lut_table[d4 + 1];
            }
			buffer[0] = digits_lut_table[d5];
			buffer[1] = digits_lut_table[d5 + 1];
			buffer[2] = digits_lut_table[d6];
			buffer[3] = digits_lut_table[d6 + 1];
			buffer[4] = digits_lut_table[d7];
			buffer[5] = digits_lut_table[d7 + 1];
			buffer[6] = digits_lut_table[d8];
			buffer[7] = digits_lut_table[d8 + 1];
			buffer += 8;
		}
		else
		{
			const uint32_t a = static_cast<uint32_t>(value / kTen16); // 1 to 1844
			value %= kTen16;
			if( a < 10 )
            {
                *buffer++ = static_cast<char>('0' + static_cast<char>(a));
            }
			else if( a < 100 )
			{
				const uint32_t i = a << 1;
				*buffer++ = digits_lut_table[i];
				*buffer++ = digits_lut_table[i + 1];
			}
			else if( a < 1000 )
			{
				*buffer++ = static_cast<char>('0' + static_cast<char>(a / 100));
				const uint32_t i = (a % 100) << 1;
				*buffer++ = digits_lut_table[i];
				*buffer++ = digits_lut_table[i + 1];
			}
			else
			{
				const uint32_t i = (a / 100) << 1;
				const uint32_t j = (a % 100) << 1;
				buffer[0] = digits_lut_table[i];
				buffer[1] = digits_lut_table[i + 1];
				buffer[2] = digits_lut_table[j];
				buffer[3] = digits_lut_table[j + 1];
				buffer += 4;
			}
			const uint32_t v0 = static_cast<uint32_t>(value / kTen8);
			const uint32_t v1 = static_cast<uint32_t>(value % kTen8);
			const uint32_t b0 = v0 / 10000;
			const uint32_t c0 = v0 % 10000;
			const uint32_t d1 = (b0 / 100) << 1;
			const uint32_t d2 = (b0 % 100) << 1;
			const uint32_t d3 = (c0 / 100) << 1;
			const uint32_t d4 = (c0 % 100) << 1;
			const uint32_t b1 = v1 / 10000;
			const uint32_t c1 = v1 % 10000;
			const uint32_t d5 = (b1 / 100) << 1;
			const uint32_t d6 = (b1 % 100) << 1;
			const uint32_t d7 = (c1 / 100) << 1;
			const uint32_t d8 = (c1 % 100) << 1;
			buffer[0] = digits_lut_table[d1];
			buffer[1] = digits_lut_table[d1 + 1];
			buffer[2] = digits_lut_table[d2];
			buffer[3] = digits_lut_table[d2 + 1];
			buffer[4] = digits_lut_table[d3];
			buffer[5] = digits_lut_table[d3 + 1];
			buffer[6] = digits_lut_table[d4];
			buffer[7] = digits_lut_table[d4 + 1];
			buffer[8] = digits_lut_table[d5];
			buffer[9] = digits_lut_table[d5 + 1];
			buffer[10] = digits_lut_table[d6];
			buffer[11] = digits_lut_table[d6 + 1];
			buffer[12] = digits_lut_table[d7];
			buffer[13] = digits_lut_table[d7 + 1];
			buffer[14] = digits_lut_table[d8];
			buffer[15] = digits_lut_table[d8 + 1];
			buffer += 16;
		}
		return buffer;
	}

	inline char* itoa(int64_t value, char* buffer)
	{
		if( value < 0 )
		{
			*buffer++ = '-';
			value = -value;
		}
		return itoa(static_cast<uint64_t>(value), buffer);
	}

}
