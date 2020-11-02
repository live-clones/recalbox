//
// Created by bkg2k on 30/10/2020.
//
#pragma once

#include <utils/os/system/Thread.h>
#include "OdroidAdvanceGo2PowerEventReader.h"
#include "OdroidAdvanceGo2SpecialButtonsReader.h"

// Forward declaration
class InputCompactEvent;

class OdroidAdvanceGo2Board
{
  public:
    /*!
     * @brief Process special input if any
     * @param inputEvent Input to process
     * @return True if the input has been processed, false otherwise
     */
    static bool ProcessSpecialInputs(InputCompactEvent& inputEvent);

    /*!
     * @brief Set brightness
     * @param step Step value from 0 to 8
     */
    static void SetBrightness(int step);

    /*!
     * @brief Set lowerst brightness available or even switch off the screen
     * @param step Step value from 0 to 8
     */
    static void SetLowestBrightness();

    /*!
     * @brief Start power button event reader
     * This method is called when ES starts
     */
    static void StartGlobalBackgroundProcesses() { mPowerReader.StartReader(); }

    /*!
     * @brief Stop power button event reader
     * This method is called when ES stops
     */
    static void StopGlobalBackgroundProcesses() { mPowerReader.StopReader(); }

    /*!
     * @brief Start reading pad events to manage volume/brightness
     * This method is called when a game starts
     */
    static void StartInGameBackgroundProcesses() { mButtonsReader.StartReader(); }

    /*!
     * @brief Stop reading pad events
     * This method is called when a game stops
     */
    static void StopInGameBackgroundProcesses() { mButtonsReader.StopReader(); }

    /*!
     * @brief Graceful power off
     * Terminate child process, exit demo mode, then power off
     */
    static void GracefulPowerOff();

    /*!
     * @brief Suspend!
     */
    static void Suspend() { mPowerReader.Suspend(); }

  private:
    //! Power button event reader
    static OdroidAdvanceGo2PowerEventReader mPowerReader;
    //! Special button reader (volume/brightness)
    static OdroidAdvanceGo2SpecialButtonsReader mButtonsReader;
};


