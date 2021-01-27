//
// Created by bkg2k on 30/10/2020.
//

#include <utils/locale/LocaleHelper.h>
#include <components/SliderComponent.h>
#include <audio/AudioController.h>
#include <audio/AudioManager.h>
#include <algorithm>
#include "GuiMenuSound.h"
#include "guis/MenuMessages.h"

GuiMenuSound::GuiMenuSound(WindowManager& window)
  : GuiMenuBase(window, _("SOUND SETTINGS"), nullptr)
{
  // Volume
  if (!AudioController::Instance().HasSpecialAudio())
    mVolume = AddSlider(_("SYSTEM VOLUME"), 0.f, 100.f, 1.f, (float)AudioController::Instance().GetVolume(), "%", (int)Components::Volume, this, _(MENUMESSAGE_SOUND_VOLUME_HELP_MSG));

  // Music on/off
  mMusic = AddSwitch(_("FRONTEND MUSIC"), RecalboxConf::Instance().GetAudioMusic(), (int)Components::Music, this, _(MENUMESSAGE_SOUND_FRONTEND_MUSIC_HELP_MSG));

  // Audio device
  if (!AudioController::Instance().HasSpecialAudio() && RecalboxConf::Instance().GetMenuType() != RecalboxConf::Menu::Bartop)
    mOutputList = AddList<std::string>(_("OUTPUT DEVICE"), (int)Components::Output, this, GetOutputEntries(), _(MENUMESSAGE_SOUND_DEVICE_HELP_MSG));
}

std::vector<GuiMenuBase::ListEntry<std::string>> GuiMenuSound::GetOutputEntries()
{
  std::vector<ListEntry<std::string>> list;

  std::string currentDevice = RecalboxConf::Instance().GetAudioOuput();
  HashMap<int, std::string> playbacksMap = AudioController::Instance().GetPlaybackList();
  std::vector<std::string> availableAudio;
  // Scan default
  for(const auto& playback : playbacksMap)
    if (playback.first == -1) // default item
      availableAudio.push_back(playback.second);
  // Add all remaining stuff
  for(const auto& playback : playbacksMap)
    if (playback.first != -1) // default item
      availableAudio.push_back(playback.second);
  // Sort
  if (!availableAudio.empty())
    std::sort(availableAudio.begin() + 1, availableAudio.end());

  for (auto& it : availableAudio)
    list.push_back({ _S(it), it, currentDevice == it });

  return list;
}

void GuiMenuSound::Update(int deltaTime)
{
  GuiMenuBase::Update(deltaTime);

  // Synchronize volume bar if the volume is modified elsewhere
  int realVolume = AudioController::Instance().GetVolume();
  if (realVolume != (int)mVolume->getSlider())
    mVolume->setSlider((float)realVolume);
}

void GuiMenuSound::OptionListComponentChanged(int id, int index, const std::string& value)
{
  (void)index;
  if ((Components)id == Components::Output)
  {
    AudioManager::Instance().Deactivate();
    AudioController::Instance().SetDefaultPlayback(value);
    AudioManager::Instance().Reactivate();
    RecalboxConf::Instance().SetAudioOuput(value).Save();
  }
}

void GuiMenuSound::SwitchComponentChanged(int id, bool status)
{
  if ((Components)id == Components::Music)
  {
    if (status) AudioManager::Instance().StartPlaying(ThemeData::getCurrent());
    else AudioManager::Instance().StopAll();
    RecalboxConf::Instance().SetAudioMusic(status).Save();
  }
}

void GuiMenuSound::SliderMoved(int id, float value)
{
  if ((Components)id == Components::Volume && AudioController::Instance().GetVolume() != Math::roundi(value))
  {
    AudioController::Instance().SetVolume(Math::roundi(value));
    RecalboxConf::Instance().SetAudioVolume(Math::roundi(value)).Save();
  }
}

