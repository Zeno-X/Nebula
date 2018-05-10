// Copyright © 2016 zendo (734181759@qq.com). All rights reserved.

#pragma once
#include <memory>
#include "itoa.h"

#if defined(_MSC_VER) && defined(_M_AMD64)
#include <intrin.h> 
#endif

#define ZENO_MAKE_UINT64(high32, low32) ((static_cast<uint64_t>(high32) << 32) | static_cast<uint64_t>(low32))

namespace zendo
{

	namespace detail
	{

		inline size_t count_decimal_digit(uint32_t value)
		{
			if( value < 10 ) return 1;
			if( value < 100 ) return 2;
			if( value < 1000 ) return 3;
			if( value < 10000 ) return 4;
			if( value < 100000 ) return 5;
			if( value < 1000000 ) return 6;
			if( value < 10000000 ) return 7;
			if( value < 100000000 ) return 8;
			if( value < 1000000000 ) return 9;
			return 10;
		}

		inline size_t count_decimal_digit(uint64_t value)
		{
			if( value < 10 ) return 1;
			if( value < 100 ) return 2;
			if( value < 1000 ) return 3;
			if( value < 10000 ) return 4;
			if( value < 100000 ) return 5;
			if( value < 1000000 ) return 6;
			if( value < 10000000 ) return 7;
			if( value < 100000000 ) return 8;
			if( value < 1000000000 ) return 9;
			if( value < 10000000000 ) return 10;
			if( value < 100000000000UL ) return 11;
			if( value < 1000000000000UL ) return 12;
			if( value < 10000000000000UL ) return 13;
			if( value < 100000000000000UL ) return 14;
			if( value < 1000000000000000UL ) return 15;
			if( value < 10000000000000000UL ) return 16;
			if( value < 100000000000000000UL ) return 17;
			if( value < 1000000000000000000UL ) return 18;
			if( value < 10000000000000000000UL ) return 19;
			return 20;
		}

		struct floating_point
		{
			static const int kDiySignificandSize = 64;
			static const int kDpSignificandSize = 52;
			static const int kDpExponentBias = 0x3FF + kDpSignificandSize;
			static const int kDpMaxExponent = 0x7FF - kDpExponentBias;
			static const int kDpMinExponent = -kDpExponentBias;
			static const int kDpDenormalExponent = -kDpExponentBias + 1;
			static const uint64_t kDpExponentMask = ZENO_MAKE_UINT64(0x7FF00000, 0x00000000);
			static const uint64_t kDpSignificandMask = ZENO_MAKE_UINT64(0x000FFFFF, 0xFFFFFFFF);
			static const uint64_t kDpHiddenBit = ZENO_MAKE_UINT64(0x00100000, 0x00000000);
			uint64_t significand_;
			int exp_;

			floating_point() = default;
			floating_point(uint64_t fp, int exp) : significand_(fp), exp_(exp)
			{
			}

			explicit floating_point(double d)
			{
				union
				{
					double dle;
					uint64_t u64;
				} u = { d };
				const int biased_e = (u.u64 & kDpExponentMask) >> kDpSignificandSize;
				const uint64_t significand = (u.u64 & kDpSignificandMask);
				if( biased_e != 0 )
				{
					significand_ = significand + kDpHiddenBit;
					exp_ = biased_e - kDpExponentBias;
				}
				else
				{
					significand_ = significand;
					exp_ = kDpMinExponent + 1;
				}
			}

			floating_point operator-(const floating_point& rhs) const
			{
				return floating_point(significand_ - rhs.significand_, exp_);
			}

			floating_point operator*(const floating_point& rhs) const
			{
#if defined(_MSC_VER) && defined(_M_AMD64)

				uint64_t h;
				uint64_t l = _umul128(significand_, rhs.significand_, &h);
				if( l & (uint64_t(1) << 63) )
                {
                    ++h;
                }
				return floating_point(h, exp_ + rhs.exp_ + 64);
#elif (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)) && defined(__x86_64__)
				unsigned __int128 p = static_cast<unsigned __int128>(significand_)* static_cast<unsigned __int128>(rhs.significand_);
				uint64_t h = p >> 64;
				uint64_t l = static_cast<uint64_t>(p);
				if( l & (uint64_t(1) << 63) )
                {
                    ++h;
                }
				return floating_point(h, exp_ + rhs.exp_ + 64);
#else
				const uint64_t M32 = 0xFFFFFFFF;
				const uint64_t a = significand_ >> 32;
				const uint64_t b = significand_ & M32;
				const uint64_t c = rhs.significand_ >> 32;
				const uint64_t d = rhs.significand_ & M32;
				const uint64_t bc = b * c;
				const uint64_t ad = a * d;
				return floating_point((a * c) + (ad >> 32) + (bc >> 32) + ((((b * d) >> 32) + (ad & M32) + (bc & M32) + (1U << 31)) >> 32), exp_ + rhs.exp_ + 64);
#endif
			}

			floating_point normalize() const
			{
#if defined(_MSC_VER) && defined(_M_AMD64)
				unsigned long index;
				_BitScanReverse64(&index, significand_);
				return floating_point(significand_ << (63 - index), exp_ - (63 - index));
#elif defined(__GNUC__) && __GNUC__ >= 4
				int s = __builtin_clzll(significand_);
				return floating_point(significand_ << s, exp_ - s);
#else
				floating_point res = *this;
				const uint64_t sign_bit = static_cast<uint64_t>(1) << 63;
				while( !(res.significand_ & sign_bit) )
				{
					res.significand_ <<= 1;
					--res.exp_;
				}
				return res;
#endif
			}

			floating_point normalize_boundary() const
			{
				floating_point res = *this;
				const uint64_t tr = kDpHiddenBit << 1;
				while( !(res.significand_ & tr) )
				{
					res.significand_ <<= 1;
					--res.exp_;
				}
				const int ts = kDpSignificandSize - 2;
				res.significand_ <<= (kDiySignificandSize - ts);
				res.exp_ = res.exp_ - (kDiySignificandSize - ts);
				return res;
			}

			void normalized_boundaries(floating_point* minus, floating_point* plus) const
			{
				const int under_exp = exp_ - 1;
				floating_point pl = floating_point((significand_ << 1) + 1, under_exp).normalize_boundary();
				floating_point mi = (significand_ == kDpHiddenBit) ? floating_point((significand_ << 2) - 1, under_exp - 1) : floating_point((significand_ << 1) - 1, under_exp);
				mi.significand_ <<= mi.exp_ - pl.exp_;
				mi.exp_ = pl.exp_;
				*plus = pl;
				*minus = mi;
			}

			double to_double() const
			{
				union
				{
					double d;
					uint64_t u64;
				}u;
				uint64_t significand = significand_;
				int exponent = exp_;
				const uint64_t tr = kDpHiddenBit + kDpSignificandMask;
				while( significand > tr )
				{
					significand >>= 1;
					++exponent;
				}
				while( exponent > kDpDenormalExponent && (significand & kDpHiddenBit) == 0 )
				{
					significand <<= 1;
					--exponent;
				}
				if( exponent >= kDpMaxExponent )
				{
					u.u64 = kDpExponentMask;// infinity
					return u.d;
				}
				else if( exponent < kDpDenormalExponent )
                {
                    return 0.0;
                }
				u.u64 = (significand & kDpSignificandMask) |
					(((exponent == kDpDenormalExponent && (significand & kDpHiddenBit) == 0) ? 0 : static_cast<uint64_t>(exponent + kDpExponentBias)) << kDpSignificandSize);
				return u.d;
			}
		};

		inline char* write_exponent(int K, char* buffer)
		{
			const char* digits_lut_table = get_digits_lut();
			if( K < 0 )
			{
				*buffer++ = '-';
				K = -K;
			}
			if( K >= 100 )
			{
				*buffer++ = static_cast<char>('0' + static_cast<char>(K / 100));
				K %= 100;
				const char* d = digits_lut_table + K * 2;
				*buffer++ = d[0];
				*buffer++ = d[1];
			}
			else if( K >= 10 )
			{
				const char* d = digits_lut_table + K * 2;
				*buffer++ = d[0];
				*buffer++ = d[1];
			}
			else
            {
                *buffer++ = static_cast<char>('0' + static_cast<char>(K));
            }
			return buffer;
		}

		inline char* prettify(char* buffer, int length, int k)
		{
			const int kk = length + k;  // 10^(kk-1) <= v < 10^kk
			if( length <= kk && kk <= 21 )
			{
				// 1234e7 -> 12340000000
				for( int i = length; i < kk; ++i )
                {
                    buffer[i] = '0';
                }
				buffer[kk] = '.';
				buffer[kk + 1] = '0';
				return &buffer[kk + 2];
			}
			else if( 0 < kk && kk <= 21 )
			{
				// 1234e-2 -> 12.34
				memmove(&buffer[kk + 1], &buffer[kk], length - kk);
				buffer[kk] = '.';
				return &buffer[length + 1];
			}
			else if( -6 < kk && kk <= 0 )
			{
				// 1234e-6 -> 0.001234
				const int offset = 2 - kk;
				memmove(&buffer[offset], &buffer[0], length);
				buffer[0] = '0';
				buffer[1] = '.';
				for( int i = 2; i < offset; ++i )
                {
                    buffer[i] = '0';
                }
				return &buffer[length + offset];
			}
			else if( length == 1 )
			{
				// 1e30
				buffer[1] = 'e';
				return write_exponent(kk - 1, &buffer[2]);
			}
			else
			{
				// 1234e30 -> 1.234e33
				memmove(&buffer[2], &buffer[1], length - 1);
				buffer[1] = '.';
				buffer[length + 1] = 'e';
				return write_exponent(kk - 1, &buffer[/*0 + */length + 2]);
			}
		}

		inline floating_point get_cached_power_by_index(size_t index)
		{
			// 10^-348, 10^-340, ..., 10^340
			static const uint64_t kCachedPowers_F[] =
			{
				ZENO_MAKE_UINT64(0xfa8fd5a0, 0x081c0288), ZENO_MAKE_UINT64(0xbaaee17f, 0xa23ebf76),
				ZENO_MAKE_UINT64(0x8b16fb20, 0x3055ac76), ZENO_MAKE_UINT64(0xcf42894a, 0x5dce35ea),
				ZENO_MAKE_UINT64(0x9a6bb0aa, 0x55653b2d), ZENO_MAKE_UINT64(0xe61acf03, 0x3d1a45df),
				ZENO_MAKE_UINT64(0xab70fe17, 0xc79ac6ca), ZENO_MAKE_UINT64(0xff77b1fc, 0xbebcdc4f),
				ZENO_MAKE_UINT64(0xbe5691ef, 0x416bd60c), ZENO_MAKE_UINT64(0x8dd01fad, 0x907ffc3c),
				ZENO_MAKE_UINT64(0xd3515c28, 0x31559a83), ZENO_MAKE_UINT64(0x9d71ac8f, 0xada6c9b5),
				ZENO_MAKE_UINT64(0xea9c2277, 0x23ee8bcb), ZENO_MAKE_UINT64(0xaecc4991, 0x4078536d),
				ZENO_MAKE_UINT64(0x823c1279, 0x5db6ce57), ZENO_MAKE_UINT64(0xc2109436, 0x4dfb5637),
				ZENO_MAKE_UINT64(0x9096ea6f, 0x3848984f), ZENO_MAKE_UINT64(0xd77485cb, 0x25823ac7),
				ZENO_MAKE_UINT64(0xa086cfcd, 0x97bf97f4), ZENO_MAKE_UINT64(0xef340a98, 0x172aace5),
				ZENO_MAKE_UINT64(0xb23867fb, 0x2a35b28e), ZENO_MAKE_UINT64(0x84c8d4df, 0xd2c63f3b),
				ZENO_MAKE_UINT64(0xc5dd4427, 0x1ad3cdba), ZENO_MAKE_UINT64(0x936b9fce, 0xbb25c996),
				ZENO_MAKE_UINT64(0xdbac6c24, 0x7d62a584), ZENO_MAKE_UINT64(0xa3ab6658, 0x0d5fdaf6),
				ZENO_MAKE_UINT64(0xf3e2f893, 0xdec3f126), ZENO_MAKE_UINT64(0xb5b5ada8, 0xaaff80b8),
				ZENO_MAKE_UINT64(0x87625f05, 0x6c7c4a8b), ZENO_MAKE_UINT64(0xc9bcff60, 0x34c13053),
				ZENO_MAKE_UINT64(0x964e858c, 0x91ba2655), ZENO_MAKE_UINT64(0xdff97724, 0x70297ebd),
				ZENO_MAKE_UINT64(0xa6dfbd9f, 0xb8e5b88f), ZENO_MAKE_UINT64(0xf8a95fcf, 0x88747d94),
				ZENO_MAKE_UINT64(0xb9447093, 0x8fa89bcf), ZENO_MAKE_UINT64(0x8a08f0f8, 0xbf0f156b),
				ZENO_MAKE_UINT64(0xcdb02555, 0x653131b6), ZENO_MAKE_UINT64(0x993fe2c6, 0xd07b7fac),
				ZENO_MAKE_UINT64(0xe45c10c4, 0x2a2b3b06), ZENO_MAKE_UINT64(0xaa242499, 0x697392d3),
				ZENO_MAKE_UINT64(0xfd87b5f2, 0x8300ca0e), ZENO_MAKE_UINT64(0xbce50864, 0x92111aeb),
				ZENO_MAKE_UINT64(0x8cbccc09, 0x6f5088cc), ZENO_MAKE_UINT64(0xd1b71758, 0xe219652c),
				ZENO_MAKE_UINT64(0x9c400000, 0x00000000), ZENO_MAKE_UINT64(0xe8d4a510, 0x00000000),
				ZENO_MAKE_UINT64(0xad78ebc5, 0xac620000), ZENO_MAKE_UINT64(0x813f3978, 0xf8940984),
				ZENO_MAKE_UINT64(0xc097ce7b, 0xc90715b3), ZENO_MAKE_UINT64(0x8f7e32ce, 0x7bea5c70),
				ZENO_MAKE_UINT64(0xd5d238a4, 0xabe98068), ZENO_MAKE_UINT64(0x9f4f2726, 0x179a2245),
				ZENO_MAKE_UINT64(0xed63a231, 0xd4c4fb27), ZENO_MAKE_UINT64(0xb0de6538, 0x8cc8ada8),
				ZENO_MAKE_UINT64(0x83c7088e, 0x1aab65db), ZENO_MAKE_UINT64(0xc45d1df9, 0x42711d9a),
				ZENO_MAKE_UINT64(0x924d692c, 0xa61be758), ZENO_MAKE_UINT64(0xda01ee64, 0x1a708dea),
				ZENO_MAKE_UINT64(0xa26da399, 0x9aef774a), ZENO_MAKE_UINT64(0xf209787b, 0xb47d6b85),
				ZENO_MAKE_UINT64(0xb454e4a1, 0x79dd1877), ZENO_MAKE_UINT64(0x865b8692, 0x5b9bc5c2),
				ZENO_MAKE_UINT64(0xc83553c5, 0xc8965d3d), ZENO_MAKE_UINT64(0x952ab45c, 0xfa97a0b3),
				ZENO_MAKE_UINT64(0xde469fbd, 0x99a05fe3), ZENO_MAKE_UINT64(0xa59bc234, 0xdb398c25),
				ZENO_MAKE_UINT64(0xf6c69a72, 0xa3989f5c), ZENO_MAKE_UINT64(0xb7dcbf53, 0x54e9bece),
				ZENO_MAKE_UINT64(0x88fcf317, 0xf22241e2), ZENO_MAKE_UINT64(0xcc20ce9b, 0xd35c78a5),
				ZENO_MAKE_UINT64(0x98165af3, 0x7b2153df), ZENO_MAKE_UINT64(0xe2a0b5dc, 0x971f303a),
				ZENO_MAKE_UINT64(0xa8d9d153, 0x5ce3b396), ZENO_MAKE_UINT64(0xfb9b7cd9, 0xa4a7443c),
				ZENO_MAKE_UINT64(0xbb764c4c, 0xa7a44410), ZENO_MAKE_UINT64(0x8bab8eef, 0xb6409c1a),
				ZENO_MAKE_UINT64(0xd01fef10, 0xa657842c), ZENO_MAKE_UINT64(0x9b10a4e5, 0xe9913129),
				ZENO_MAKE_UINT64(0xe7109bfb, 0xa19c0c9d), ZENO_MAKE_UINT64(0xac2820d9, 0x623bf429),
				ZENO_MAKE_UINT64(0x80444b5e, 0x7aa7cf85), ZENO_MAKE_UINT64(0xbf21e440, 0x03acdd2d),
				ZENO_MAKE_UINT64(0x8e679c2f, 0x5e44ff8f), ZENO_MAKE_UINT64(0xd433179d, 0x9c8cb841),
				ZENO_MAKE_UINT64(0x9e19db92, 0xb4e31ba9), ZENO_MAKE_UINT64(0xeb96bf6e, 0xbadf77d9),
				ZENO_MAKE_UINT64(0xaf87023b, 0x9bf0ee6b)
			};
			static const int16_t kCachedPowers_E[] =
			{
				-1220, -1193, -1166, -1140, -1113, -1087, -1060, -1034, -1007, -980,
				-954, -927, -901, -874, -847, -821, -794, -768, -741, -715,
				-688, -661, -635, -608, -582, -555, -529, -502, -475, -449,
				-422, -396, -369, -343, -316, -289, -263, -236, -210, -183,
				-157, -130, -103, -77, -50, -24, 3, 30, 56, 83,
				109, 136, 162, 189, 216, 242, 269, 295, 322, 348,
				375, 402, 428, 455, 481, 508, 534, 561, 588, 614,
				641, 667, 694, 720, 747, 774, 800, 827, 853, 880,
				907, 933, 960, 986, 1013, 1039, 1066
			};
			return floating_point(kCachedPowers_F[index], kCachedPowers_E[index]);
		}

		inline floating_point get_cached_power(int exp, int* K)
		{
			//int k = static_cast<int>(ceil((-61 - exp) * 0.30102999566398114)) + 374;
			double dk = (-61 - exp) * 0.30102999566398114 + 347;
			int k = static_cast<int>(dk);
			if( k != dk )
            {
                ++k;
            }
			unsigned index = static_cast<unsigned>((k >> 3) + 1);
			*K = -(-348 + static_cast<int>(index << 3));
			return get_cached_power_by_index(index);
		}

		inline floating_point get_cached_power10(int exp, int* out_exp)
		{
			unsigned index = (exp + 348) / 8;
			*out_exp = -348 + index * 8;
			return get_cached_power_by_index(index);
		}

		inline void grisu_round(char* buffer, int len, uint64_t delta, uint64_t rest, uint64_t ten_kappa, uint64_t wp_w)
		{
			while( rest < wp_w && delta - rest >= ten_kappa && (rest + ten_kappa < wp_w || wp_w - rest > rest + ten_kappa - wp_w) )
			{
				--(buffer[len - 1]);
				rest += ten_kappa;
			}
		}

		inline void digit_gen(const floating_point& W, const floating_point& Mp, uint64_t delta, char* buffer, int* len, int* K)
		{
			static const uint32_t kPow10[] = { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000 };
			const floating_point one(uint64_t(1) << -Mp.exp_, Mp.exp_);
			const floating_point wp_w = Mp - W;
			uint32_t p1 = static_cast<uint32_t>(Mp.significand_ >> -one.exp_);
			uint64_t p2 = Mp.significand_ & (one.significand_ - 1);
			int kappa = static_cast<int>(count_decimal_digit(p1));
			*len = 0;
			while( kappa > 0 )
			{
				uint32_t d;
				switch( kappa )
				{
					case 10: d = p1 / 1000000000; p1 %= 1000000000; break;
					case  9: d = p1 / 100000000; p1 %= 100000000; break;
					case  8: d = p1 / 10000000; p1 %= 10000000; break;
					case  7: d = p1 / 1000000; p1 %= 1000000; break;
					case  6: d = p1 / 100000; p1 %= 100000; break;
					case  5: d = p1 / 10000; p1 %= 10000; break;
					case  4: d = p1 / 1000; p1 %= 1000; break;
					case  3: d = p1 / 100; p1 %= 100; break;
					case  2: d = p1 / 10; p1 %= 10; break;
					case  1: d = p1; p1 = 0; break;
					default:
#if defined(_MSC_VER)
						__assume(0);
#elif __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5)
						__builtin_unreachable();
#else
						d = 0;
#endif
				}
				if( d || *len )
                {
                    buffer[(*len)++] = static_cast<char>('0' + static_cast<char>(d));
                }
				--kappa;
				const uint64_t tmp = (static_cast<uint64_t>(p1) << -one.exp_) + p2;
				if( tmp <= delta )
				{
					*K += kappa;
					grisu_round(buffer, *len, delta, tmp, static_cast<uint64_t>(kPow10[kappa]) << -one.exp_, wp_w.significand_);
					return;
				}
			}
			// kappa = 0
			for( ;;)
			{
				p2 *= 10;
				delta *= 10;
				char d = static_cast<char>(p2 >> -one.exp_);
				if( d || *len )
                {
                    buffer[(*len)++] = static_cast<char>('0' + d);
                }
				p2 &= one.significand_ - 1;
				--kappa;
				if( p2 < delta )
				{
					*K += kappa;
					grisu_round(buffer, *len, delta, p2, one.significand_, wp_w.significand_ * kPow10[-kappa]);
					return;
				}
			}
		}

		inline void grisu(double value, char* buffer, int* length, int* K)
		{
			const floating_point v(value);
			floating_point w_m, w_p;
			v.normalized_boundaries(&w_m, &w_p);
			const floating_point c_mk = get_cached_power(w_p.exp_, K);
			const floating_point W = v.normalize() * c_mk;
			floating_point Wp = w_p * c_mk;
			floating_point Wm = w_m * c_mk;
			++Wm.significand_;
			--Wp.significand_;
			digit_gen(W, Wp, Wp.significand_ - Wm.significand_, buffer, length, K);
		}

	}

	inline size_t dtoa(double value, char* buffer)
	{
		if( value == 0 )
		{
			buffer[0] = '0';
			buffer[1] = '.';
			buffer[2] = '0';
			return 3;
		}
		else
		{
			if( value < 0 )
			{
				*buffer++ = '-';
				value = -value;
			}
			int length, K;
			detail::grisu(value, buffer, &length, &K);
			detail::prettify(buffer, length, K);
			return length;
		}
	}

}
