// Copyright © 2016 zendo (734181759@qq.com). All rights reserved.

#pragma once

namespace zendo
{
    
    class noncopyable
    {
    protected:
        constexpr noncopyable() = default;
        ~noncopyable() = default;
        noncopyable(const noncopyable&) = delete;
        noncopyable& operator=(const noncopyable&) = delete;
    };
    
}
