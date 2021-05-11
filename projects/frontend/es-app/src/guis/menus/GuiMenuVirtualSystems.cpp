//
// Created by bkg2k on 08/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include <guis/menus/GuiMenuVirtualSystems.h>
#include <components/SwitchComponent.h>
#include <utils/locale/LocaleHelper.h>
#include <MainRunner.h>
#include <guis/MenuMessages.h>
#include "GuiMenuArcadeVirtualSystem.h"
#include <guis/menus/GuiMenuVirtualSystemPerGenre.h>
#include <systems/SystemManager.h>

GuiMenuVirtualSystems::GuiMenuVirtualSystems(WindowManager& window, SystemManager& systemManager)
  : GuiMenuBase(window, _("VIRTUAL SYSTEMS PER GENRE"), this)
  , mSystemManager(systemManager)
  , mAllGamesOriginalValues(RecalboxConf::Instance().GetCollectionAllGames())
  , mMultiplayersOriginalValues(RecalboxConf::Instance().GetCollectionMultiplayer())
  , mLastPlayedOriginalValues(RecalboxConf::Instance().GetCollectionLastPlayed())
  , mLightGunOriginalValues(!RecalboxConf::Instance().GetCollectionHide("lightgun"))
{
  // All games
  mAllGames = AddSwitch(_("SHOW ALL-GAMES SYSTEM"), mAllGamesOriginalValues, (int)Components::AllGames, this, _(MENUMESSAGE_ADVANCED_ALLGAMES_HELP_MSG));

  // Multiplayers
  mMultiplayers = AddSwitch(_("SHOW MULTIPLAYER SYSTEM"), mMultiplayersOriginalValues, (int)Components::Multiplayers, this, _(MENUMESSAGE_ADVANCED_MULTIPLAYERS_HELP_MSG));

  // Last Played
  mLastPlayed = AddSwitch(_("SHOW LAST-PLAYED SYSTEM"), mLastPlayedOriginalValues, (int)Components::LastPlayed, this, _(MENUMESSAGE_ADVANCED_LASTPLAYED_HELP_MSG));

  // Last Played
  BoardType board = Board::Instance().GetBoardType();
  if (board != BoardType::OdroidAdvanceGo && board != BoardType::OdroidAdvanceGoSuper)
    mLastPlayed = AddSwitch(_("SHOW LIGHTGUN SYSTEM"), mLightGunOriginalValues, (int)Components::LightGun, this, _(MENUMESSAGE_ADVANCED_LIGHTGUN_HELP_MSG));

  AddSubMenu(_("VIRTUAL SYSTEMS PER GENRE"), (int)Components::VirtualPerGenre, _(MENUMESSAGE_ADVANCED_VIRTUALGENRESYSTEMS_HELP_MSG));
  AddSubMenu(_("ARCADE VIRTUAL SYSTEM"), (int)Components::VirtualArcade, _(MENUMESSAGE_ADVANCED_ARCADEVIRTUALSYSTEM_HELP_MSG));
}

GuiMenuVirtualSystems::~GuiMenuVirtualSystems()
{
  if ((mLastPlayedOriginalValues != RecalboxConf::Instance().GetCollectionLastPlayed()) ||
      (mMultiplayersOriginalValues != RecalboxConf::Instance().GetCollectionMultiplayer()) ||
      (mLightGunOriginalValues == RecalboxConf::Instance().GetCollectionHide("lightgun")) ||
      (mAllGamesOriginalValues != RecalboxConf::Instance().GetCollectionAllGames()))
    RequestRelaunch();
}

void GuiMenuVirtualSystems::SubMenuSelected(int id)
{
  if ((Components)id == Components::VirtualPerGenre) mWindow.pushGui(new GuiMenuVirtualSystemPerGenre(mWindow));
  else if ((Components)id == Components::VirtualArcade) mWindow.pushGui(new GuiMenuArcadeVirtualSystem(mWindow, mSystemManager));
}

void GuiMenuVirtualSystems::SwitchComponentChanged(int id, bool status)
{
  switch((Components)id)
  {
    case Components::AllGames: RecalboxConf::Instance().SetCollectionAllGames(status).Save(); break;
    case Components::Multiplayers: RecalboxConf::Instance().SetCollectionMultiplayer(status).Save(); break;
    case Components::LastPlayed: RecalboxConf::Instance().SetCollectionLastPlayed(status).Save(); break;
    case Components::LightGun: RecalboxConf::Instance().SetCollectionHide("lightgun", !status).Save(); break;
    case Components::VirtualPerGenre:
    case Components::VirtualArcade: break;
  }
}
