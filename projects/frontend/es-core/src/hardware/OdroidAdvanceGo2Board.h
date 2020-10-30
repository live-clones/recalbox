//
// Created by bkg2k on 30/10/2020.
//
#pragma once

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
};


