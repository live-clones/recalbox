//
// Created by Supernature2k on 07/02/2022.
//
#pragma once

#include "Pi400PowerEventReader.h"
#include "hardware/IBoardInterface.h"

// Forward declaration
class InputCompactEvent;

class Pi400Board: public IBoardInterface
{
public:
    explicit Pi400Board(HardwareMessageSender& messageSender)
    : IBoardInterface(messageSender)
    , mPowerReader(messageSender)
    {
    }

private:

    //! Power button event reader
    Pi400PowerEventReader mPowerReader;

    /*!
     * @brief Start optional global background processes
     * This method is called when ES starts
     */
    void StartGlobalBackgroundProcesses() final
    {
        mPowerReader.StartReader();
    }

    /*!
     * @brief Stop optional global background processes
     * This method is called when ES stops
     */
    void StopGlobalBackgroundProcesses() final
    {
        mPowerReader.StopReader();
    }

    /*!
     * @brief Start optional in-game background processes.
     * This method is called when a game starts
     */
    void StartInGameBackgroundProcesses(Sdl2Runner&) final {}

    /*!
     * @brief Stop optional in-game background processes.
     * This method is called when a game stops
     */
    void StopInGameBackgroundProcesses(Sdl2Runner&) final {}

    /*!
     * @brief Has Battery?
     */
    bool HasBattery() final { return false; }

    /*!
     * @brief Has CPU governance? (and is it worth the use)
     */
    bool HasCPUGovernance() final { return false; }

    /*!
     * @brief Has physical volume buttons?
     */
    bool HasMappableVolumeButtons() final { return false; }

    /*!
     * @brief Has physical brightness buttons?
     */
    bool HasMappableBrightnessButtons() final { return false; }

    /*!
     * @brief Has physical brightness buttons?
     */
    bool HasBrightnessSupport() final { return false; }

    /*!
     * @brief Has hardware suspend/resume?
     */
    bool HasSuspendResume() final { return false; }

    /*!
     * @brief Set lowest brightess possible, including black screen
     */
    void SetLowestBrightness() final {}

    /*!
     * @brief Set brightness
     * @param brighness brightness step from 0 to 8 included
     */
    void SetBrightness(int brighness) final { (void)brighness; }

    /*!
     * @brief Set new CPU governance
     * @param governance cpu governance
     */
    void SetCPUGovernance(CPUGovernance governance) final { (void)governance; }

    /*!
     * @brief Process special input if any
     * @param inputEvent Input to process
     * @return True if the input has been processed, false otherwise
     */
    bool ProcessSpecialInputs(InputCompactEvent& inputEvent) final { (void)inputEvent; return false; }

    /*!
     * @brief Suspend!
     */
    void Suspend() final {}

    /*!
     * @brief Get battery charge in percent
     * @return Battery charge (-1 = no battery)
     */
    int BatteryChargePercent() final { return -1; }

    /*!
     * @brief Check if the battery is charging
     * @return True = charging, False = discharging or no battery
     */
    bool IsBatteryCharging() final { return false; }
};
