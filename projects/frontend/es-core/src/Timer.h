#pragma once

#include <functional>
#include <atomic>
#include <boost/thread.hpp>

/**
 * The purpose of this class is to provide a kind of javascript setTimeout.
 * It allows to wait for a duration before running a callback.
 * During the interval, it is possinle to cancel the call of the callback
 */
class Timer
{
public:
	Timer(int duration_in_ms, const std::function<void()>& callback);
	~Timer();
	void clearTimeout();
private:
    boost::thread *mHandle;
	void proceed();
	std::atomic<int> mDuration {0};
	std::atomic<bool> mCancelled {false};
	std::function<void()> mCallback;
};
