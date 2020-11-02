#pragma once

#include <string>

// Forward declaration
class InputCompactEvent;

class Board
{
  public:
    //! Board model/generation
    enum class BoardType
    {
        // Undetected
        UndetectedYet, // Not yet detected
        Unknown,       // Unknown hardware
        // RaspberryPi
        Pi0,           // Pi 0, 0W
        Pi1,           // Pi 1, A, B, A+, B+
        Pi2,           // Pi 2B
        Pi3,           // Pi 3B
        Pi3plus,       // Pi 3B+
        Pi4,           // Pi 4B
        UnknownPi,     // Unknown Pi with higher revisions
        // Odroid
        OdroidAdvanceGo2, // Odroid advance go 2
    };

    //! CPU Speed governance
    enum class CPUGovernance
    {
        PowerSave, //! Save has much power as possible
        FullSpeed, //! Real full spead or "on demand"
    };

    /*!
     * @brief Run system command and capture output
     * @param cmd_utf8 Command to execute
     * @param debug log output?
     * @return Return code
     */
    static int Run(const std::string& cmd_utf8, bool debug);

    /*!
     * @brief Get board type
     * @return Board type
     */
    static BoardType GetBoardType();

    /*!
     * @brief Get brightness support
     * @return True if the current board support brightness, false otherwise
     */
    static bool BrightnessSupport()
    {
      switch(GetBoardType())
      {
        case BoardType::OdroidAdvanceGo2: return true;
        case BoardType::UndetectedYet:
        case BoardType::Unknown:
        case BoardType::Pi0:
        case BoardType::Pi1:
        case BoardType::Pi2:
        case BoardType::Pi3:
        case BoardType::Pi3plus:
        case BoardType::Pi4:
        case BoardType::UnknownPi:
        default: break;
      }
      return false;
    }

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
     * @brief Check if the current board has battery
     * @return
     */
    static bool HasBattery();

    /*!
     * @brief Get battery charge in percent
     * @return Battery charge (-1 = no battery)
     */
    static int GetBatteryChargePercent();

    /*!
     * @brief Check if the battery is charging
     * @return True = charging, False = discharging or no battery
     */
    static bool IsBatteryCharging();

    /*!
     * @brief Set CPU governance
     * @param cpuGovernance CPU governance
     */
    static void SetCPUGovernance(CPUGovernance cpuGovernance);

    /*!
     * @brief Check if this board has extra volume +/- buttons
     * @return True if such buttons are available, false otherwise
     */
    static bool HasExtraVolumeButtons();

    /*!
     * @brief Check if this board has extra brightness +/- buttons
     * @return True if such buttons are available, false otherwise
     */
    static bool HasExtraBrightnessButtons();

    /*!
     * @brief Check if the current board supports suspend/resume operations
     * @return True if the board supports suspend/resume operations, false otherwise
     */
    static bool IsSupportingSuspendResume();

    /*!
     * @brief Suspend!
     */
    static void Suspend();

    /*!
     * @brief Process special input if any
     * @param inputEvent Input to process
     * @return True if the input has been processed, false otherwise
     */
    static bool ProcessSpecialInputs(InputCompactEvent& inputEvent);

    /*!
     * @brief Start optional global background processes
     * This method is called when ES starts
     */
    static void StartGlobalBackgroundProcesses();

    /*!
     * @brief Stop optional global background processes
     * This method is called when ES stops
     */
    static void StopGlobalBackgroundProcesses();

    /*!
     * @brief Start optional in-game background processes.
     * This method is called when a game starts
     */
    static void StartInGameBackgroundProcesses();

    /*!
     * @brief Stop optional in-game background processes.
     * This method is called when a game stops
     */
    static void StopInGameBackgroundProcesses();

  private:
    static constexpr const char* sCpuGovernancePath = "/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor";
    static constexpr const char* sBatteryCapacityPath1 = "/sys/class/power_supply/BAT0/capacity";
    static constexpr const char* sBatteryCapacityPath2 = "/sys/class/power_supply/battery/capacity";
    static constexpr const char* sBatteryStatusPath1 = "/sys/class/power_supply/BAT0/status";
    static constexpr const char* sBatteryStatusPath2 = "/sys/class/power_supply/battery/status";

    /*!
     * Raspberry model (real models)
     * https://www.raspberrypi.org/documentation/hardware/raspberrypi/revision-codes/README.md
     */
    enum class RaspberryModel
    {
        OneA        = 0x00,
        OneB        = 0x01,
        OneAPlus    = 0x02,
        OneBPlus    = 0x03,
        TwoB        = 0x04,
        Alpha       = 0x05,
        OneCM1      = 0x06,
        TreeB       = 0x08,
        Zero        = 0x09,
        TreeCM3     = 0x0A,
        ZeroW       = 0x0C,
        TreeBPlus   = 0x0D,
        TreeAPlus   = 0x0E,
        TreeCM3Plus = 0x10,
        FourB       = 0x11,
        FourHundred = 0x13,
        FourCM4     = 0x14,
    };

    /*!
     * @brief Extract RPi modem
     * @param revision raw revision number
     * @return RPi model
     */
    static BoardType GetPiModel(unsigned int revision);
};
