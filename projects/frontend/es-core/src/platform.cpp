#include "platform.h"
#include <stdio.h>
#include <stdlib.h>
#include <boost/filesystem.hpp>
#include <iostream>
#include <sys/statvfs.h>
#include <sstream>
#include "Settings.h"

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

      for(unsigned int i = 0; i < homePath.length(); i++)
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
  return system((cmd_utf8 + " 2> /recalbox/share/system/logs/es_launch_stderr.log | head -300 > /recalbox/share/system/logs/es_launch_stdout.log").c_str());
#endif
}

#define CPU_INFO_FILE   "/proc/cpuinfo"
#define REVISION_STRING "Revision"

#define SizeLitteral(x) (sizeof(x) - 1)

RaspberryPiVersion getRaspberryVersion()
{
  RaspberryPiVersion version = RaspberryPiVersion::NotRaspberry;

  FILE *f = fopen(CPU_INFO_FILE, "r");
  if (f != nullptr)
  {
    char line[1024];
    std::string str; // Declared before loop to keep memory allocated
    while (fgets(line, sizeof(line) - 1, f))
    {
      if (strncmp(line, REVISION_STRING, SizeLitteral(REVISION_STRING)) == 0)
      {
        char* colon = strchr(line, ':');
        if (colon != nullptr)
        {
          int revision = (int)strtol(colon + 2, NULL, 16); // Convert hexa revision
          revision &= 0x00FFFFFF; // Keep only the least significant 24 bits
          int highRevision = revision >> 16;
          int lowRevision = revision & 0xFFFF;

          // The following test are based on the revision table available here:
          // https://elinux.org/RPi_HardwareHistory
          if (highRevision == 0) // Pi 1
          {
            version = RaspberryPiVersion::Pi1;
          }
          else if (highRevision == 0x90) // Pi 1 / Pi 0
          {
            version = lowRevision <= 0x32 ? RaspberryPiVersion::Pi1 : RaspberryPiVersion::Pi0;
          }
          else if (highRevision >= 0xa0 && highRevision <= 0xa3) // Pi 2 / Pi 3 (+)
          {
            if (lowRevision == 0x20d3) version = RaspberryPiVersion::Pi3plus;
            else version = lowRevision <= 0x2042 ? RaspberryPiVersion::Pi2 : RaspberryPiVersion::Pi3;
          }
          else if (highRevision > 0xa3)
          {
            version = RaspberryPiVersion::NotYetKnown;
          }
          break;
        }
      }
    }
    fclose(f);
  }

  return version;
}
