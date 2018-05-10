// Copyright © 2016 zendo (734181759@qq.com). All rights reserved.

#pragma once
#include <chrono>

namespace zendo
{
    
    inline uint64_t time_since_epoch_count()
    {
        return std::chrono::high_resolution_clock::now().time_since_epoch().count();
    }
    
    inline uint8_t week_day(int year, int month, int date)
    {
        if(month == 1 || month == 2)
        {
            --year;
            month += 12;
        }
        const int century = year / 100;
        const int y = year - century * 100;
        int week = century / 4 - 2 * century + y + y / 4 + 13 * (month + 1) / 5 + date - 1;
        while(week < 0)
        {
            week += 7;
        }
        week %= 7;
        return week;
    }
    
}
