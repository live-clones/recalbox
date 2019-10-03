#include "InputStack.h"

#define SDL_IDLE_MS 150

bool InputStack::hasInput(const Input& input)
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

void InputStack::push(const Input& input, const std::function<void(const std::list<Input>& inputs)>& func)
{
  LOG(LogDebug) << "Push!";
  // Debouncing
  TimeSpan diff = DateTime() - mDebounceReference;
  if (diff.TotalMilliseconds() < SDL_IDLE_MS)
    return;

	mInputs.push_back(input);
	// First event pushed, start debouncing
  debounce();

	// Start or restart the timer.
	// Once this timer end, inputs are sent to the method associated with inputstack
	mCallback = func; // TODO: should be set by the constructor
	mTimer.Start(SDL_IDLE_MS, false);
}

