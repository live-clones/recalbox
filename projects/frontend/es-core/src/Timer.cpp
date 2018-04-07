#include "Timer.h"
#include <future>
#include <boost/thread.hpp>

Timer::Timer(int duration_in_ms, const std::function<void()>& callback)
{
	mDuration = duration_in_ms;
	mCallback = callback;
	mHandle = new boost::thread(boost::bind(&Timer::proceed, this));
}

Timer::~Timer() 
{
    mHandle->join();
}

void Timer::proceed()
{
	auto ms = std::chrono::milliseconds(1);
	for (int i = 0; i < mDuration && !mCancelled; i++) {
		std::this_thread::sleep_for( ms );
	}
	if (!mCancelled)
		mCallback();
}

void Timer::clearTimeout()
{
	mCancelled = true;
}