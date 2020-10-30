//
// Created by bkg2k on 30/10/2020.
//

#include "OdroidAdvanceGo2.h"

#include <utils/Log.h>
#include <alsa/asoundlib.h>

void OdroidAdvanceGo2::Route(OdroidAdvanceGo2::OutputPath path)
{
  snd_mixer_elem_t* mixerElement = OpenMixer();
  if (mixerElement != nullptr)
  {
    if (snd_mixer_selem_set_enum_item(mixerElement, SND_MIXER_SCHN_MONO, (unsigned int)path) < 0)
      LOG(LogError) << "Failed to set mixer volume for " << Name();
    CloseMixer();
  }
}
