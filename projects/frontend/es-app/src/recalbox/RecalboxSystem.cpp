/*
 * File:   RecalboxSystem.cpp
 * Author: digitallumberjack
 * 
 * Created on 29 novembre 2014, 03:1
 */

#include "RecalboxConf.h"
#include "RecalboxSystem.h"
#include <sys/statvfs.h>
#include "Settings.h"
#include "utils/Log.h"

#include "AudioManager.h"
#include "VolumeControl.h"
#include "systems/SystemData.h"

#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <VideoEngine.h>
#include <utils/StringUtil.h>
#include <utils/FileUtil.h>

void RecalboxSystem::NotifySystemAndGame(const SystemData* system, const FileData* game, bool play, bool demo)
{
  std::string output;
  output.append("System=").append((system != nullptr) ? system->getFullName() : "").append("\r\n");
  output.append("SystemId=").append((system != nullptr) ? system->getName() : "").append("\r\n");
  output.append("Game=").append((game != nullptr) ? game->getName() : "").append("\r\n");
  output.append("GamePath=").append((game != nullptr) ? game->getPath().ToString() : "").append("\r\n");
  output.append("ImagePath=").append((game != nullptr) ? game->Metadata().Image().ToString() : "").append("\r\n");
  output.append("State=").append(play ? (demo ? "demo" : "playing") : "selected").append("\r\n");
  FILE* f = fopen("/tmp/es_state.inf", "w");
  if (f != nullptr)
  {
    fwrite(output.c_str(), output.size(), 1, f);
    fclose(f);
  }
  VideoEngine::This().StopVideo();
}


unsigned long RecalboxSystem::getFreeSpaceGB(const std::string& mountpoint)
{
  struct statvfs fiData {};
  const char* fnPath = mountpoint.c_str();
  unsigned long long free = 0;
  if ((statvfs(fnPath, &fiData)) >= 0)
  {
    free = (((unsigned long long)fiData.f_bfree * (unsigned long long)fiData.f_bsize)) >> 30;
  }
  return (unsigned long)free;
}

std::string RecalboxSystem::SizeToString(unsigned long long size)
{
  if ((size >> 30) != 0) return std::to_string((int)(size >> 30)) + "GB";
  if ((size >> 20) != 0) return std::to_string((int)(size >> 20)) + "MB";
  if ((size >> 10) != 0) return std::to_string((int)(size >> 10)) + "KB";
  return std::to_string((int)size) + 'B';
}

std::string RecalboxSystem::getFreeSpaceInfo()
{
  std::string sharePart = Settings::getInstance()->getString("SharePartition");
  std::string result = "N/A";
  if (!sharePart.empty())
  {
    const char* fnPath = sharePart.c_str();
    struct statvfs fiData {};
    if ((statvfs(fnPath, &fiData)) < 0)
    {
      result += " (SYSTEM ERROR)";
    }
    else
    {
      unsigned long long total = ((unsigned long long)fiData.f_blocks * (unsigned long long)fiData.f_bsize);
      unsigned long long free = ((unsigned long long)fiData.f_bfree * (unsigned long long)fiData.f_bsize);
      if (total != 0)
      {
        unsigned long long used = total - free;
        int percent = (int)(used * 100 / total);
        result = SizeToString(used) + '/' + SizeToString(total) + " (" + std::to_string(percent) + "%)";
      }
    }
  }
  else result += " (NO PARTITION)";

  return result;
}

bool RecalboxSystem::isFreeSpaceLimit()
{
  std::string sharePart = Settings::getInstance()->getString("SharePartition");
  if (!sharePart.empty())
  {
    return getFreeSpaceGB(sharePart) < 2;
  }
  else
  {
    return false; //"ERROR";
  }

}

std::vector<std::string> RecalboxSystem::getAvailableWiFiSSID(bool activatedWifi)
{
  if (!activatedWifi)
  {
    enableWifi("", "");
  }
  std::vector<std::string> res;
  std::ostringstream oss;
  oss << Settings::getInstance()->getString("RecalboxSettingScript") << " " << "wifi list";
  FILE* pipe = popen(oss.str().c_str(), "r");
  char line[1024];

  if (pipe == nullptr)
  {
    return res;
  }

  while (fgets(line, 1024, pipe) != nullptr)
  {
    strtok(line, "\n");
    res.push_back(std::string(line));
  }
  pclose(pipe);
  return res;
}

std::vector<std::string> RecalboxSystem::getAvailableAudioOutputDevices()
{
  std::vector<std::string> res;
  std::ostringstream oss;
  oss << Settings::getInstance()->getString("RecalboxSettingScript") << " " << "lsaudio";
  FILE* pipe = popen(oss.str().c_str(), "r");
  char line[1024];

  if (pipe == nullptr)
  {
    return res;
  }

  while (fgets(line, 1024, pipe) != nullptr)
  {
    strtok(line, "\n");
    res.push_back(std::string(line));
  }
  pclose(pipe);

  return res;
}

std::string RecalboxSystem::getCurrentAudioOutputDevice()
{

  std::ostringstream oss;
  oss << Settings::getInstance()->getString("RecalboxSettingScript") << " " << "getaudio";
  FILE* pipe = popen(oss.str().c_str(), "r");
  char line[1024];

  if (pipe == nullptr)
  {
    return "";
  }

  if (fgets(line, 1024, pipe) != nullptr)
  {
    strtok(line, "\n");
    pclose(pipe);
    return std::string(line);
  }
  return "auto";
}

bool RecalboxSystem::setAudioOutputDevice(const std::string& selected)
{
  std::ostringstream oss;

  AudioManager::getInstance()->deinit();
  VolumeControl::getInstance()->deinit();

  oss << Settings::getInstance()->getString("RecalboxSettingScript") << " " << "audio" << " '" << selected << "'";
  int exitcode = system(oss.str().c_str());
  if (selected.find('[') != std::string::npos)
  {
    int p1 = selected.find(':');
    int p2 = selected.find(']');
    std::string acard = selected.substr(1, p1 - 1);
    std::string adevice = selected.substr(p1 + 1, p2 - p1 - 1);
    std::string alsaAudioDev = "hw:" + acard + "," + adevice;
    setenv("AUDIODEV", alsaAudioDev.c_str(), 1);
  }
  else
    setenv("AUDIODEV", "hw:0,0", 1);
  VolumeControl::getInstance()->init();
  AudioManager::getInstance()->resumeMusic();
  //AudioManager::getInstance()->playCheckSound();

  return exitcode == 0;
}

bool RecalboxSystem::setOverscan(bool enable)
{

  std::ostringstream oss;
  oss << Settings::getInstance()->getString("RecalboxSettingScript") << " " << "overscan";
  if (enable)
  {
    oss << " " << "enable";
  }
  else
  {
    oss << " " << "disable";
  }
  std::string command = oss.str();
  LOG(LogInfo) << "Launching " << command;
  if (system(command.c_str()) != 0)
  {
    LOG(LogWarning) << "Error executing " << command;
    return false;
  }
  else
  {
    LOG(LogInfo) << "Overscan set to : " << enable;
    return true;
  }

}

bool RecalboxSystem::setOverclock(const std::string& mode)
{
  if (!mode.empty())
  {
    std::ostringstream oss;
    oss << Settings::getInstance()->getString("RecalboxSettingScript") << " " << "overclock" << " " << mode;
    std::string command = oss.str();
    LOG(LogInfo) << "Launching " << command;
    if (system(command.c_str()) != 0)
    {
      LOG(LogWarning) << "Error executing " << command;
      return false;
    }
    else
    {
      LOG(LogInfo) << "Overclocking set to " << mode;
      return true;
    }
  }

  return false;
}

bool RecalboxSystem::launchKodi(Window* window)
{

  LOG(LogInfo) << "Attempting to launch kodi...";


  AudioManager::getInstance()->deinit();
  VolumeControl::getInstance()->deinit();

  std::string commandline = InputManager::Instance().GenerateConfiggenConfiguration();
  std::string command = "configgen -system kodi -rom '' " + commandline;

  window->deinit();

  int exitCode = system(command.c_str());
  if (WIFEXITED(exitCode))
  {
    exitCode = WEXITSTATUS(exitCode);
  }

  window->init();
  VolumeControl::getInstance()->init();
  AudioManager::getInstance()->resumeMusic();
  window->normalizeNextUpdate();

  // handle end of kodi
  switch (exitCode)
  {
    case 10: // reboot code
    {
      reboot();
      return true;
    }
    case 11: // shutdown code
    {
      shutdown();
      return true;
    }
    default: break;
  }

  return exitCode == 0;

}

bool RecalboxSystem::backupRecalboxConf()
{
  std::ostringstream oss;
  oss << Settings::getInstance()->getString("RecalboxSettingScript") << " configbackup";
  std::string command = oss.str();

  LOG(LogInfo) << "Launching " << command;
  if (system(command.c_str()) == 0)
  {
    LOG(LogInfo) << "recalbox.conf backup'ed successfully";
    return true;
  }
  else
  {
    LOG(LogInfo) << "recalbox.conf backup failed";
    return false;
  }
}

bool RecalboxSystem::enableWifi(std::string ssid, std::string key)
{
  std::ostringstream oss;
  ssid = StringUtil::replace(ssid, "\"", "\\\"");
  key = StringUtil::replace(key, "\"", "\\\"");
  oss << Settings::getInstance()->getString("RecalboxSettingScript") << " " << "wifi" << " " << "enable" << " \""
      << ssid << "\" \"" << key << "\"";
  std::string command = oss.str();
  LOG(LogInfo) << "Launching " << command;
  if (system(command.c_str()) == 0)
  {
    LOG(LogInfo) << "Wifi enabled ";
    return true;
  }
  else
  {
    LOG(LogInfo) << "Cannot enable wifi ";
    return false;
  }
}

bool RecalboxSystem::disableWifi()
{
  std::ostringstream oss;
  oss << Settings::getInstance()->getString("RecalboxSettingScript") << " " << "wifi" << " " << "disable";
  std::string command = oss.str();
  LOG(LogInfo) << "Launching " << command;
  if (system(command.c_str()) == 0)
  {
    LOG(LogInfo) << "Wifi disabled ";
    return true;
  }
  else
  {
    LOG(LogInfo) << "Cannot disable wifi ";
    return false;
  }
}


bool RecalboxSystem::halt(bool reboot, bool fast)
{
  SDL_Event* quit = new SDL_Event();
  if (fast)
    if (reboot)
      quit->type = SDL_FAST_QUIT | SDL_RB_REBOOT;
    else
      quit->type = SDL_FAST_QUIT | SDL_RB_SHUTDOWN;
  else if (reboot)
    quit->type = SDL_QUIT | SDL_RB_REBOOT;
  else
    quit->type = SDL_QUIT | SDL_RB_SHUTDOWN;
  SDL_PushEvent(quit);
  return false;
}

bool RecalboxSystem::reboot()
{
  return halt(true, false);
}

bool RecalboxSystem::fastReboot()
{
  return halt(true, true);
}

bool RecalboxSystem::shutdown()
{
  return halt(false, false);
}

bool RecalboxSystem::fastShutdown()
{
  return halt(false, true);
}


std::string RecalboxSystem::getIpAdress()
{
  struct ifaddrs* ifAddrStruct = nullptr;
  struct ifaddrs* ifa = nullptr;
  void* tmpAddrPtr = nullptr;

  std::string result = "NOT CONNECTED";
  getifaddrs(&ifAddrStruct);

  for (ifa = ifAddrStruct; ifa != nullptr; ifa = ifa->ifa_next)
  {
    if (ifa->ifa_addr == nullptr)
    {
      continue;
    }
    if (ifa->ifa_addr->sa_family == AF_INET)
    { // check it is IP4
      // is a valid IP4 Address
      tmpAddrPtr = &((struct sockaddr_in*) ifa->ifa_addr)->sin_addr;
      char addressBuffer[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
      printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
      if (std::string(ifa->ifa_name).find("eth") != std::string::npos ||
          std::string(ifa->ifa_name).find("wlan") != std::string::npos)
      {
        result = std::string(addressBuffer);
      }
    }
  }
  // Seeking for ipv6 if no IPV4
  if (result == "NOT CONNECTED")
  {
    for (ifa = ifAddrStruct; ifa != nullptr; ifa = ifa->ifa_next)
    {
      if (ifa->ifa_addr == nullptr)
      {
        continue;
      }
      if (ifa->ifa_addr->sa_family == AF_INET6)
      { // check it is IP6
        // is a valid IP6 Address
        tmpAddrPtr = &((struct sockaddr_in6*) ifa->ifa_addr)->sin6_addr;
        char addressBuffer[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
        printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
        if (std::string(ifa->ifa_name).find("eth") != std::string::npos ||
            std::string(ifa->ifa_name).find("wlan") != std::string::npos)
        {
          return std::string(addressBuffer);
        }
      }
    }
  }
  if (ifAddrStruct != nullptr)
    freeifaddrs(ifAddrStruct);
  return result;
}

std::vector<std::string>* RecalboxSystem::scanBluetooth()
{
  std::vector<std::string>* res = new std::vector<std::string>();
  std::ostringstream oss;
  oss << Settings::getInstance()->getString("RecalboxSettingScript") << " " << "hcitoolscan";
  FILE* pipe = popen(oss.str().c_str(), "r");
  char line[1024];

  if (pipe == nullptr)
  {
    return nullptr;
  }

  while (fgets(line, 1024, pipe) != nullptr)
  {
    strtok(line, "\n");
    res->push_back(std::string(line));
  }
  pclose(pipe);

  return res;
}

bool RecalboxSystem::pairBluetooth(std::string& controller)
{
  std::ostringstream oss;
  oss << Settings::getInstance()->getString("RecalboxSettingScript") << " " << "hiddpair" << " " << controller;
  int exitcode = system(oss.str().c_str());
  return exitcode == 0;
}

std::vector<std::string> RecalboxSystem::getAvailableStorageDevices()
{

  std::vector<std::string> res;
  std::ostringstream oss;
  oss << Settings::getInstance()->getString("RecalboxSettingScript") << " " << "storage list";
  FILE* pipe = popen(oss.str().c_str(), "r");
  char line[1024];

  if (pipe == nullptr)
  {
    return res;
  }

  while (fgets(line, 1024, pipe) != nullptr)
  {
    strtok(line, "\n");
    res.push_back(std::string(line));
  }
  pclose(pipe);

  return res;
}

std::string RecalboxSystem::getCurrentStorage()
{

  std::ostringstream oss;
  oss << Settings::getInstance()->getString("RecalboxSettingScript") << " " << "storage current";
  FILE* pipe = popen(oss.str().c_str(), "r");
  char line[1024];

  if (pipe == nullptr)
  {
    return "";
  }

  if (fgets(line, 1024, pipe) != nullptr)
  {
    strtok(line, "\n");
    pclose(pipe);
    return std::string(line);
  }
  return "INTERNAL";
}

bool RecalboxSystem::setStorage(const std::string& selected)
{
  std::ostringstream oss;
  oss << Settings::getInstance()->getString("RecalboxSettingScript") << " " << "storage" << " " << selected;
  int exitcode = system(oss.str().c_str());
  return exitcode == 0;
}

bool RecalboxSystem::forgetBluetoothControllers()
{
  std::ostringstream oss;
  oss << Settings::getInstance()->getString("RecalboxSettingScript") << " " << "forgetBT";
  int exitcode = system(oss.str().c_str());
  return exitcode == 0;
}

std::string RecalboxSystem::getRootPassword()
{

  std::ostringstream oss;
  oss << Settings::getInstance()->getString("RecalboxSettingScript") << " " << "getRootPassword";
  FILE* pipe = popen(oss.str().c_str(), "r");
  char line[1024];

  if (pipe == nullptr)
  {
    return "";
  }

  if (fgets(line, 1024, pipe) != nullptr)
  {
    strtok(line, "\n");
    pclose(pipe);
    return std::string(line);
  }
  return oss.str();
}

std::pair<std::string, int> RecalboxSystem::execute(const std::string& command)
{
  std::ostringstream oss;
  oss << command;

  FILE* pipe = popen(oss.str().c_str(), "r");
  char line[1024];

  if (pipe == nullptr)
  {
    return std::make_pair("", -1);
  }
  std::ostringstream res;
  while (fgets(line, 1024, pipe) != nullptr)
  {
    res << line;
  }
  int exitCode = pclose(pipe);
  return std::make_pair(res.str(), WEXITSTATUS(exitCode));
}

bool RecalboxSystem::ping()
{
  std::string updateserver = Settings::getInstance()->getString("UpdateServer");
  std::string s("ping -c 1 " + updateserver);
  int exitcode = system(s.c_str());
  return exitcode == 0;
}

std::pair<std::string, int> RecalboxSystem::getSDLBatteryInfo()
{
  std::pair<std::string, int> result;
  int percent;
  auto powerInfo = SDL_GetPowerInfo(nullptr, &percent);
  switch (powerInfo)
  {
    case SDL_POWERSTATE_UNKNOWN:
    {
      percent = -1;
      break;
    }
    case SDL_POWERSTATE_NO_BATTERY:
    {
      percent = -1;
      break;
    }
    case SDL_POWERSTATE_ON_BATTERY:
    {
      if (percent > 66)
        result.first = "\uF1ba";
      else if (percent > 33)
        result.first = "\uF1b8";
      else if (percent > 15)
        result.first = "\uF1b1";
      else
        result.first = "\uF1b5";
      break;
    }
    case SDL_POWERSTATE_CHARGING:
    {
      result.first = "\uf1b4";
      break;
    }
    case SDL_POWERSTATE_CHARGED:
    {
      result.first = "\uf1b4";
      break;
    }
  }
  result.second = percent;

  return result;
}

std::pair<std::string, int> RecalboxSystem::getSysBatteryInfo()
{
  std::pair<std::string, int> result;

  Path batteryCapacity("/sys/class/power_supply/BAT0/capacity");
  Path batteryStatus("/sys/class/power_supply/BAT0/status");
  if (!batteryCapacity.Exists())
    return std::make_pair("", -1);

  int percent = std::stoi(FileUtil::LoadFile(batteryCapacity));
  std::string status = FileUtil::LoadFile(batteryStatus);

  if (status == "Discharging")
  {
    if (percent > 66)
      result.first = "\uF1ba";
    else if (percent > 33)
      result.first = "\uF1b8";
    else if (percent > 15)
      result.first = "\uF1b1";
    else
      result.first = "\uF1b5";
  }
  else
  {
    result.first = "\uf1b4";
  }
  result.second = percent;

  return result;
}

std::string RecalboxSystem::getJSONStringValue(const std::string& json, const std::string& key)
{
  int startPos = json.find("\"" + key + "\": ");

  if (startPos > 0)
  {
    std::string token = json.substr(startPos + key.length() + 4, json.length());
    if (token.substr(0, 4) != "null")
    {
      int endPos = token.find('\"', 1);

      if (endPos > 0)
      {
        return token.substr(1, endPos - 1);
      }
    }

  }
  return "";
}

EmulatorDefaults RecalboxSystem::getEmulatorDefaults(const std::string& emulatorName)
{
  EmulatorDefaults defaults;
  auto initConfig = new RecalboxConf(true);
  std::string json = runCmd("getEmulatorDefaults " + emulatorName);

  defaults.emulator = initConfig->AsString(emulatorName + ".emulator");
  if (defaults.emulator.empty())
  {
    defaults.emulator = getJSONStringValue(json, "emulator");
  }

  defaults.core = initConfig->AsString(emulatorName + ".core");
  if (defaults.core.empty())
  {
    defaults.core = getJSONStringValue(json, "core");
  }

  return defaults;
}

std::string RecalboxSystem::runCmd(const std::string& options)
{
  std::string cmd = Settings::getInstance()->getString("RecalboxSettingScript") + " " + options;
  FILE* pipe = popen(cmd.c_str(), "r");
  char line[1024];

  if (pipe == nullptr)
  {
    return "";
  }

  if (fgets(line, 1024, pipe) != nullptr)
  {
    strtok(line, "\n");
    pclose(pipe);
    return std::string(line);
  }
  return cmd;
}