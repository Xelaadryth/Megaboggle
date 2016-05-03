#pragma once

#include <chrono>

class Timer
{
public:
    Timer();
    double stop();
private:
    long long mStartTime;
};
