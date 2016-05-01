#include <chrono>
#include "Timer.h"

Timer::Timer()
{
    mStartTime = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()
        ).count();
}

double Timer::stop()
{
    long long stopTime = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();

    return (double)(stopTime - mStartTime) / 1000000.0f;
}
