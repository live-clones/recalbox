#include <platform.h>
#include <cstring>
#include <utils/Log.h>
#include <utils/Files.h>

int runSystemCommand(const std::string& cmd_utf8, bool debug)
{
  static std::string output("/recalbox/share/system/logs/es_launch_stdout.log");
  static std::string outerr("/recalbox/share/system/logs/es_launch_stderr.log");

  // Run command
  std::string cmd(cmd_utf8);
  cmd.append(" > ").append(output)
     .append(" 2> ").append(outerr);
  int exitcode = system(cmd.data());

  // Get logs
  if (debug)
  {
    Path outPath(output);
    Path errPath(outerr);

    static constexpr int sLogSizeLimit = 2 << 20; // 2Mb

    // stdout
    if (outPath.Size() > sLogSizeLimit)
    {
      long long size = outPath.Size();
      std::string start = Files::LoadFile(outPath, 0, sLogSizeLimit / 2);
      std::string stop = Files::LoadFile(outPath, size - (sLogSizeLimit / 2), sLogSizeLimit / 2);
      LOG(LogInfo) << "Configgen Output:\n" << start << "\n...\n" << stop;
    }
    else
    {
      std::string content = Files::LoadFile(outPath);
      if (!content.empty()) LOG(LogInfo) << "Configgen Output:\n" << content;
    }

    // stderr
    if (errPath.Size() > sLogSizeLimit)
    {
      long long size = errPath.Size();
      std::string start = Files::LoadFile(errPath, 0, sLogSizeLimit / 2);
      std::string stop = Files::LoadFile(errPath, size - (sLogSizeLimit / 2), sLogSizeLimit / 2);
      LOG(LogInfo) << "Configgen Errors:\n" << start << "\n...\n" << stop;
    }
    else
    {
      std::string content = Files::LoadFile(errPath);
      if (!content.empty()) LOG(LogInfo) << "Configgen Errors:\n" << content;
    }
  }

  // Return state
  return exitcode;
}

BoardGeneration extractGeneration(unsigned int revision)
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
    return BoardGeneration::Pi1;

  // New models
  switch ((RaspberryModel)model)
  {
    case RaspberryModel::Zero:
    case RaspberryModel::ZeroW: return BoardGeneration::Pi0;
    case RaspberryModel::OneA:
    case RaspberryModel::OneAPlus:
    case RaspberryModel::OneB:
    case RaspberryModel::OneBPlus:
    case RaspberryModel::OneCM1: return BoardGeneration::Pi1;
    case RaspberryModel::TwoB: return BoardGeneration::Pi2;
    case RaspberryModel::TreeB:
    case RaspberryModel::TreeCM3: return BoardGeneration::Pi3;
    case RaspberryModel::TreeBPlus:
    case RaspberryModel::TreeCM3Plus:
    case RaspberryModel::TreeAPlus: return BoardGeneration::Pi3plus;
    case RaspberryModel::FourB: return BoardGeneration::Pi4;
    case RaspberryModel::Alpha:
    default: break;
  }

  return BoardGeneration::UnknownPi;
}

#define CPU_INFO_FILE   "/proc/cpuinfo"
#define REVISION_STRING "Revision"
#define HARDWARE_STRING "Hardware"

#define SizeLitteral(x) (sizeof(x) - 1)

BoardGeneration getHardwareBoardVersion()
{
  static BoardGeneration version = BoardGeneration::UndetectedYet;
  std::string hardware;

  if (version == BoardGeneration::UndetectedYet)
  {
    version = BoardGeneration::Unknown;

    FILE* f = fopen(CPU_INFO_FILE, "r");
    if (f != nullptr)
    {
      char line[1024];
      std::string str; // Declared before loop to keep memory allocated
      while (fgets(line, sizeof(line) - 1, f) != nullptr)
      {
        // Raspberry pi
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
            if (hardware == "Hardkernel ODROID-GO2")
            {
              LOG(LogInfo) << "Odroid Advance Go 2 revision " << (colon + 2);
              version = BoardGeneration::OdroidAdvanceGo2;
            }
          }
        }
      }
      fclose(f);
    }
  }

  return version;
}
