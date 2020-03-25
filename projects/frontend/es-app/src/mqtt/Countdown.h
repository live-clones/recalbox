//
// Created by Bkg2k on 11/03/2020.
//
#pragma once

/*!
 * @brief Fake timer interface for MqttClient
 */
class Countdown
{
  public:
    /*!
     * @brief Default constructors
     */
    Countdown() = default;

    /*!
     * @brief Initialize this timer with a millisecond countdown
     * @param ms Milliseconds
     */
    explicit Countdown(int ms) { (void)ms; }

    /*!
     * @brief Check if the timer is expired
     * @return True if the timer is expired
     */
    bool expired() const { return false; }

    /*!
     * @brief Reset this timer with a millisecond countdown
     * @param ms Milliseconds
     */
    void countdown_ms(int ms) { (void)ms; }

    /*!
     * @brief Reset this timer with a second countdown
     * @param ms Seconds
     */
    void countdown(int seconds) { (void)seconds; }

    /*!
     * @brief Get time left in millisecond
     * @return Left milliseconds
     */
    int left_ms() { return 100; }
};
