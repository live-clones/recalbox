//
// Created by bkg2k on 30/10/2020.
//
#pragma once

#include "RG353XPowerEventReader.h"
#include "RG353XSpecialButtonsReader.h"
#include "hardware/IBoardInterface.h"
#include "RG353XJackEventReader.h"

// Forward declaration
class InputCompactEvent;

class RG353XBoard: public IBoardInterface
{
  public:
    explicit RG353XBoard(HardwareMessageSender& messageSender, BoardType model)
      : IBoardInterface(messageSender)
      , mHeadphoneReader(messageSender)
      , mPowerReader(messageSender)
      , mButtonsReader(*this)
      , mModel(model)
    {
    }

  private:
    static constexpr const char* sCpuGovernancePath   = "/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor";
    static constexpr const char* sGpuGovernancePath   = "/sys/class/devfreq/ff400000.gpu/governor";
    static constexpr const char* sDmcGovernancePath   = "/sys/class/devfreq/dmc/governor";
    static constexpr const char* sBatteryCapacityPath = "/sys/class/power_supply/battery/capacity";
    static constexpr const char* sBatteryStatusPath   = "/sys/class/power_supply/battery/status";

    //! Headphone jack event reader
    RG353XJackEventReader mHeadphoneReader;
    //! Power button event reader
    RG353XPowerEventReader mPowerReader;
    //! Special button reader (volume/brightness)
    RG353XSpecialButtonsReader mButtonsReader;
    //! Type
    BoardType mModel;

    /*!
     * @brief Start optional global background processes
     * This method is called when ES starts
     */
    void StartGlobalBackgroundProcesses() final
    {
      mPowerReader.StartReader();
      mHeadphoneReader.StartReader();
    }

    /*!
     * @brief Stop optional global background processes
     * This method is called when ES stops
     */
    void StopGlobalBackgroundProcesses() final
    {
      mPowerReader.StopReader();
      mHeadphoneReader.StopReader();
    }

    /*!
     * @brief Start optional in-game background processes.
     * This method is called when a game starts
     */
    void StartInGameBackgroundProcesses(Sdl2Runner& sdlRunner) final
    {
      mButtonsReader.StartReader(sdlRunner);
    }

    /*!
     * @brief Stop optional in-game background processes.
     * This method is called when a game stops
     */
    void StopInGameBackgroundProcesses(Sdl2Runner& sdlRunner) final
    {
      mButtonsReader.StopReader(sdlRunner);
    }

    /*!
     * @brief Has Battery?
     */
    bool HasBattery() final { return true; }

    /*!
     * @brief Has CPU governance? (and is it worth the use)
     */
    bool HasCPUGovernance() final
    { return true; }

    /*!
     * @brief Has physical volume buttons?
     */
    bool HasMappableVolumeButtons() final
    { return false; }

    /*!
     * @brief Has physical brightness buttons?
     */
    bool HasMappableBrightnessButtons() final
    { return true; }

    /*!
     * @brief Has physical brightness buttons?
     */
    bool HasBrightnessSupport() final
    { return true; }

    /*!
     * @brief Has hardware suspend/resume?
     */
    bool HasSuspendResume() final
    { return true; }

    /*!
     * @brief Set lowest brightess possible, including black screen
     */
    void SetLowestBrightness() final;

    /*!
     * @brief Set brightness
     * @param brighness brightness step from 0 to 8 included
     */
    void SetBrightness(int brighness) final;

    /*!
     * @brief Set new CPU governance
     * @param governance cpu governance
     */
    void SetCPUGovernance(CPUGovernance governance) final;

    /*!
     * @brief Process special input if any
     * @param inputEvent Input to process
     * @return True if the input has been processed, false otherwise
     */
    bool ProcessSpecialInputs(InputCompactEvent& inputEvent) final;

    /*!
     * @brief Suspend!
     */
    void Suspend() final { mPowerReader.Suspend(); }

    /*!
     * @brief Get battery charge in percent
     * @return Battery charge (-1 = no battery)
     */
    int BatteryChargePercent() final;

    /*!
     * @brief Check if the battery is charging
     * @return True = charging, False = discharging or no battery
     */
    bool IsBatteryCharging() final;

    /*!
     * @brief The reboot or shutdown is managed by MainRunner, but the board can have some features to manage
     * @return True if a side effect has been triggered
     */
    bool OnRebootOrShutdown() { return false; }
};


