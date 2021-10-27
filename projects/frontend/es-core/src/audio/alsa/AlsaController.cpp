//
// Created by bkg2k on 13/08/2020.
//

#include <RecalboxConf.h>
#include <utils/Files.h>
#include <utils/cplusplus/StaticLifeCycleControler.h>
#include <utils/math/Misc.h>
#include "AlsaController.h"
#include <audio/alsa/Raspberry.h>
#include <hardware/Board.h>

void AlsaController::Initialize()
{
  // Card loop
  for (int cardNum = -1;;)
  {
    // Get next sound card's card number. When "cardNum" == -1, then ALSA fetches the first card
    int err = snd_card_next(&cardNum);
    if (err < 0)
    {
      { LOG(LogError) << "[Alsa] Can't get the next card number: " << snd_strerror(err); }
      break;
    }
    if (cardNum < 0) break;

    // Open this card's (cardNum's) control interface. We specify only the card number -- not any device nor sub-device
    snd_ctl_t* cardHandle = nullptr;
    std::string param("hw:");
    param.append(Strings::ToString(cardNum));
    if ((err = snd_ctl_open(&cardHandle, param.data(), 0)) < 0)	//Now cardHandle becomes your sound card.
    {
      { LOG(LogError) << "[Alsa] Can't open card " << cardNum << ": " << snd_strerror(err); }
      continue;
    }

    snd_ctl_card_info_t* cardInfo = nullptr; // hold card information
    snd_ctl_card_info_alloca(&cardInfo); // Allocate (?!)
    //Tell ALSA to fill in our snd_ctl_card_info_t with info about this card
    if ((err = snd_ctl_card_info(cardHandle, cardInfo)) < 0)
    {
      { LOG(LogError) << "[Alsa] Can't get info for card " << cardNum << ": " << snd_strerror(err); }
    }
    else
    {
      { LOG(LogDebug) <<"[Alsa] Found Card " << cardNum << ": " << snd_ctl_card_info_get_name(cardInfo); }
      AlsaCard card(cardNum, snd_ctl_card_info_get_name(cardInfo));

      snd_mixer_t *handle = nullptr;
      snd_mixer_selem_id_t *sid = nullptr;
      snd_mixer_selem_id_alloca(&sid);
      if ((err = snd_mixer_open(&handle, 0)) < 0) { LOG(LogError) << "[Alsa] Mixer " << param << " open error: " << snd_strerror(err); continue; }
      if ((err = snd_mixer_attach(handle, param.data())) < 0) { LOG(LogError) << "[Alsa] Mixer attach " << param << " error: " << snd_strerror(err); snd_mixer_close(handle); continue; }
      if ((err = snd_mixer_selem_register(handle, nullptr, nullptr)) < 0) { LOG(LogError) << "[Alsa] Mixer register error: " << snd_strerror(err); snd_mixer_close(handle); continue; }
      if ((err = snd_mixer_load(handle)) < 0) { LOG(LogError) << "[Alsa] Mixer " << param << " load error: " << snd_strerror(err); snd_mixer_close(handle); continue; }

      for (snd_mixer_elem_t* elem = snd_mixer_first_elem(handle); elem != nullptr; elem = snd_mixer_elem_next(elem))
      {
        snd_mixer_selem_get_id(elem, sid);
        if (snd_mixer_selem_has_playback_volume(elem) != 0)
        {
          { LOG(LogDebug) << "[Alsa]   Mixer control (Volume) '" << snd_mixer_selem_id_get_name(sid) << "'," << snd_mixer_selem_id_get_index(sid); }
          card.AddVolumeControl(AlsaVolume((int) snd_mixer_selem_id_get_index(sid), snd_mixer_selem_id_get_name(sid), cardNum));
        }
        else if (snd_mixer_selem_has_playback_switch(elem) != 0)
        {
          { LOG(LogDebug) << "[Alsa]   Mixer control (Switch) '" << snd_mixer_selem_id_get_name(sid) << "'," << snd_mixer_selem_id_get_index(sid); }
          card.AddSwitch(AlsaSwitch((int) snd_mixer_selem_id_get_index(sid), snd_mixer_selem_id_get_name(sid), cardNum));
        }
        else
        {
          if (Board::Instance().GetBoardType() == BoardType::OdroidAdvanceGo)
            if (snd_mixer_selem_is_enumerated(elem) != 0)
              if (strcmp(snd_mixer_selem_id_get_name(sid), "Playback Path") == 0)
              {
                { LOG(LogDebug) << "[Alsa]   Odroid Advance Go2's Path selector detected '" << snd_mixer_selem_id_get_name(sid) << "'," << snd_mixer_selem_id_get_index(sid); }
                card.AddOdroidAdvanceGo2Router(OdroidAdvanceGo2Alsa((int) snd_mixer_selem_id_get_index(sid), snd_mixer_selem_id_get_name(sid), cardNum));
                continue;
              }
          { LOG(LogDebug) << "[Alsa]   Ignored '" << snd_mixer_selem_id_get_name(sid) << "'," << snd_mixer_selem_id_get_index(sid); }
        }
      }

      snd_mixer_detach(handle, param.data());
      snd_mixer_close(handle);

      for (int devNum = -1;;)
      {
        // Get next sound device's device number. When "devNum" == -1, then ALSA fetches the first device
        err = snd_ctl_pcm_next_device(cardHandle, &devNum);
        if (err < 0)
        {
          { LOG(LogError) << "[Alsa] Can't get the next device number: " << snd_strerror(err) << " of " << snd_ctl_card_info_get_name(cardInfo); }
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
          { LOG(LogError) << "[Alsa] Can't get the device informations: " << snd_strerror(err) << " of " << snd_ctl_card_info_get_name(cardInfo); }
          break;
        }
        unsigned int nsubd = snd_pcm_info_get_subdevices_count(pcminfo);
        { LOG(LogDebug) << "[Alsa]   Device " << devNum << ", ID `" << snd_pcm_info_get_id(pcminfo) << "`, name `"
                        << snd_pcm_info_get_name(pcminfo) << "`, " << nsubd << " subdevices ("
                        << snd_pcm_info_get_subdevices_avail(pcminfo) <<" available)"; }

        for(unsigned int subd=0; subd < nsubd; ++subd)
        {
          snd_pcm_info_set_subdevice(pcminfo, subd);
          if (snd_ctl_pcm_info(cardHandle, pcminfo) >= 0)
          { LOG(LogDebug) << "[Alsa]       Subdevice " << subd << ", name `" << snd_pcm_info_get_subdevice_name(pcminfo) << "`"; }
        }

        if (Board::Instance().GetBoardType() == BoardType::OdroidAdvanceGo)
        {
          card.AddDevice(AlsaDevice((int)OdroidAdvanceGo2Alsa::OutputPath::Off, OdroidAdvanceGo2Alsa::sOff, 0));
          card.AddDevice(AlsaDevice((int)OdroidAdvanceGo2Alsa::OutputPath::Speaker, OdroidAdvanceGo2Alsa::sSpeaker, 0));
          card.AddDevice(AlsaDevice((int)OdroidAdvanceGo2Alsa::OutputPath::Headphone, OdroidAdvanceGo2Alsa::sHeadphone, 0));
          card.AddDevice(AlsaDevice((int)OdroidAdvanceGo2Alsa::OutputPath::Both, OdroidAdvanceGo2Alsa::sBoth, 0));
        }
        else card.AddDevice(AlsaDevice(devNum, snd_pcm_info_get_name(pcminfo), (int)nsubd));
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

HashMap<int, std::string> AlsaController::GetPlaybackList()
{
  HashMap<int, std::string> result;
  result[-1] = sDefaultOutput;
  for(const AlsaCard& playback : mPlaybacks)
  {
    if (Board::Instance().GetBoardType() == BoardType::OdroidAdvanceGo)
    {
      result[-1] = sDefaultOutput;
      for (int i = playback.DeviceCount(); --i >= 0;)
        result[playback.DeviceAt(i).Identifier()] = playback.DeviceAt(i).Name();
    }
    else
    {
      int cardIndex = playback.Identifier() << 16 | 0xFFFF;
      if (!playback.Name().empty())
        result[cardIndex] = playback.Name() + " (default output)";
      for (int i = playback.DeviceCount(); --i >= 0;)
      {
        int cardDeviceIndex = (playback.Identifier() << 16) | playback.DeviceAt(i).Identifier();
        if (playback.Name() != playback.DeviceAt(i).Name() && !playback.Name().empty())
          // Complete form "Card (Device)"
          result[cardDeviceIndex] = playback.Name() + " (" + playback.DeviceAt(i).Name() + ')';
        else
          // Simplified form "Card"
          result[cardDeviceIndex] = playback.DeviceAt(i).Name();
      }
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
  { LOG(LogError) << "[Alsa] Cannot find audio device " << playbackName; }
  SetDefaultPlayback(-1);
  return sDefaultOutput;
}

bool AlsaController::LookupCardDevice(int identifier, int& cardIndex, int& deviceIndex)
{
  int deviceIdentifier = ((identifier & 0xFFFF) << 16) >> 16;
  int cardIdentifier = (identifier >> 16) & 0xFFFF;
  { LOG(LogInfo) << "[Alsa] Requested ALSA output Card #" << cardIdentifier << " Device #" << deviceIdentifier; }

  bool found = false;
  for(int i = 0; i < (int)mPlaybacks.size(); ++i)
  {
    const AlsaCard& playback = mPlaybacks[i];
    // Lookup card or take the first
    if (playback.Identifier() == cardIdentifier || cardIdentifier < 0)
    {
      cardIndex = i;
      deviceIndex = -1;
      found = (playback.DeviceCount() == 0); // No device = found
      // Lookup device identifier
      for(int j = playback.DeviceCount(); !found && --j >= 0; )
        if (deviceIdentifier == playback.DeviceAt(j).Identifier())
        {
          deviceIndex = j;
          found = true;
        }
      // Default device requested?
      if (!found && deviceIdentifier < 0)
      {
        deviceIndex = 0;
        deviceIdentifier = playback.DeviceAt(0).Identifier();
        found = true;
      }

      break;
    }
  }

  if (found) { LOG(LogInfo) << "[Alsa] Found ALSA output Card #" << cardIdentifier << " Device #" << deviceIdentifier; }
  else { LOG(LogError) << "[Alsa] Output Card#" << cardIdentifier << " device#" << deviceIdentifier << " NOT FOUND!"; }

  return found;
}

void AlsaController::SetDefaultPlayback(int identifier)
{
  static Path asoundrcPath("/overlay/.configs/.asoundrc");

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

  // Odroid Advance go 2 patch
  if (Board::Instance().GetBoardType() == BoardType::OdroidAdvanceGo)
  {
    if (identifier == -1) identifier = (int)OdroidAdvanceGo2Alsa::OutputPath::Speaker;
    mPlaybacks[0].OdroidAdvanceGo2Router()->Route((OdroidAdvanceGo2Alsa::OutputPath)identifier);
    return;
  }

  // Default card & device - Let ALSA deciding
  if (identifier == -1)
  {
    { LOG(LogInfo) << "[Alsa] Default alsa output selected. Removing .asoundrc & AUDIOENV"; }
    asoundrcPath.Delete();
    unsetenv("AUDIODEV");
    return;
  }

  // No sound card?
  if (mPlaybacks.empty()) return;

  int deviceIndex = -1;
  int cardIndex = -1;
  if (LookupCardDevice(identifier, cardIndex, deviceIndex))
  {
    int cardIdentifier = mPlaybacks[cardIndex].Identifier();
    int deviceIdentifier = deviceIndex >= 0 ? mPlaybacks[cardIndex].DeviceAt(deviceIndex).Identifier() : -1;
    // Build & save ~/.asoundrc
    std::string asoundrcContent(deviceIndex < 0 ? asoundrcPatternCardOnly : asoundrcPatternCardAndDevice);
    Strings::ReplaceAllIn(asoundrcContent, "{@CARD@}", Strings::ToString(cardIdentifier));
    if (deviceIndex >= 0)
      Strings::ReplaceAllIn(asoundrcContent, "{@DEVICE@}", Strings::ToString(deviceIdentifier));
    Files::SaveFile(asoundrcPath, asoundrcContent);
    // Set env
    setenv("AUDIODEV", ("hw:" + Strings::ToString(cardIdentifier) + ',' + Strings::ToString(deviceIdentifier)).data(), 1);
    // Activate
    mPlaybacks[cardIndex].SwitchOn();
    // Final lof
    { LOG(LogInfo) << "[Alsa] ALSA output set to Card #" << cardIdentifier << " (index: " << cardIndex << ") Device #" << deviceIdentifier << " 'index: " << deviceIndex << ')'; }

    // Raspberry Pi hack
    switch(Board::Instance().GetBoardType())
    {
      case BoardType::Pi1:
      case BoardType::Pi2:
      case BoardType::Pi3:
      case BoardType::Pi3plus:
      {
        Raspberry::SetRoute(deviceIndex == 0 ? Raspberry::Output::Headphones : Raspberry::Output::HDMI);
        break;
      }
      case BoardType::UndetectedYet:
      case BoardType::Unknown:
      case BoardType::Pi0:
      case BoardType::Pi02:
      case BoardType::Pi4:
      case BoardType::Pi400:
      case BoardType::UnknownPi:
      case BoardType::OdroidAdvanceGo:
      case BoardType::PCx86:
      case BoardType::PCx64:
      default: break;
    }
  }
  else
  {
    { LOG(LogError) << "[Alsa] Error setting ALSA output. Try to set default."; }
    // Set default card/default device
    SetDefaultPlayback(-1);
  }
}

int AlsaController::GetVolume()
{
  return Math::clampi(RecalboxConf::Instance().AsInt("audio.volume"),0, 100);
}

void AlsaController::SetVolume(int volume)
{
  for(AlsaCard& playback : mPlaybacks)
    playback.SetVolume(volume);
}

