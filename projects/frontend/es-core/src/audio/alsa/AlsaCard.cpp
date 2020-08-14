//
// Created by bkg2k on 13/08/2020.
//

#include "AlsaCard.h"

void AlsaCard::SetVolume(int volume)
{
  for(AlsaMixer& mixer : mMixers)
    mixer.SetVolume(volume);
}
