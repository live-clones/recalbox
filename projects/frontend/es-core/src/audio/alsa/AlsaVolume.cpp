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
  if (mixerElement == nullptr) return;

  volume = Math::clampi(volume, 0, 100);
  long minVolume = 0, maxVolume = 0;
  if (snd_mixer_selem_get_playback_volume_range(mixerElement, &minVolume, &maxVolume) == 0)
  {
    long rawVolume = PercentToVolume(volume, minVolume, maxVolume);
    if (snd_mixer_selem_set_playback_volume_all(mixerElement, rawVolume) < 0)
    { LOG(LogError) << "[AlsaVolume] Failed to set mixer volume for " << Name(); }
  }
  else { LOG(LogError) << "[AlsaVolume] Failed to get volume range for " << Name(); }
  CloseMixer();
}

int AlsaVolume::GetVolume()
{
  int volume = 0;

  snd_mixer_elem_t* mixerElement = OpenMixer();
  if (mixerElement == nullptr) return 0;

  long minVolume = 0, maxVolume = 0;
  long rawVolume = 0;
  if (snd_mixer_selem_get_playback_volume_range(mixerElement, &minVolume, &maxVolume) == 0)
  {
    if (snd_mixer_selem_get_playback_volume(mixerElement, SND_MIXER_SCHN_MONO, &rawVolume) == 0)
      volume = (int)VolumeToPercent((double)rawVolume, minVolume, maxVolume);
    else { LOG(LogError) << "[AlsaVolume] Failed to get mixer volume for " << Name(); }
  }
  else { LOG(LogError) << "[AlsaVolume] Failed to get volume range for " << Name(); }
  CloseMixer();

  return volume;
}

#define LOGARITHM(x) log2(x)
#define EXPONENTIAL(x) exp2(x)

long AlsaVolume::PercentToVolume(double percent, long min, long max)
{
  // Max log value
  static double sMaxValue = LOGARITHM(101.0);
  // Get log from clamped value 1..101
  double value = LOGARITHM(Math::clampf(percent, 0.0, 100.0) + 1.0);
  // Stretch value
  double volume = ((value * (double)(max - min)) / sMaxValue) + (double)min;
  return lround(volume);
}

int AlsaVolume::VolumeToPercent(double raw, long min, long max)
{
  // Max log value
  static double sMaxValue = LOGARITHM(101.0);
  // Get log value
  double volume = ((raw - (double)min) * sMaxValue) / (double)(max - min);
  // Get exponential
  double value = Math::clampf(EXPONENTIAL(volume) - 1, 0.0, 100.0);
  return (int)lround(value);
}

