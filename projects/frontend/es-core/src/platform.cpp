#include "platform.h"
#include <stdio.h>
#include <stdlib.h>
#include <boost/filesystem.hpp>
#include <iostream>
#include <sys/statvfs.h>
#include <sstream>
#include "Settings.h"
#include "Log.h"

#include <fstream>


#ifdef WIN32
#include <codecvt>
#endif

std::string getHomePath()
{
  std::string homePath;

  // this should give you something like "/home/YOUR_USERNAME" on Linux and "C:\Users\YOUR_USERNAME\" on Windows
  const char * envHome = getenv("HOME");
  if(envHome != nullptr)
  {
    homePath = envHome;
  }

#ifdef WIN32
  // but does not seem to work for Windows XP or Vista, so try something else
  if (homePath.empty()) {
    const char * envDir = getenv("HOMEDRIVE");
    const char * envPath = getenv("HOMEPATH");
    if (envDir != nullptr && envPath != nullptr) {
      homePath = envDir;
      homePath += envPath;

      for (unsigned int i = 0; i < homePath.length(); i++)
        if(homePath[i] == '\\')
          homePath[i] = '/';
    }
  }
#endif

  // convert path to generic directory seperators
  boost::filesystem::path genericPath(homePath);
  return genericPath.generic_string();
}

int runShutdownCommand()
{
#ifdef WIN32 // windows
  return system("shutdown -s -t 0");
#else // osx / linux
  return system("poweroff");
#endif
}

int runRestartCommand()
{
#ifdef WIN32 // windows
  return system("shutdown -r -t 0");
#else // osx / linux
  return system("reboot");
#endif
}

int runSystemCommand(const std::string& cmd_utf8)
{
#ifdef WIN32
  // on Windows we use _wsystem to support non-ASCII paths
  // which requires converting from utf8 to a wstring
  typedef std::codecvt_utf8<wchar_t> convert_type;
  std::wstring_convert<convert_type, wchar_t> converter;
  std::wstring wchar_str = converter.from_bytes(cmd_utf8);
  return _wsystem(wchar_str.c_str());
#else
  //return system((cmd_utf8 + " 2> /recalbox/share/system/logs/es_launch_stderr.log | head -300 > /recalbox/share/system/logs/es_launch_stdout.log").c_str());
  int exitcode = system((cmd_utf8 + " > /recalbox/share/system/logs/es_launch_stdout.log 2> /recalbox/share/system/logs/es_launch_stderr.log").c_str());
  return exitcode;
#endif
}

RaspberryGeneration extractGeneration(int revision)
{
  // Split - uuuuuuuuFMMMCCCCPPPPTTTTTTTTRRRR
  bool newGeneration  = (revision >> 23) & 1;
  int  memorySize     = (revision >> 20) & 0x7; (void)memorySize;
  int  manufacturer   = (revision >> 16) & 0xF; (void)manufacturer;
  int  processor      = (revision >> 12) & 0xF; (void)processor;
  int  model          = (revision >>  4) & 0xFF;
  int  revisionNumber = (revision >>  0) & 0xF; (void)revisionNumber;

  // Old revision numbering
  if (!newGeneration)
    return RaspberryGeneration::Pi0or1;

  // New models
  switch ((RaspberryModel)model)
  {
    case RaspberryModel::OneA:
    case RaspberryModel::OneAPlus:
    case RaspberryModel::OneB:
    case RaspberryModel::OneBPlus:
    case RaspberryModel::OneCM1:
    case RaspberryModel::Zero:
    case RaspberryModel::ZeroW: return RaspberryGeneration::Pi0or1;
    case RaspberryModel::TwoB: return RaspberryGeneration::Pi2;
    case RaspberryModel::TreeB:
    case RaspberryModel::TreeCM3: return RaspberryGeneration::Pi3;
    case RaspberryModel::TreeBPlus:
    case RaspberryModel::TreeAPlus: return RaspberryGeneration::Pi3plus;
    case RaspberryModel::Alpha:
    default: break;
  }

  return RaspberryGeneration::NotYetKnown;
}

#define CPU_INFO_FILE   "/proc/cpuinfo"
#define REVISION_STRING "Revision"
#define HARDWARE_STRING "Hardware"

#define SizeLitteral(x) (sizeof(x) - 1)

RaspberryGeneration getRaspberryVersion()
{
  static RaspberryGeneration version = RaspberryGeneration::UndetectedYet;
  std::string hardware;

  if (version == RaspberryGeneration::UndetectedYet)
  {
    version = RaspberryGeneration::NotRaspberry;

    FILE* f = fopen(CPU_INFO_FILE, "r");
    if (f != nullptr)
    {
      char line[1024];
      std::string str; // Declared before loop to keep memory allocated
      while (fgets(line, sizeof(line) - 1, f))
      {
        if (strncmp(line, HARDWARE_STRING, SizeLitteral(HARDWARE_STRING)) == 0)
        {
          char* colon = strchr(line, ':');
          if (colon != nullptr)
          {
            hardware = colon + 2;
            hardware.erase(hardware.find_last_not_of(" \t\r\n") + 1);
            LOG(LogInfo) << "Hardware " << hardware;
          }
        }

        if (strncmp(line, REVISION_STRING, SizeLitteral(REVISION_STRING)) == 0)
        {
          char* colon = strchr(line, ':');
          if (colon != nullptr)
          {
            int revision = (int) strtol(colon + 2, nullptr, 16); // Convert hexa revision

            if (hardware == "BCM2835")
            {
              LOG(LogInfo) << "Pi revision " << (colon + 2);
              version = extractGeneration(revision);
            }
          }
        }
      }
      fclose(f);
    }
  }

  return version;
}
