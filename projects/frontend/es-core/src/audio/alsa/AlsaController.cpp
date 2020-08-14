//
// Created by bkg2k on 13/08/2020.
//

#include <Settings.h>
#include <utils/Files.h>
#include "AlsaController.h"

void AlsaController::Initialize()
{
  // Card loop
  for (int cardNum = -1;;)
  {
    // Get next sound card's card number. When "cardNum" == -1, then ALSA fetches the first card
    int err = snd_card_next(&cardNum);
    if (err < 0)
    {
      LOG(LogError) << "Can't get the next card number: " << snd_strerror(err);
      break;
    }
    if (cardNum < 0) break;

    // Open this card's (cardNum's) control interface. We specify only the card number -- not any device nor sub-device
    snd_ctl_t* cardHandle = nullptr;
    std::string param("hw:");
    param.append(Strings::ToString(cardNum));
    if ((err = snd_ctl_open(&cardHandle, param.data(), 0)) < 0)	//Now cardHandle becomes your sound card.
    {
      LOG(LogError) << "Can't open card " << cardNum << ": " << snd_strerror(err);
      continue;
    }

    snd_ctl_card_info_t* cardInfo = nullptr; // hold card information
    snd_ctl_card_info_alloca(&cardInfo); // Allocate (?!)
    //Tell ALSA to fill in our snd_ctl_card_info_t with info about this card
    if ((err = snd_ctl_card_info(cardHandle, cardInfo)) < 0)
    {
      LOG(LogError) << "Can't get info for card " << cardNum << ": " << snd_strerror(err);
    }
    else
    {
      LOG(LogDebug) <<"Found Card " << cardNum << ": " << snd_ctl_card_info_get_name(cardInfo);
      AlsaCard card(cardNum, snd_ctl_card_info_get_name(cardInfo));

      snd_mixer_t *handle = nullptr;
      snd_mixer_selem_id_t *sid = nullptr;
      snd_mixer_selem_id_alloca(&sid);
      if ((err = snd_mixer_open(&handle, 0)) < 0) { LOG(LogError) << "Mixer " << param << " open error: " << snd_strerror(err); continue; }
      if ((err = snd_mixer_attach(handle, param.data())) < 0) { LOG(LogError) << "Mixer attach " << param << " error: " << snd_strerror(err); snd_mixer_close(handle); continue; }
      if ((err = snd_mixer_selem_register(handle, nullptr, nullptr)) < 0) { LOG(LogError) << "Mixer register error: " << snd_strerror(err); snd_mixer_close(handle); continue; }
      if ((err = snd_mixer_load(handle)) < 0) { LOG(LogError) << "Mixer " << param << " load error: " << snd_strerror(err); snd_mixer_close(handle); continue; }

      for (snd_mixer_elem_t* elem = snd_mixer_first_elem(handle); elem != nullptr; elem = snd_mixer_elem_next(elem))
      {
        snd_mixer_selem_get_id(elem, sid);
        if (snd_mixer_selem_has_playback_volume(elem) == 0) continue;
        LOG(LogDebug) << "  Simple Mixer control '" << snd_mixer_selem_id_get_name(sid) <<"'," << snd_mixer_selem_id_get_index(sid);
        card.AddMixer(AlsaMixer((int)snd_mixer_selem_id_get_index(sid), snd_mixer_selem_id_get_name(sid), cardNum));
      }

      snd_mixer_detach(handle, param.data());
      snd_mixer_close(handle);

      for (int devNum = -1;;)
      {
        // Get next sound device's device number. When "devNum" == -1, then ALSA fetches the first device
        err = snd_ctl_pcm_next_device(cardHandle, &devNum);
        if (err < 0)
        {
          LOG(LogError) << "Can't get the next device number: " << snd_strerror(err) << " of " << snd_ctl_card_info_get_name(cardInfo);
          break;
        }
        if (devNum < 0) break;

        snd_pcm_info_t* pcminfo = nullptr; // hold device information
        snd_pcm_info_alloca(&pcminfo); // Allocate (?!)

        snd_pcm_info_set_device(pcminfo, devNum);
        snd_pcm_info_set_subdevice(pcminfo, 0);
        snd_pcm_info_set_stream(pcminfo, SND_PCM_STREAM_PLAYBACK);
        err = snd_ctl_pcm_info(cardHandle, pcminfo);
        if (err < 0)
        {
          LOG(LogError) << "Can't get the device informations: " << snd_strerror(err) << " of " << snd_ctl_card_info_get_name(cardInfo);
          break;
        }
        unsigned int nsubd = snd_pcm_info_get_subdevices_count(pcminfo);
        LOG(LogDebug) << "  Device " << devNum << ", ID `" << snd_pcm_info_get_id(pcminfo) << "`, name `"
                      << snd_pcm_info_get_name(pcminfo) << "`, " << nsubd << " subdevices ("
                      << snd_pcm_info_get_subdevices_avail(pcminfo) <<" available)";

        for(unsigned int subd=0; subd < nsubd; ++subd)
        {
          snd_pcm_info_set_subdevice(pcminfo, subd);
          if (snd_ctl_pcm_info(cardHandle, pcminfo) >= 0)
            LOG(LogDebug) << "      Subdevice " << subd << ", name `" << snd_pcm_info_get_subdevice_name(pcminfo) << "`";
        }

        card.AddDevice(AlsaDevice(devNum, snd_pcm_info_get_name(pcminfo), (int)nsubd));
      }

      // Add card and all its stuff :)
      mPlaybacks.push_back(card);
    }

    // Close the card's control interface after we're done with it
    snd_ctl_close(cardHandle);
  }

  //ALSA allocates some mem to load its config file when we call some of the
  //above functions. Now that we're done getting the info, let's tell ALSA
  //to unload the info and free up that mem
  snd_config_update_free_global();
}

HashMap<int, std::string> AlsaController::GetPlaybackList() const
{
  HashMap<int, std::string> result;
  result[-1] = "Default";
  for(const AlsaCard& playback : mPlaybacks)
  {
    int cardIndex = playback.Identifier() << 16;
    result[cardIndex] = playback.Name() + " (default output)";
    for(int i = playback.DeviceCount(); --i >= 0; )
    {
      int cardDeviceIndex = (playback.Identifier() << 16) | playback.DeviceAt(i).Identifier();
      if (playback.Name() != playback.DeviceAt(i).Name())
        // Complete form "Card (Device)"
        result[cardDeviceIndex] = playback.Name() + '(' + playback.DeviceAt(i).Name()+ ')';
      else
        // Simplified form "Card"
        result[cardDeviceIndex] = playback.Name();
    }
  }

  return result;
}

std::string AlsaController::SetDefaultPlayback(const std::string& playbackName)
{
  HashMap<int, std::string> playbacks = GetPlaybackList();
  for(auto& playback : playbacks)
  {
    if (playbackName == playback.second)
    {
      SetDefaultPlayback(playback.first);
      return playbackName;
    }
  }
  LOG(LogError) << "Cannot find audio device " << playbackName;
  SetDefaultPlayback(0);
  return "Default";
}

void AlsaController::SetDefaultPlayback(int identifier)
{
  static Path asoundrcPath("/recalbox/share/system/.asoundrc");

  const char* asoundrcPatternCardAndDevice =
    "pcm.!default {\n" \
    "        type hw\n" \
    "        card {@CARD@}\n" \
    "        device {@DEVICE@}\n" \
    "}\n" \
    "\n" \
    "ctl.!default {\n" \
    "        type hw\n" \
    "        card {@CARD@}\n" \
    "}\n";

  const char* asoundrcPatternCardOnly =
    "pcm.!default {\n" \
    "        type hw\n" \
    "        card {@CARD@}\n" \
    "}\n" \
    "\n" \
    "ctl.!default {\n" \
    "        type hw\n" \
    "        card {@CARD@}\n" \
    "}\n";

  // Default - Let ALSA deciding
  if (identifier == -1)
  {
    asoundrcPath.Delete();
    return;
  }

  int deviceIdentifier = identifier & 0xFFFF;
  int cardIdentifier = (identifier >> 16) & 0xFFFF;

  bool found = false;
  for(AlsaCard& playback : mPlaybacks)
  {
    if (playback.Identifier() == cardIdentifier)
    {
      found = (playback.DeviceCount() == 0);
      for(int i = playback.DeviceCount(); !found && --i >= 0; )
        if (deviceIdentifier == playback.DeviceAt(i).Identifier()) found = true;
      break;
    }
  }

  if (found)
  {
    // Build & save ~/.asoundrc
    std::string asoundrcContent(deviceIdentifier == 0 ? asoundrcPatternCardOnly : asoundrcPatternCardAndDevice);
    Strings::ReplaceAllIn(asoundrcContent, "{@CARD@}", Strings::ToString(cardIdentifier));
    Strings::ReplaceAllIn(asoundrcContent, "{@DEVICE@}", Strings::ToString(deviceIdentifier));
    Files::SaveFile(asoundrcPath, asoundrcContent);
  }
  else
  {
    LOG(LogError) << "Cannot set default playback to card " << cardIdentifier << " device " << deviceIdentifier << " because: not found!";
    // Set default card/default device
    SetDefaultPlayback(0);
  }
}

int AlsaController::GetVolume()
{
  return Settings::Instance().SystemVolume();
}

void AlsaController::SetVolume(int volume)
{
  for(AlsaCard& playback : mPlaybacks)
    playback.SetVolume(volume);
}

