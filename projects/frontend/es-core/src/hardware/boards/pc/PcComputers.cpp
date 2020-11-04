//
// Created by bkg2k on 02/11/2020.
//

#include <utils/os/fs/Path.h>
#include <utils/Files.h>
#include <utils/Log.h>
#include "PcComputers.h"

void PcComputers::SetCPUGovernance(IBoardInterface::CPUGovernance cpuGovernance)
{
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

bool PcComputers::HasBattery()
{
  static bool hasBattery = Path(sBatteryCapacityPath).Exists();
  return hasBattery;
}

int PcComputers::BatteryChargePercent()
{
  static Path sBatteryCharge(sBatteryCapacityPath);
  int charge = -1;
  Strings::ToInt(Strings::Trim(Files::LoadFile(sBatteryCharge), "\n"), charge);
  return charge;
}

bool PcComputers::IsBatteryCharging()
{
  static Path sBatteryStatus(sBatteryStatusPath);
  return Strings::Trim(Files::LoadFile(sBatteryStatus), "\n") == "Charging";
}
