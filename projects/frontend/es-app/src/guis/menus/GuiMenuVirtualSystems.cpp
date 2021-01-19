//
// Created by bkg2k on 08/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include <guis/menus/GuiMenuVirtualSystems.h>
#include <components/SwitchComponent.h>
#include <utils/locale/LocaleHelper.h>
#include <guis/GuiMsgBox.h>
#include <MainRunner.h>
#include <guis/MenuMessages.h>
#include "GuiMenuArcadeVirtualSystem.h"
#include <guis/menus/GuiMenuVirtualSystemPerGenre.h>
#include <systems/SystemManager.h>

GuiMenuVirtualSystems::GuiMenuVirtualSystems(WindowManager& window, SystemManager& systemManager)
  : GuiMenuBase(window, _("VIRTUAL SYSTEMS PER GENRE"))
  , mSystemManager(systemManager)
  , mAllGamesOriginalValues(false)
  , mMultiplayersOriginalValues(false)
  , mLastPlayedOriginalValues(false)
{
  // All games
  mAllGames = std::make_shared<SwitchComponent>(window, mAllGamesOriginalValues = RecalboxConf::Instance().GetCollectionAllGames());
  mAllGames->setChangedCallback(SetAllGames);
  mMenu.addWithLabel(mAllGames, _("SHOW ALL-GAMES SYSTEM"), _(MENUMESSAGE_ADVANCED_ALLGAMES_HELP_MSG));

  // Multiplayers
  mMultiplayers = std::make_shared<SwitchComponent>(window, mMultiplayersOriginalValues = RecalboxConf::Instance().GetCollectionMultiplayer());
  mMultiplayers->setChangedCallback(SetMultiplayers);
  mMenu.addWithLabel(mMultiplayers, _("SHOW MULTIPLAYER SYSTEM"), _(MENUMESSAGE_ADVANCED_MULTIPLAYERS_HELP_MSG));

  // Last Played
  mLastPlayed = std::make_shared<SwitchComponent>(window, mLastPlayedOriginalValues = RecalboxConf::Instance().GetCollectionLastPlayed());
  mLastPlayed->setChangedCallback(SetLastPlayed);
  mMenu.addWithLabel(mLastPlayed, _("SHOW LAST-PLAYED SYSTEM"), _(MENUMESSAGE_ADVANCED_LASTPLAYED_HELP_MSG));

  AddSubMenu(_("VIRTUAL SYSTEMS PER GENRE"), [this] { mWindow.pushGui(new GuiMenuVirtualSystemPerGenre(mWindow)); }, _(MENUMESSAGE_ADVANCED_VIRTUALGENRESYSTEMS_HELP_MSG));
  AddSubMenu(_("ARCADE VIRTUAL SYSTEM"), [this] { mWindow.pushGui(new GuiMenuArcadeVirtualSystem(mWindow, mSystemManager)); }, _(MENUMESSAGE_ADVANCED_ARCADEVIRTUALSYSTEM_HELP_MSG));
}

GuiMenuVirtualSystems::~GuiMenuVirtualSystems()
{
  if ((mLastPlayedOriginalValues != RecalboxConf::Instance().GetCollectionLastPlayed()) ||
      (mMultiplayersOriginalValues != RecalboxConf::Instance().GetCollectionMultiplayer()) ||
      (mAllGamesOriginalValues != RecalboxConf::Instance().GetCollectionAllGames()))
  {
    mWindow.pushGui(new GuiMsgBox(mWindow, _("EmulationStation must relaunch to apply your changes."), _("OK"), []
    { MainRunner::RequestQuit(MainRunner::ExitState::Relaunch, true); }));
  }
}

void GuiMenuVirtualSystems::SetAllGames(bool on)
{
  RecalboxConf::Instance().SetCollectionAllGames(on).Save();
}

void GuiMenuVirtualSystems::SetMultiplayers(bool on)
{
  RecalboxConf::Instance().SetCollectionMultiplayer(on).Save();
}

void GuiMenuVirtualSystems::SetLastPlayed(bool on)
{
  RecalboxConf::Instance().SetCollectionLastPlayed(on).Save();
}
