//
// Created by bkg2k on 30/10/2020.
//

#include "OdroidAdvanceGo2Board.h"
#include <input/InputCompactEvent.h>
#include <RecalboxConf.h>
#include <utils/math/Misc.h>
#include <utils/Files.h>
#include <audio/AudioController.h>

bool OdroidAdvanceGo2Board::ProcessSpecialInputs(InputCompactEvent& inputEvent)
{
  // When the wizard is on screen, do not consume special key events
  // so that the wizard can use it
  bool pressResult = !RecalboxConf::Instance().GetFirstTimeUse();

  if (inputEvent.VolumeUpPressed())
  {
    LOG(LogDebug) << "[OdroidAdvanceGo2] Volume + pressed";
    int value = RecalboxConf::Instance().AsInt("audio.volume", 90) + 10;
    value = Math::clampi(value, 0, 100);
    value = (value / 10) * 10;
    AudioController::Instance().SetVolume(value);
    RecalboxConf::Instance().SetInt("audio.volume", value);
    RecalboxConf::Instance().Save();
    return pressResult;
  }
  else if (inputEvent.VolumeDownPressed())
  {
    LOG(LogDebug) << "[OdroidAdvanceGo2] Volume - pressed";
    int value = RecalboxConf::Instance().AsInt("audio.volume", 90) - 10;
    value = Math::clampi(value, 0, 100);
    value = (value / 10) * 10;
    AudioController::Instance().SetVolume(value);
    RecalboxConf::Instance().SetInt("audio.volume", value);
    RecalboxConf::Instance().Save();
    return pressResult;
  }
  else if (inputEvent.BrightnessUpPressed())
  {
    LOG(LogDebug) << "[OdroidAdvanceGo2] Brightness + pressed";
    int value = RecalboxConf::Instance().GetBrightness() + 1;
    value = Math::clampi(value, 0, 8);
    SetBrightness(value);
    RecalboxConf::Instance().SetBrightness(value);
    RecalboxConf::Instance().Save();
    return pressResult;
  }
  else if (inputEvent.BrightnessDownPressed())
  {
    LOG(LogDebug) << "[OdroidAdvanceGo2] Brightness - pressed";
    int value = RecalboxConf::Instance().GetBrightness() - 1;
    value = Math::clampi(value, 0, 8);
    SetBrightness(value);
    RecalboxConf::Instance().SetBrightness(value);
    RecalboxConf::Instance().Save();
    return pressResult;
  }

  return false;
}

void OdroidAdvanceGo2Board::SetLowestBrightness()
{
  Files::SaveFile(Path("/sys/class/backlight/backlight/brightness"), "0");
}

void OdroidAdvanceGo2Board::SetBrightness(int step)
{
  int value = 1 << step; if (value > 255) value = 255;
  Files::SaveFile(Path("/sys/class/backlight/backlight/brightness"), Strings::ToString(value));
}

void OdroidAdvanceGo2Board::SetCPUGovernance(IBoardInterface::CPUGovernance cpuGovernance)
{
  switch (cpuGovernance)
  {
    case CPUGovernance::PowerSave:
    {
      LOG(LogInfo) << "[CPU] Set powersaving mode";
      Files::SaveFile(Path(sCpuGovernancePath), "powersave");
      Files::SaveFile(Path(sGpuGovernancePath), "powersave");
      Files::SaveFile(Path(sDmcGovernancePath), "powersave");
      break;
    }
    case CPUGovernance::OnDemand:
    {
      LOG(LogInfo) << "[CPU] Set on-demand mode";
      Files::SaveFile(Path(sCpuGovernancePath), "ondemand");
      Files::SaveFile(Path(sGpuGovernancePath), "simple_ondemand");
      Files::SaveFile(Path(sDmcGovernancePath), "dmc_ondemand");
      break;
    }
    case CPUGovernance::FullSpeed:
    {
      LOG(LogInfo) << "[CPU] Set performance mode";
      Files::SaveFile(Path(sCpuGovernancePath), "performance");
      Files::SaveFile(Path(sGpuGovernancePath), "performance");
      Files::SaveFile(Path(sDmcGovernancePath), "performance");
      break;
    }
    default: break;
  }
}

int OdroidAdvanceGo2Board::BatteryChargePercent()
{
  static Path sBatteryCharge(sBatteryCapacityPath);
  int charge = -1;
  Strings::ToInt(Strings::Trim(Files::LoadFile(sBatteryCharge), "\n"), charge);
  return charge;
}

bool OdroidAdvanceGo2Board::IsBatteryCharging()
{
  static Path sBatteryStatus(sBatteryStatusPath);
  return Strings::Trim(Files::LoadFile(sBatteryStatus), "\n") == "Charging";
}

