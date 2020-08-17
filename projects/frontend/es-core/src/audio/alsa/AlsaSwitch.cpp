//
// Created by bkg2k on 13/08/2020.
//

#include <utils/math/Misc.h>
#include <utils/Log.h>
#include "AlsaMixer.h"
#include "AlsaSwitch.h"

#include <alsa/asoundlib.h>

void AlsaSwitch::SwitchOn()
{
  snd_mixer_elem_t* mixerElement = OpenMixer();
  if (mixerElement != nullptr)
  {
    snd_mixer_selem_set_playback_switch_all(mixerElement, 1);
    CloseMixer();
  }
}
