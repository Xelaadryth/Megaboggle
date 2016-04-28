#include "Timer.h"
#include <chrono>

Timer::Timer()
{
	mStartTime = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()
		).count();
}

long Timer::stop()
{
	long stopTime = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()).count();

	return stopTime - mStartTime;
}
