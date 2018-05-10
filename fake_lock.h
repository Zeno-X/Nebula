// Copyright © 2016 zendo (734181759@qq.com). All rights reserved.

#pragma once
#include <mutex>

namespace zendo
{
    
    struct fake_lock
    {
        using guard_type = std::lock_guard<fake_lock>;
        fake_lock() = default;
        constexpr void lock()noexcept{}
        constexpr bool try_lock()noexcept{return true;}
        constexpr void unlock()noexcept{}
    };
    
}
