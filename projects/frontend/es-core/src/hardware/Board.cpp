#include <hardware/Board.h>
#include <cstring>
#include <utils/Log.h>
#include <utils/Files.h>
#include <input/InputCompactEvent.h>
#include <audio/alsa/OdroidAdvanceGo2.h>
#include "OdroidAdvanceGo2Board.h"

int Board::Run(const std::string& cmd_utf8, bool debug)
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

Board::BoardType Board::GetPiModel(unsigned int revision)
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
    return BoardType::Pi1;

  // New models
  switch ((RaspberryModel)model)
  {
    case RaspberryModel::Zero:
    case RaspberryModel::ZeroW: return BoardType::Pi0;
    case RaspberryModel::OneA:
    case RaspberryModel::OneAPlus:
    case RaspberryModel::OneB:
    case RaspberryModel::OneBPlus:
    case RaspberryModel::OneCM1: return BoardType::Pi1;
    case RaspberryModel::TwoB: return BoardType::Pi2;
    case RaspberryModel::TreeB:
    case RaspberryModel::TreeCM3: return BoardType::Pi3;
    case RaspberryModel::TreeBPlus:
    case RaspberryModel::TreeCM3Plus:
    case RaspberryModel::TreeAPlus: return BoardType::Pi3plus;
    case RaspberryModel::FourB: return BoardType::Pi4;
    case RaspberryModel::Alpha:
    default: break;
  }

  return BoardType::UnknownPi;
}

#define CPU_INFO_FILE   "/proc/cpuinfo"
#define REVISION_STRING "Revision"
#define HARDWARE_STRING "Hardware"

#define SizeLitteral(x) (sizeof(x) - 1)

Board::BoardType Board::GetBoardType()
{
  static BoardType version = BoardType::UndetectedYet;
  std::string hardware;

  if (version == BoardType::UndetectedYet)
  {
    version = BoardType::Unknown;

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
              version = GetPiModel(revision);
            }
            if (hardware == "Hardkernel ODROID-GO2")
            {
              LOG(LogInfo) << "Odroid Advance Go 2 revision " << (colon + 2);
              version = BoardType::OdroidAdvanceGo2;
            }
          }
        }
      }
      fclose(f);
    }
  }

  return version;
}

void Board::SetLowestBrightness()
{
  LOG(LogInfo) << "[Backlight] Set lowest brightness";
  switch(GetBoardType())
  {
    case BoardType::OdroidAdvanceGo2: { OdroidAdvanceGo2Board::SetLowestBrightness(); break; }
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
}

void Board::SetBrightness(int step)
{
  LOG(LogInfo) << "[Backlight] Set brightness to step " << step;
  if (step < 0) step = 0;
  if (step > 8) step = 8;
  switch(GetBoardType())
  {
    case BoardType::OdroidAdvanceGo2: { OdroidAdvanceGo2Board::SetBrightness(step); break; }
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
}

bool Board::HasBattery()
{
  static bool sHasBattery = Path(sBatteryCapacityPath1).Exists() ||
                            Path(sBatteryCapacityPath2).Exists();

  return sHasBattery;
}

int Board::GetBatteryChargePercent()
{
  if (!HasBattery()) return -1;

  static Path sBatteryCharge = Path(Path(sBatteryCapacityPath1).Exists() ? sBatteryCapacityPath1 : sBatteryCapacityPath2);

  int charge = -1;
  Strings::ToInt(Strings::Trim(Files::LoadFile(sBatteryCharge), "\n"), charge);
  return charge;
}

bool Board::IsBatteryCharging()
{
  if (!HasBattery()) return false;

  static Path sBatteryStatus = Path(Path(sBatteryStatusPath1).Exists() ? sBatteryStatusPath1 : sBatteryStatusPath2);

  return Strings::Trim(Files::LoadFile(sBatteryStatus), "\n") == "Charging";
}

void Board::SetCPUGovernance(Board::CPUGovernance cpuGovernance)
{
  if (!HasBattery()) return;

  switch (cpuGovernance)
  {
    case CPUGovernance::PowerSave:
    {
      LOG(LogInfo) << "[CPU] Set powersaving on";
      Files::SaveFile(Path(sCpuGovernancePath), "powersave");
      break;
    }
    case CPUGovernance::FullSpeed:
    {
      LOG(LogInfo) << "[CPU] Set dynamic mode on";
      Files::SaveFile(Path(sCpuGovernancePath), "ondemand");
      break;
    }
    default: break;
  }
}

bool Board::HasExtraVolumeButtons()
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

bool Board::HasExtraBrightnessButtons()
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

bool Board::ProcessSpecialInputs(InputCompactEvent& inputEvent)
{
  switch(GetBoardType())
  {
    case BoardType::OdroidAdvanceGo2: return OdroidAdvanceGo2Board::ProcessSpecialInputs(inputEvent);
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

void Board::StartGlobalBackgroundProcesses()
{
  LOG(LogInfo) << "[Board] Starting background processes.";
  switch(GetBoardType())
  {
    case BoardType::OdroidAdvanceGo2: OdroidAdvanceGo2Board::StartGlobalBackgroundProcesses(); break;
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
}

void Board::StopGlobalBackgroundProcesses()
{
  LOG(LogInfo) << "[Board] Stopping background processes.";
  switch(GetBoardType())
  {
    case BoardType::OdroidAdvanceGo2: OdroidAdvanceGo2Board::StopGlobalBackgroundProcesses(); break;
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
}

void Board::StartInGameBackgroundProcesses()
{
  LOG(LogInfo) << "[Board] Starting in-game background processes.";
  switch(GetBoardType())
  {
    case BoardType::OdroidAdvanceGo2: OdroidAdvanceGo2Board::StartInGameBackgroundProcesses(); break;
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
}

void Board::StopInGameBackgroundProcesses()
{
  LOG(LogInfo) << "[Board] Stopping in-game background processes.";
  switch (GetBoardType())
  {
    case BoardType::OdroidAdvanceGo2: OdroidAdvanceGo2Board::StopInGameBackgroundProcesses(); break;
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
}
