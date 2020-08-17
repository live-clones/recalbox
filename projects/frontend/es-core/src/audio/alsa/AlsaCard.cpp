//
// Created by bkg2k on 13/08/2020.
//

#include "AlsaCard.h"

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
