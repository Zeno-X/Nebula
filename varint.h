// Copyright © 2016 zendo (734181759@qq.com). All rights reserved.

#pragma once
#include <exception>
#include "portability.h"

namespace zendo
{
    
    inline uint32_t zig_zag_encode(int32_t val)
    {
        uint32_t uval = static_cast<uint32_t>(val);
        return static_cast<uint32_t>((uval << 1) ^ (val >> 31));
    }
    
    inline int32_t zig_zag_decode(uint32_t val)
    {
        return static_cast<int32_t>((val >> 1) ^ -(val & 0x1));
    }
    
    inline uint64_t zig_zag_encode(int64_t val)
    {
        uint64_t uval = static_cast<uint64_t>(val);
        return static_cast<uint64_t>((uval << 1) ^ (val >> 63));
    }
    
    inline int64_t zig_zag_decode(uint64_t val)
    {
        return static_cast<int64_t>((val >> 1) ^ -(val & 0x1));
    }
    
    inline size_t varint_encode(uint64_t val, void* data)
    {
        uint8_t* p = reinterpret_cast<uint8_t*>(data);
        while (val >= 128)
        {
            *p++ = 0x80 | (val & 0x7f);
            val >>= 7;
        }
        *p++ = uint8_t(val);
        return size_t(p - reinterpret_cast<uint8_t*>(data));
    }
    
    inline uint64_t varint_decode(void* data)
    {
        const int8_t* begin = reinterpret_cast<const int8_t*>(data);
        const int8_t* p = begin;
        int shift = 0;
        uint64_t val = 0;
        while (*p < 0)
        {
            val |= static_cast<uint64_t>(*p++ & 0x7f) << shift;
            shift += 7;
        }
        val |= static_cast<uint64_t>(*p++) << shift;
        return val;
    }
    
}
