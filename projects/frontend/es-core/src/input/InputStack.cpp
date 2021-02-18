#include "InputStack.h"
#include "utils/Log.h"

#define SDL_IDLE_MS 150

bool InputStack::hasInput(const InputEvent& input)
{
	for (auto& mInput : mInputs)
	{
		if(mInput.Device() == input.Device() && mInput.Type() == input.Type() && mInput.Id() == input.Id())
		{
			return true;
		}
	}
	return false;
}

void InputStack::push(const InputEvent& input, const std::function<void(const std::vector<InputEvent>& inputs)>& func)
{
  { LOG(LogDebug) << "[InputStack] Push! " << input.ToString(); }
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

