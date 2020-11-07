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

GuiMenuSound::GuiMenuSound(Window& window)
  : GuiMenuBase(window, _("SOUND SETTINGS"))
{
  auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();

  // volume
  mVolume = std::make_shared<SliderComponent>(mWindow, 0.f, 100.f, 1.f, "%");
  mVolume->setSlider((float) AudioController::Instance().GetVolume());
  if (!AudioController::Instance().HasSpecialAudio())
  {
    mVolume->setSelectedChangedCallback(SetVolume);
    mMenu.addWithLabel(mVolume, _("SYSTEM VOLUME"), _(MENUMESSAGE_SOUND_VOLUME_HELP_MSG));
  }

  // disable sounds
  mMusic = std::make_shared<SwitchComponent>(mWindow, RecalboxConf::Instance().GetAudioMusic());
  mMusic->setChangedCallback(SetMusicOnOff);
  mMenu.addWithLabel(mMusic, _("FRONTEND MUSIC"), _(MENUMESSAGE_SOUND_FRONTEND_MUSIC_HELP_MSG));

  // audio device
  mOutputList = std::make_shared<OptionListComponent<std::string> >(mWindow, _("OUTPUT DEVICE"),false, FONT_SIZE_EXTRASMALL);
  std::string currentDevice = RecalboxConf::Instance().GetAudioOuput();

  // Transform/Sort playback list
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
  std::sort(availableAudio.begin() + 1, availableAudio.end());

  if (!AudioController::Instance().HasSpecialAudio() && RecalboxConf::Instance().AsString("emulationstation.menu") != "bartop")
  {
    for (auto& it : availableAudio) mOutputList->add(_S(it), it, currentDevice == it);
    mOutputList->setChangedCallback([this] { SetOutput(mOutputList->getSelected()); });
    mMenu.addWithLabel(mOutputList, _("OUTPUT DEVICE"), _(MENUMESSAGE_SOUND_DEVICE_HELP_MSG));
  }
}

void GuiMenuSound::SetVolume(float volume)
{
  AudioController::Instance().SetVolume(Math::roundi(volume));
  RecalboxConf::Instance().SetAudioVolume(Math::roundi(volume));
  RecalboxConf::Instance().Save();
}

void GuiMenuSound::SetMusicOnOff(bool on)
{
  if (on) AudioManager::Instance().StartPlaying(ThemeData::getCurrent());
  else AudioManager::Instance().StopAll();
  RecalboxConf::Instance().SetAudioMusic(on);
  RecalboxConf::Instance().Save();
}

void GuiMenuSound::SetOutput(const std::string& output)
{
  AudioManager::Instance().Deactivate();
  AudioController::Instance().SetDefaultPlayback(output);
  AudioManager::Instance().Reactivate();
  RecalboxConf::Instance().SetAudioOuput(output);
  RecalboxConf::Instance().Save();
}

void GuiMenuSound::Update(int deltaTime)
{
  GuiMenuBase::Update(deltaTime);

  // Synchronize volume bar if the volume is modified elsewhere
  int realVolume = AudioController::Instance().GetVolume();
  if (realVolume != (int)mVolume->getSlider())
    mVolume->setSlider((float)realVolume);
}

