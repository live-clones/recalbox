//
// Created by bkg2k on 30/10/2020.
//

#include <utils/locale/LocaleHelper.h>
#include <components/SliderComponent.h>
#include <audio/AudioController.h>
#include <audio/AudioManager.h>
#include <algorithm>
#include "GuiMenuSound.h"
#include "MenuMessages.h"

GuiMenuSound::GuiMenuSound(Window& window)
  : Gui(window)
  , mMenu(window, _("NETWORK SETTINGS"))
{
  addChild(&mMenu);

  auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();

  // volume
  mVolume = std::make_shared<SliderComponent>(mWindow, 0.f, 100.f, 1.f, "%");
  mVolume->setSlider((float) AudioController::Instance().GetVolume());
  if (!AudioController::Instance().HasSpecialAudio())
  {
    mVolume->setSelectedChangedCallback([this](const float& volume) { SetVolume(volume); });
    mMenu.addWithLabel(mVolume, _("SYSTEM VOLUME"), _(MENUMESSAGE_SOUND_VOLUME_HELP_MSG));
  }

  // disable sounds
  mMusic = std::make_shared<SwitchComponent>(mWindow, RecalboxConf::Instance().AsBool("audio.bgmusic"));
  mMusic->setChangedCallback([this]() { SetMusicOnOff(mMusic->getState()); });
  mMenu.addWithLabel(mMusic, _("FRONTEND MUSIC"), _(MENUMESSAGE_SOUND_FRONTEND_MUSIC_HELP_MSG));

  // audio device
  mOutputList = std::make_shared<OptionListComponent<std::string> >(mWindow, _("OUTPUT DEVICE"),false, FONT_SIZE_EXTRASMALL);
  std::string currentDevice = RecalboxConf::Instance().AsString("audio.device");

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
    mOutputList->setSelectedChangedCallback([](const std::string &output) { SetOutput(output); });
    mMenu.addWithLabel(mOutputList, _("OUTPUT DEVICE"), _(MENUMESSAGE_SOUND_DEVICE_HELP_MSG));
  }

  // Buttons
  mMenu.addButton(_("CLOSE"), _("CLOSE"), [this] { Close(); } );

  mMenu.setPosition((Renderer::getDisplayWidthAsFloat() - mMenu.getSize().x()) / 2, (Renderer::getDisplayHeightAsFloat() - mMenu.getSize().y()) / 2);
}

void GuiMenuSound::SetVolume(float volume)
{
  AudioController::Instance().SetVolume(Math::roundi(volume));
  RecalboxConf::Instance().SetInt("audio.volume", Math::roundi(volume));
  RecalboxConf::Instance().Save();
}

void GuiMenuSound::SetMusicOnOff(bool on)
{
  if (on) AudioManager::Instance().StartPlaying(ThemeData::getCurrent());
  else AudioManager::Instance().StopAll();
  RecalboxConf::Instance().SetBool("audio.bgmusic", on);
  RecalboxConf::Instance().Save();
}

void GuiMenuSound::SetOutput(const std::string& output)
{
  AudioManager::Instance().Deactivate();
  AudioController::Instance().SetDefaultPlayback(output);
  AudioManager::Instance().Reactivate();
  RecalboxConf::Instance().SetString("audio.device", output);
  RecalboxConf::Instance().Save();
}

bool GuiMenuSound::getHelpPrompts(Help& help)
{
  help.Clear()
      .Set(HelpType::A, _("CLOSE"));
  return true;
}

void GuiMenuSound::Update(int deltaTime)
{
  Component::Update(deltaTime);

  // Synchronize volume bar if the volume is modified elsewhere
  int realVolume = AudioController::Instance().GetVolume();
  if (realVolume != (int)mVolume->getSlider())
    mVolume->setSlider((float)realVolume);
}

