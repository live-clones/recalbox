#include "platform.h"
#include <cstring>
#include "utils/Log.h"

int runSystemCommand(const std::string& cmd_utf8)
{
  //return system((cmd_utf8 + " 2> /recalbox/share/system/logs/es_launch_stderr.log | head -300 > /recalbox/share/system/logs/es_launch_stdout.log").c_str());
  int exitcode = system((cmd_utf8 + " > /recalbox/share/system/logs/es_launch_stdout.log 2> /recalbox/share/system/logs/es_launch_stderr.log").c_str());
  return exitcode;
}

RaspberryGeneration extractGeneration(unsigned int revision)
{
  // Split - uuuuuuuuFMMMCCCCPPPPTTTTTTTTRRRR
  bool newGeneration  = ((revision >> 23) & 1) != 0;
  int  memorySize     = ((int)revision >> 20) & 0x7; (void)memorySize;
  int  manufacturer   = ((int)revision >> 16) & 0xF; (void)manufacturer;
  int  processor      = ((int)revision >> 12) & 0xF; (void)processor;
  int  model          = ((int)revision >>  4) & 0xFF;
  int  revisionNumber = ((int)revision >>  0) & 0xF; (void)revisionNumber;

  // Old revision numbering
  if (!newGeneration)
    return RaspberryGeneration::Pi1;

  // New models
  switch ((RaspberryModel)model)
  {
    case RaspberryModel::Zero:
    case RaspberryModel::ZeroW: return RaspberryGeneration::Pi0;
    case RaspberryModel::OneA:
    case RaspberryModel::OneAPlus:
    case RaspberryModel::OneB:
    case RaspberryModel::OneBPlus:
    case RaspberryModel::OneCM1: return RaspberryGeneration::Pi1;
    case RaspberryModel::TwoB: return RaspberryGeneration::Pi2;
    case RaspberryModel::TreeB:
    case RaspberryModel::TreeCM3: return RaspberryGeneration::Pi3;
    case RaspberryModel::TreeBPlus:
    case RaspberryModel::TreeCM3Plus:
    case RaspberryModel::TreeAPlus: return RaspberryGeneration::Pi3plus;
    case RaspberryModel::FourB: return RaspberryGeneration::Pi4;
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
      while (fgets(line, sizeof(line) - 1, f) != nullptr)
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
            unsigned int revision = (int) strtol(colon + 2, nullptr, 16); // Convert hexa revision

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
