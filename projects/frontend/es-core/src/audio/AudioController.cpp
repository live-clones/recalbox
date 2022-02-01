//
// Created by bkg2k on 13/08/2020.
//

#include <utils/IniFile.h>
#include "AudioController.h"

bool AudioController::GetSpecialAudio()
{
  bool result = false;

  // GPI audio configuration must not be changed
  IniFile recalboxBootConf(Path("/boot/recalbox-boot.conf"), false);
  if (recalboxBootConf.AsString("case") == "GPiV1:1") result = true;

  return result;
}

std::string AudioController::SetDefaultPlayback(const std::string& playbackName)
{
  if (!mHasSpecialAudio)
  {
    std::string playback = mController.SetDefaultPlayback(playbackName);
    return playback;
  }

  return playbackName;
}
