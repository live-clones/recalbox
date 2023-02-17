//
// Created by bkg2k on 01/11/2020.
// Modified by davidb2111 for the RG353x series of boards
//
#pragma once

#include <hardware/IBoardInterface.h>
#include <sdl2/Sdl2Runner.h>
#include <input/InputDevice.h>

class RG353XSpecialButtonsReader : private ISdl2EventNotifier
{
  public:
    //! Constructor
    explicit RG353XSpecialButtonsReader(IBoardInterface& boardInterface);

    //! Start reading the power events
    void StartReader(Sdl2Runner& sdlRunner);

    //! Stop reading power events
    void StopReader(Sdl2Runner& sdlRunner);

  private:
    //! Parent board interface
    IBoardInterface& mBoardInterface;

    // Device
    InputDevice* mDevice;

    //! Receive SDL events
    void Sdl2EventReceived(const SDL_Event& event) override;
};

