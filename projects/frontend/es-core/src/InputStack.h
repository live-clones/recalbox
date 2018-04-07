#pragma once

#include "InputConfig.h"
#include "Timer.h"
#include <functional>
#include <list>
#include <memory>

/**
 * The purpose of this class is to catch the whole inputs sent by SDL when using a gamepad hat / joy / button
 * Indeed, when pressing one HAT button for example, the GamePad / SDL will send more than one SDL event,
 * Example: when pressing UP HAT button, SDL send: SDL_JOYBUTTONDOWN and SDL_JOYHATMOTION
 * Idea of this class is to stack them waiting for a pause of SDL_IDLE_MS ms before calling a callback
 */
class InputStack
{
public:
	bool hasInput(const Input input);
	void push(const Input input, const std::function<void(const std::list<Input> inputs)>& func);
	void debounce();
private:
	bool mSwallow = false;
	std::list<Input> mInputs;
	std::shared_ptr<Timer> mPushTimer;
	std::shared_ptr<Timer> mDebounceTimer;
};

