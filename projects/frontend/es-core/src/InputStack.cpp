#include "InputStack.h"
#include "Timer.h"

#define SDL_IDLE_MS 100

bool InputStack::hasInput(const Input input)
{
	for (auto& mInput : mInputs)
	{
		if(mInput.device == input.device && mInput.type == input.type && mInput.id == input.id)
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

