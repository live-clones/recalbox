//
// Created by bkg2k on 13/08/2020.
//

#include <utils/math/Misc.h>
#include <utils/Log.h>
#include "AlsaVolume.h"
#include <alsa/asoundlib.h>

void AlsaVolume::SetVolume(int volume)
{
  snd_mixer_elem_t* mixerElement = OpenMixer();
  if (mixerElement != nullptr)
  {
    volume = Math::clampi(volume, 0, 100);
    long minVolume = 0, maxVolume = 0;
    if (snd_mixer_selem_get_playback_volume_range(mixerElement, &minVolume, &maxVolume) == 0)
    {
      long rawVolume = (volume * (maxVolume - minVolume) / 100) + minVolume;
      if (snd_mixer_selem_set_playback_volume_all(mixerElement, rawVolume) < 0)
        LOG(LogError) << "Failed to set mixer volume for " << Name();
    }
    else LOG(LogError) << "Failed to get volume range for " << Name();
    CloseMixer();
  }
}

int AlsaVolume::GetVolume()
{
  int volume = 0;

  snd_mixer_elem_t* mixerElement = OpenMixer();
  if (mixerElement != nullptr)
  {
    long minVolume = 0, maxVolume = 0;
    if (snd_mixer_selem_get_playback_volume_range(mixerElement, &minVolume, &maxVolume) == 0)
    {
      long rawVolume = 0;
      if (snd_mixer_selem_get_playback_volume(mixerElement, SND_MIXER_SCHN_MONO, &rawVolume) == 0)
      {
        rawVolume -= minVolume;
        volume = Math::clampi(Math::ceili(((float)rawVolume * 100.0f) / (float)(maxVolume - minVolume)), 0, 100);
      }
      else LOG(LogError) << "Failed to get mixer volume for " << Name();
    }
    else LOG(LogError) << "Failed to get volume range for " << Name();
    CloseMixer();
  }

  return volume;
}

