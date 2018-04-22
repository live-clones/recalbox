#ifndef RECALBOX_SYSTEM
#define    RECALBOX_SYSTEM

#include <string>
#include "Window.h"
#include "components/BusyComponent.h"

struct EmulatorDefaults {
    std::string core;
    std::string emulator;
};

class RecalboxSystem {
public:

    static RecalboxSystem *getInstance();

    const static Uint32 SDL_FAST_QUIT = 0x800F;
    const static Uint32 SDL_RB_SHUTDOWN = 0X4000;
    const static Uint32 SDL_RB_REBOOT = 0x2000;

    unsigned long getFreeSpaceGB(std::string mountpoint);

    std::string getFreeSpaceInfo();

    bool isFreeSpaceLimit();

    std::string getRootPassword();

    bool setAudioOutputDevice(std::string device);
	
	std::vector<std::string> getAvailableWiFiSSID(bool activateWifi);

    std::vector<std::string> getAvailableAudioOutputDevices();

    std::string getCurrentAudioOutputDevice();

    bool setOverscan(bool enable);

    bool setOverclock(std::string mode);

    bool ping();

    bool launchKodi(Window *window);

    bool enableWifi(std::string ssid, std::string key);

    bool disableWifi();

    bool reboot();

    bool shutdown();

    bool fastReboot();

    bool fastShutdown();

    std::string getIpAdress();

    std::vector<std::string> *scanBluetooth();

    bool pairBluetooth(std::string &basic_string);

    std::vector<std::string> getAvailableStorageDevices();

    std::string getCurrentStorage();

    bool setStorage(std::string basic_string);

    bool forgetBluetoothControllers();

    std::string readFile(std::string file);

    std::pair<std::string, int> execute(std::string command);

	std::pair<std::string, int> getSDLBatteryInfo();

	std::pair<std::string, int> getSysBatteryInfo();

    EmulatorDefaults getEmulatorDefaults(std::string emulatorName);

private:
    static RecalboxSystem *instance;

    RecalboxSystem();

    bool halt(bool reboot, bool fast);

    std::string runCmd(std::string cmd);
    std::string getJSONStringValue(std::string json, std::string key);

};

#endif

