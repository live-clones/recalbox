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
  if (inputEvent.VolumeUpPressed())
  {
    LOG(LogDebug) << "[OdroidAdvanceGo2] Volume + pressed";
    int value = RecalboxConf::Instance().AsInt("audio.volume", 90) + 10;
    value = Math::clampi(value, 0, 100);
    value = (value / 10) * 10;
    AudioController::Instance().SetVolume(value);
    RecalboxConf::Instance().SetInt("audio.volume", value);
    RecalboxConf::Instance().Save();
    return true;
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
    return true;
  }
  else if (inputEvent.BrightnessUpPressed())
  {
    LOG(LogDebug) << "[OdroidAdvanceGo2] Brightness + pressed";
    int value = RecalboxConf::Instance().GetBrightness() + 1;
    value = Math::clampi(value, 0, 8);
    SetBrightness(value);
    RecalboxConf::Instance().SetBrightness(value);
    RecalboxConf::Instance().Save();
    return true;
  }
  else if (inputEvent.BrightnessDownPressed())
  {
    LOG(LogDebug) << "[OdroidAdvanceGo2] Brightness - pressed";
    int value = RecalboxConf::Instance().GetBrightness() - 1;
    value = Math::clampi(value, 0, 8);
    SetBrightness(value);
    RecalboxConf::Instance().SetBrightness(value);
    RecalboxConf::Instance().Save();
    return true;
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
