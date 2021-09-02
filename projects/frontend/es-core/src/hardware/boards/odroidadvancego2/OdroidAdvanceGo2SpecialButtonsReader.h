//
// Created by bkg2k on 01/11/2020.
//
#pragma once

#include <hardware/IBoardInterface.h>
#include <sdl2/Sdl2Runner.h>
#include <input/InputDevice.h>

class OdroidAdvanceGo2SpecialButtonsReader : private ISdl2EventNotifier
{
  public:
    //! Constructor
    explicit OdroidAdvanceGo2SpecialButtonsReader(IBoardInterface& boardInterface);

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

