//
// Created by bkg2k on 13/08/2020.
//

#include <utils/math/Misc.h>
#include <utils/Log.h>
#include "AlsaMixer.h"
#include <alsa/asoundlib.h>

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
  if (err < 0) { LOG(LogError) << "[AlsaMixer] Failed to open ALSA mixer: " << snd_strerror(err); return nullptr; }

  err = snd_mixer_attach(mMixerHandle, card.data());
  if (err < 0) { LOG(LogError) << "[AlsaMixer] Failed to attach card " << card << ' ' << snd_strerror(err); CloseMixer(); return nullptr; }

  err = snd_mixer_selem_register(mMixerHandle, nullptr, nullptr);
  if (err < 0) { LOG(LogError) << "[AlsaMixer] Failed to register simple element class " << snd_strerror(err); CloseMixer(); return nullptr; }

  err = snd_mixer_load(mMixerHandle);
  if (err < 0) { LOG(LogError) << "[AlsaMixer] VolumeControl::init() - Failed to load mixer elements " << snd_strerror(err); CloseMixer(); return nullptr; }

  snd_mixer_elem_t* mixerElem = snd_mixer_find_selem(mMixerHandle, mixerSelemId);
  if (mixerElem == nullptr) { LOG(LogError) << "[AlsaMixer] Failed to find mixer elements " << mMixerName << ',' << mMixerId; }

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

