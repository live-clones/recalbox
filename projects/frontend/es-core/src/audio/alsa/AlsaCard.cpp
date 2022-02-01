//
// Created by bkg2k on 13/08/2020.
//

#include "AlsaCard.h"
#include <utils/Log.h>

void AlsaCard::SetVolume(int volume)
{
  for(AlsaVolume& mixer : mVolumes)
    mixer.SetVolume(volume);
}

void AlsaCard::SwitchOn()
{
  for(AlsaSwitch& mixer : mSwitches)
    mixer.SwitchOn();
}

OdroidAdvanceGo2Alsa* AlsaCard::OdroidAdvanceGo2Router() const
{
  for(const AlsaMixer& mixer : mRawMixers)
    if (mixer.Type() == AlsaMixer::MixerType::OdroidAdvanceGo2Path)
      return ((OdroidAdvanceGo2Alsa*)&mixer);

  { LOG(LogError) << "[Alsa] No OdroidAdvanceGo2 found!"; }
  return nullptr;
}
