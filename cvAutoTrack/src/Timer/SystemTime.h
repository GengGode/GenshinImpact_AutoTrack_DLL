#pragma once
#include "TimerFacility.h"
#include <string>
#include <time.h>

class SystemTimer
{
	time_t t;
    tm tm;
public:
    SystemTimer() {}
    virtual std::string now()
    {
        char str[255];
        t = time(NULL);
        localtime_s(&tm, &t);
        strftime(str, 255, "%Y-%m-%d:%H:%M:%S",&tm);
        return std::string(str);
    }
};
