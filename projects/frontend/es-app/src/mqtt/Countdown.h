//
// Created by Bkg2k on 11/03/2020.
//
#pragma once

#include <utils/datetime/DateTime.h>

/*!
 * @brief Fake timer interface for MqttClient
 */
class Countdown
{
  private:
    DateTime mReference;
    long long mTimer;

  public:
    /*!
     * @brief Default constructors
     */
    Countdown()
      : mTimer(0)
    {
    }

    /*!
     * @brief Initialize this timer with a millisecond countdown
     * @param ms Milliseconds
     */
    explicit Countdown(int ms) { mReference = DateTime(); mTimer = ms; }

    /*!
     * @brief Check if the timer is expired
     * @return True if the timer is expired
     */
    bool expired() const { return (DateTime() - mReference).TotalMilliseconds() > mTimer; }

    /*!
     * @brief Reset this timer with a millisecond countdown
     * @param ms Milliseconds
     */
    void countdown_ms(int ms) { mReference = DateTime(); mTimer = ms; }

    /*!
     * @brief Reset this timer with a second countdown
     * @param ms Seconds
     */
    void countdown(int seconds) { mReference = DateTime(); mTimer = seconds * 1000; }

    /*!
     * @brief Get time left in millisecond
     * @return Left milliseconds
     */
    int left_ms() { return (int)(mTimer - (DateTime() - mReference).TotalMilliseconds()); }
};
