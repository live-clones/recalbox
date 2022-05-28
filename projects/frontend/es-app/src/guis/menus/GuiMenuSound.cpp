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
  mVolume = AddSlider(_("SYSTEM VOLUME"), 0.f, 100.f, 1.f, (float)AudioController::Instance().GetVolume(), "%", (int)Components::Volume, this, _(MENUMESSAGE_SOUND_VOLUME_HELP_MSG));

  // AudioMode
  mAudioMode = AddList<AudioMode>(_("AUDIO MODE"), (int)Components::AudioMode, this, GetAudioModeEntries(), _(MENUMESSAGE_SOUND_MODE_HELP_MSG));

  // Audio device
  if (RecalboxConf::Instance().GetMenuType() != RecalboxConf::Menu::Bartop)
    mOutputList = AddList<std::string>(_("OUTPUT DEVICE"), (int)Components::Output, this, GetOutputEntries(), _(MENUMESSAGE_SOUND_DEVICE_HELP_MSG));
}

std::vector<GuiMenuBase::ListEntry<std::string>> GuiMenuSound::GetOutputEntries()
{
  std::vector<ListEntry<std::string>> list;

  std::string currentDevice = AudioController::Instance().GetActivePlaybackName();
  IAudioController::DeviceList playbackList = AudioController::Instance().GetPlaybackList();
  std::vector<std::string> availableAudio;

  // Add all remaining stuff
  for(const auto& playback : playbackList)
    list.push_back({ playback.DisplayableName, playback.InternalName, currentDevice == playback.InternalName });

  return list;
}

std::vector<GuiMenuBase::ListEntry<AudioMode>> GuiMenuSound::GetAudioModeEntries()
{
  const AudioMode audioMode = RecalboxConf::Instance().GetAudioMode();
  std::vector<ListEntry < AudioMode>> list;
  list.push_back({_("Musics or videos sound"), AudioMode::MusicsXorVideosSound, AudioMode::MusicsXorVideosSound == audioMode });
  list.push_back({_("Musics and videos sound"), AudioMode::MusicsAndVideosSound, AudioMode::MusicsAndVideosSound == audioMode });
  list.push_back({_("Musics only"), AudioMode::MusicsOnly, AudioMode::MusicsOnly == audioMode });
  list.push_back({_("Videos sound only"), AudioMode::VideosSoundOnly, AudioMode::VideosSoundOnly == audioMode });
  list.push_back({_("No sound"), AudioMode::None, AudioMode::None == audioMode });

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

void GuiMenuSound::OptionListComponentChanged(int id, int index, const AudioMode& value)
{
  (void)index;
  if ((Components)id == Components::AudioMode)
  {
    const bool currentCanPlayMusic = AudioModeTools::CanPlayMusic();
    RecalboxConf::Instance().SetAudioMode(value).Save();
    if (!currentCanPlayMusic && AudioModeTools::CanPlayMusic())
      AudioManager::Instance().StartPlaying(ThemeData::getCurrent());
    else if (currentCanPlayMusic && !AudioModeTools::CanPlayMusic())
      AudioManager::Instance().StopAll();
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

