#pragma once

#include <input/InputDevice.h>
#include <utils/sdl2/SyncronousEventService.h>
#include <utils/sdl2/ISyncTimer.h>
#include <utils/sdl2/SyncTimer.h>
#include <functional>
#include <vector>
#include <utils/datetime/DateTime.h>

/*!
 * The purpose of this class is to catch the whole inputs sent by SDL when using a gamepad hat / joy / button
 * Indeed, when pressing one HAT button for example, the GamePad / SDL will send more than one SDL event,
 * Example: when pressing UP HAT button, SDL send: SDL_JOYBUTTONDOWN and SDL_JOYHATMOTION
 * Idea of this class is to stack them waiting for a pause of SDL_IDLE_MS ms before calling a callback
 */
class InputStack : private ISyncTimer
{
  private:
    //! Time reference for debouncing (reject bouncing events)
    DateTime mDebounceReference;

    //! Timer
    SyncTimer mTimer;

    //! Event list
    std::vector<InputEvent> mInputs;

    //! Callbacl method
    std::function<void(const std::vector<InputEvent>& inputs)> mCallback;

    void TimerTick(int id) override
    {
      (void)id;
      mCallback(mInputs);
      mInputs.clear();
    }

  public:
    bool hasInput(const InputEvent& input);

    void push(const InputEvent& input, const std::function<void(const std::vector<InputEvent>& inputs)>& func);

    void debounce()
    {
      mDebounceReference = DateTime();
    }

    InputStack()
      : mTimer(this, 0, "InputStack"),
        mCallback(nullptr)
    {
      mDebounceReference = DateTime(0LL);
    }

};

