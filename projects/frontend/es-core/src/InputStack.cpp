#include "InputStack.h"
#include "Timer.h"

#define SDL_IDLE_MS 100

bool InputStack::hasInput(const Input input)
{
	for(auto it = mInputs.begin(); it != mInputs.end(); it++)
	{
		if(it->device == input.device && it->type == input.type && it->id == input.id)
		{
			return true;
		}
	}
	return false;
}

void InputStack::push(const Input input, const std::function<void(const std::list<Input> inputs)>& func) 
{
	if (mSwallow)
		return;

	if (!mInputs.empty()) {
		mPushTimer->clearTimeout();
	}

	mInputs.push_back(input);
	
	mPushTimer = std::make_shared<Timer>(SDL_IDLE_MS, [this, func] {
		func(mInputs);
		mInputs.clear();
	});
}

void InputStack::debounce()
{
	mSwallow = true;

	if (mPushTimer) 
	{
		mPushTimer->clearTimeout();
		mPushTimer = nullptr;
	}

	mInputs.clear();

	if (mDebounceTimer) 
	{
		mDebounceTimer->clearTimeout();
	}

	mDebounceTimer = std::make_shared<Timer>(SDL_IDLE_MS, [this] {
		mSwallow = false;
	});
}

