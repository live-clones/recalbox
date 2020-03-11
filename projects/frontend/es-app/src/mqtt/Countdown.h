//
// Created by Bkg2k on 11/03/2020.
//
#pragma once

#include <utils/datetime/HighResolutionTimer.h>

/*!
 * @brief Pseudo timer interface for MqttClient
 */
class Countdown
{
  private:
    //! Nanosecond timer
    HighResolutionTimer mTimer;

  public:
    /*!
     * @brief Default constructors
     */
    Countdown() {}

    /*!
     * @brief Initialize this timer with a millisecond countdown
     * @param ms Milliseconds
     */
    Countdown(int ms) { mTimer.Initialize(-ms * 1000000); }

    /*!
     * @brief Check if the timer is expired
     * @return True if the timer is expired
     */
    bool expired() { return mTimer.GetNanoSeconds() >= 0; }

    /*!
     * @brief Reset this timer with a millisecond countdown
     * @param ms Milliseconds
     */
    void countdown_ms(int ms) { mTimer.Initialize(-ms * 1000000); }

    /*!
     * @brief Reset this timer with a second countdown
     * @param ms Seconds
     */
    void countdown(int seconds) { mTimer.Initialize(-seconds * 1000000000); }

    /*!
     * @brief Get time left in millisecond
     * @return Left milliseconds
     */
    int left_ms() { return -mTimer.GetMilliSeconds(); }
};
