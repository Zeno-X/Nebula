// Copyright © 2016 zendo (734181759@qq.com). All rights reserved.

#pragma once
#include <stdint.h>
#ifdef _MSC_VER
#include <intrin.h>
#endif

namespace zendo
{
    
    inline void asm_volatile_memory()
    {
#if defined(__clang__) || defined(__GNUC__)
        asm volatile("" : : : "memory");
#elif defined(_MSC_VER)
        ::_ReadWriteBarrier();
#endif
    }
    
    inline void asm_volatile_pause()
    {
#if defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))
        ::_mm_pause();
#elif defined(__i386__) || defined(__x86_64__)
        asm volatile("pause");
#elif defined(__aarch64__) || defined(__arm__)
        asm volatile("yield");
#elif defined(__powerpc64__)
        asm volatile("or 27,27,27");
#endif
    }
    
    inline void asm_pause()
    {
#if defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))
        ::_mm_pause();
#elif defined(__i386__) || defined(__x86_64__)
        asm("pause");
#elif defined(__aarch64__) || defined(__arm__)
        asm("yield");
#elif defined(__powerpc64__)
        asm("or 31,31,31");
#endif
    }
    
#undef LIKELY
#undef UNLIKELY
#if defined(__GNUC__) && __GNUC__ >= 4
#define LIKELY(x)   (__builtin_expect((x), 1))
#define UNLIKELY(x) (__builtin_expect((x), 0))
#else
#define LIKELY(x)   (x)
#define UNLIKELY(x) (x)
#endif
    
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
    inline int64_t __sync_fetch_and_add(volatile int64_t* ptr, int64_t value)
    {
        return InterlockedExchangeAdd64(ptr, value);
    }
#endif
    
}
