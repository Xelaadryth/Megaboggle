#include <chrono>
#include "Timer.h"

Timer::Timer()
{
    mStartTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
        ).count();
}

long long Timer::stop()
{
    long long stopTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();

    return stopTime - mStartTime;
}
