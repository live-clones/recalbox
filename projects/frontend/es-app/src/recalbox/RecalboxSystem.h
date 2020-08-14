#pragma once

#include <string>
#include "Window.h"
#include "games/FileData.h"

struct EmulatorDefaults
{
  std::string core;
  std::string emulator;
};

/*!
 * @brief Interface between EmulationStation and the Recalbox System
 */
class RecalboxSystem
{
  public:
    static long long GetMinimumFreeSpaceOnSharePartition() { return 3 << 30; } // 3Gb

    static unsigned long getFreeSpaceGB(const std::string& mountpoint);

    static std::string SizeToString(unsigned long long size);

    static std::string getFreeSpaceInfo();

    static bool isFreeSpaceLimit();

    static std::string getRootPassword();

    static std::vector<std::string> getAvailableWiFiSSID(bool activateWifi);

    static bool setOverscan(bool enable);

    static bool setOverclock(const std::string& mode);

    static bool ping();

    static bool launchKodi(Window&window);

    static bool backupRecalboxConf();

    static bool enableWifi(std::string ssid, std::string key);

    static bool disableWifi();

    static std::string getIpAdress();

    static std::vector<std::string> scanBluetooth();

    static bool pairBluetooth(const std::string& basic_string);

    static std::vector<std::string> getAvailableStorageDevices();

    static std::string getCurrentStorage();

    static bool setStorage(const std::string& basic_string);

    static bool forgetBluetoothControllers();

    static std::pair<std::string, int> execute(const std::string& command);

    static std::pair<std::string, int> getSDLBatteryInfo();

    static std::pair<std::string, int> getSysBatteryInfo();

  private:
    static std::string BuildSettingsCommand(const std::string& arguments);

    static Strings::Vector ExecuteSettingsCommand(const std::string& arguments);
};

