//
// Created by bkg2k on 13/08/2020.
//

#include <utils/math/Misc.h>
#include <utils/Log.h>
#include "AlsaMixer.h"
#include <alsa/asoundlib.h>

void AlsaMixer::SetVolume(int volume)
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
        LOG(LogError) << "Failed to set mixer volume for " << mMixerName;
    }
    else LOG(LogError) << "Failed to get volume range for " << mMixerName;
    CloseMixer();
  }
}

int AlsaMixer::GetVolume()
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
      else LOG(LogError) << "Failed to get mixer volume for " << mMixerName;
    }
    else LOG(LogError) << "Failed to get volume range for " << mMixerName;
    CloseMixer();
  }

  return volume;
}

snd_mixer_elem_t* AlsaMixer::OpenMixer()
{
  // Sets simple-mixer index and name
  snd_mixer_selem_id_t* mixerSelemId = nullptr;
  snd_mixer_selem_id_alloca(&mixerSelemId);
  snd_mixer_selem_id_set_index(mixerSelemId, mMixerId);
  snd_mixer_selem_id_set_name(mixerSelemId, mMixerName.data());

  // Build card identifier
  std::string card("hw:");
  card.append(Strings::ToString(mCardReference));

  //open mixer
  int err = snd_mixer_open(&mMixerHandle, 0);
  if (err < 0) { LOG(LogError) << "Failed to open ALSA mixer: " << snd_strerror(err); return nullptr; }

  err = snd_mixer_attach(mMixerHandle, card.data());
  if (err < 0) { LOG(LogError) << "Failed to attach card " << card << ' ' << snd_strerror(err); CloseMixer(); return nullptr; }

  err = snd_mixer_selem_register(mMixerHandle, nullptr, nullptr);
  if (err < 0) { LOG(LogError) << "Failed to register simple element class " << snd_strerror(err); CloseMixer(); return nullptr; }

  err = snd_mixer_load(mMixerHandle);
  if (err < 0) { LOG(LogError) << "VolumeControl::init() - Failed to load mixer elements " << snd_strerror(err); CloseMixer(); return nullptr; }

  snd_mixer_elem_t* mixerElem = snd_mixer_find_selem(mMixerHandle, mixerSelemId);
  if (mixerElem == nullptr) { LOG(LogError) << "Failed to find mixer elements " << mMixerName << ',' << mMixerId; }

  return mixerElem;
}

void AlsaMixer::CloseMixer()
{
  if (mMixerHandle != nullptr)
  {
    // Build card identifier
    std::string card("hw:");
    card.append(Strings::ToString(mCardReference));

    snd_mixer_detach(mMixerHandle, card.data());
    snd_mixer_free(mMixerHandle);
    snd_mixer_close(mMixerHandle);
    mMixerHandle = nullptr;
  }
}

