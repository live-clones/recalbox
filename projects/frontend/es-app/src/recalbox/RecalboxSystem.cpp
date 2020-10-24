/*
 * File:   RecalboxSystem.cpp
 * Author: digitallumberjack
 * 
 * Created on 29 novembre 2014, 03:1
 */

#include "RecalboxSystem.h"
#include <sys/statvfs.h>
#include "Settings.h"
#include "utils/Log.h"
#include "audio/AudioManager.h"

#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <utils/Strings.h>
#include <utils/Files.h>
#include <MainRunner.h>
#include <Upgrade.h>

std::string RecalboxSystem::BuildSettingsCommand(const std::string& arguments)
{
  std::string result = Settings::Instance().RecalboxSettingScript();
  result.append(1, ' ');
  result.append(arguments);
  return result;
}

Strings::Vector RecalboxSystem::ExecuteSettingsCommand(const std::string& arguments)
{
  std::string output;
  char buffer[1024];
  FILE* pipe = popen(BuildSettingsCommand(arguments).c_str(), "r");
  if (pipe != nullptr)
  {
    while (feof(pipe) == 0)
      if (fgets(buffer, sizeof(buffer), pipe) != nullptr)
        output.append(buffer);
    pclose(pipe);
  }
  return Strings::Split(output, '\n');
}

unsigned long long RecalboxSystem::getFreeSpace(const std::string& mountpoint)
{
  struct statvfs fiData {};
  const char* fnPath = mountpoint.c_str();
  unsigned long long free = 0;
  if ((statvfs(fnPath, &fiData)) >= 0)
  {
    free = (((unsigned long long)fiData.f_bfree * (unsigned long long)fiData.f_bsize));
  }
  return free;
}

unsigned long RecalboxSystem::getFreeSpaceGB(const std::string& mountpoint)
{
  return (unsigned long)(getFreeSpace(mountpoint) >> 30);
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
  std::string sharePart = Settings::Instance().SharePartition();
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
  std::string sharePart = Settings::Instance().SharePartition();
  if (!sharePart.empty())
  {
    return getFreeSpace(sharePart) < GetMinimumFreeSpaceOnSharePartition();
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
  FILE* pipe = popen(BuildSettingsCommand("wifi list").c_str(), "r");
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

bool RecalboxSystem::setOverscan(bool enable)
{
  std::string cmd =Settings::Instance().RecalboxSettingScript() + " overscan";
  cmd += enable ? " enable" : " disable";
  LOG(LogInfo) << "Launching " << cmd;
  if (system(cmd.c_str()) != 0)
  {
    LOG(LogWarning) << "Error executing " << cmd;
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
    std::string cmd = Settings::Instance().RecalboxSettingScript() + " overclock" + ' ' + mode;
    LOG(LogInfo) << "Launching " << cmd;
    if (system(cmd.c_str()) != 0)
    {
      LOG(LogWarning) << "Error executing " << cmd;
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

bool RecalboxSystem::launchKodi(Window& window)
{
  LOG(LogInfo) << "Attempting to launch kodi...";

  AudioManager::Instance().Deactivate();

  OrderedDevices controllers = InputManager::Instance().GenerateConfiguration();
  std::string commandline = InputManager::GenerateConfiggenConfiguration(controllers);
  std::string command = "configgen -system kodi -rom '' " + commandline;

  Window::Finalize();

  NotificationManager::Instance().NotifyKodi();

  int exitCode = system(command.c_str());
  if (WIFEXITED(exitCode))
  {
    exitCode = WEXITSTATUS(exitCode);
  }

  window.Initialize();
  AudioManager::Instance().Reactivate();
  window.normalizeNextUpdate();

  // handle end of kodi
  switch (exitCode)
  {
    case 10: // reboot code
    {
      MainRunner::RequestQuit(MainRunner::ExitState::NormalReboot);
      return true;
    }
    case 11: // shutdown code
    {
      MainRunner::RequestQuit(MainRunner::ExitState::Shutdown);
      return true;
    }
    default: break;
  }

  return exitCode == 0;
}

bool RecalboxSystem::backupRecalboxConf()
{
  std::string cmd = Settings::Instance().RecalboxSettingScript() + " configbackup";
  LOG(LogInfo) << "Launching " << cmd;
  if (system(cmd.c_str()) == 0)
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
  ssid = Strings::Replace(ssid, "\"", "\\\"");
  key = Strings::Replace(key, "\"", "\\\"");
  std::string cmd = Settings::Instance().RecalboxSettingScript() + " wifi enable \"" + ssid + "\" \"" + key + "\"";
  LOG(LogInfo) << "Launching " << cmd;
  if (system(cmd.c_str()) == 0)
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
  std::string cmd = Settings::Instance().RecalboxSettingScript() + " wifi disable";
  LOG(LogInfo) << "Launching " << cmd;
  if (system(cmd.c_str()) == 0)
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

std::string RecalboxSystem::getIpAdress()
{
  struct ifaddrs* ifAddrStruct = nullptr;

  std::string result = "NOT CONNECTED";
  getifaddrs(&ifAddrStruct);

  for (struct ifaddrs* ifa = ifAddrStruct; ifa != nullptr; ifa = ifa->ifa_next)
  {
    if (ifa->ifa_addr == nullptr)
    {
      continue;
    }
    if (ifa->ifa_addr->sa_family == AF_INET)
    { // check it is IP4
      // is a valid IP4 Address
      void* tmpAddrPtr = &((struct sockaddr_in*) ifa->ifa_addr)->sin_addr;
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
    for (struct ifaddrs* ifa = ifAddrStruct; ifa != nullptr; ifa = ifa->ifa_next)
    {
      if (ifa->ifa_addr == nullptr)
      {
        continue;
      }
      if (ifa->ifa_addr->sa_family == AF_INET6)
      { // check it is IP6
        // is a valid IP6 Address
        void* tmpAddrPtr = &((struct sockaddr_in6*) ifa->ifa_addr)->sin6_addr;
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

std::vector<std::string> RecalboxSystem::scanBluetooth()
{
  return ExecuteSettingsCommand("hcitoolscan");
}

bool RecalboxSystem::pairBluetooth(const std::string& controller)
{
  std::string escapedController(controller);
  Strings::ReplaceAllIn(escapedController, "(", "\\(");
  Strings::ReplaceAllIn(escapedController, ")", "\\)");
  Strings::ReplaceAllIn(escapedController, "*", "\\*");
  Strings::ReplaceAllIn(escapedController, "'", "\\'");
  Strings::ReplaceAllIn(escapedController, "\"", "\\\"");
  std::string cmd = Settings::Instance().RecalboxSettingScript() + " hiddpair " + escapedController;
  int exitcode = system(cmd.c_str());
  return exitcode == 0;
}

std::vector<std::string> RecalboxSystem::getAvailableStorageDevices()
{
  return ExecuteSettingsCommand("storage list");
}

std::string RecalboxSystem::getCurrentStorage()
{
  Strings::Vector lines = ExecuteSettingsCommand("storage current");
  return lines.empty() ? "INTERNAL" : lines[0];
}

bool RecalboxSystem::setStorage(const std::string& selected)
{
  std::string cmd = Settings::Instance().RecalboxSettingScript() + " storage " + selected;
  int exitcode = system(cmd.c_str());
  return exitcode == 0;
}

bool RecalboxSystem::forgetBluetoothControllers()
{
  std::string cmd = Settings::Instance().RecalboxSettingScript() + " forgetBT";
  int exitcode = system(cmd.c_str());
  return exitcode == 0;
}

std::string RecalboxSystem::getRootPassword()
{
  Strings::Vector lines = ExecuteSettingsCommand("getRootPassword");
  return lines.empty() ? "" : lines[0];
}

std::pair<std::string, int> RecalboxSystem::execute(const std::string& command)
{
  FILE* pipe = popen(command.c_str(), "r");
  char line[1024];

  if (pipe == nullptr)
  {
    return std::make_pair("", -1);
  }
  std::string output;
  while (fgets(line, 1024, pipe) != nullptr)
  {
    output += line;
  }
  int exitCode = pclose(pipe);
  return std::make_pair(output, WEXITSTATUS(exitCode));
}

bool RecalboxSystem::ping()
{
  return Upgrade::NetworkReady();

  /*const std::string& updateserver = Settings::Instance().UpdateServer();
  std::string s("ping -c 1 " + updateserver);
  int exitcode = system(s.c_str());
  return exitcode == 0;*/
}

std::pair<std::string, int> RecalboxSystem::getSDLBatteryInfo()
{
  std::pair<std::string, int> result;
  int percent = -1;
  auto powerInfo = SDL_GetPowerInfo(nullptr, &percent);
  switch (powerInfo)
  {
    case SDL_POWERSTATE_UNKNOWN:
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

  int percent = 0;
  Strings::ToInt(Files::LoadFile(batteryCapacity), percent);
  std::string status = Files::LoadFile(batteryStatus);

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
