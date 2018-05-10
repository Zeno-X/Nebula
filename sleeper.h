// Copyright © 2016 zendo (734181759@qq.com). All rights reserved.

#pragma once

#include <cstddef>
#include <cstdint>

#ifdef _MSC_VER
#include <intrin.h>
#endif

namespace zendo
{
    
    class sleeper
    {
    private:
        uint32_t spin_count_ = 0;
    public:
        void wait()
        {
            if(spin_count_ < 4000)
            {
                ++spin_count_;
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
            else
            {
                struct timespec ts = {0, 50000};
                nanosleep(&ts, nullptr);
            }
        }
    };

}

