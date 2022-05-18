//
// Created by gugue_u on 29/03/2022.
//
//

#include "GuiMenuGameFilters.h"
#include <RecalboxConf.h>
#include <views/gamelist/ISimpleGameListView.h>
#include <games/GameFilesUtils.h>
#include "views/ViewController.h"
#include "components/SwitchComponent.h"
#include "utils/locale/LocaleHelper.h"
#include "guis/MenuMessages.h"
#include "GuiMenu.h"
#include "guis/GuiMsgBox.h"

GuiMenuGameFilters::GuiMenuGameFilters(WindowManager& window)
  :	GuiMenuBase(window, _("Game Filters"), nullptr)
{


    AddSwitch(_("SHOW ONLY LATEST VERSION") + " (BETA)", RecalboxConf::Instance().GetShowOnlyLatestVersion(), (int)Components::ShowOnlyLatestVersion, this, _(MENUMESSAGE_GAMELISTOPTION_ONLY_LASt_VERSION));


    mFavoritesOnly = AddSwitch(_("SHOW ONLY FAVORITES"), RecalboxConf::Instance().GetFavoritesOnly(), (int)Components::FavoritesOnly, this, _(MENUMESSAGE_GAMELISTOPTION_FAVORITES_ONLY_MSG));


    AddSwitch(_("SHOW HIDDEN GAMES "), RecalboxConf::Instance().GetShowHidden(), (int)Components::ShowHidden, this, _(MENUMESSAGE_GAMELISTOPTION_SHOW_HIDDEN_MSG));

  AddSwitch(_("HIDE ADULT GAMES"), RecalboxConf::Instance().GetFilterAdultGames(), (int)Components::Adult, this, _(MENUMESSAGE_GAMELISTOPTION_HIDE_ADULT_MSG));

  AddSwitch(_("HIDE PREINSTALLED GAMES"), RecalboxConf::Instance().GetGlobalHidePreinstalled(), (int)Components::HidePreinstalled, this, _(MENUMESSAGE_GAME_HIDE_PREINSTALLED));

  AddSwitch(_("HIDE NO GAMES"), RecalboxConf::Instance().GetHideNoGames(), (int)Components::NoGames, this, _(MENUMESSAGE_GAME_HIDE_PREINSTALLED));

}

GuiMenuGameFilters::~GuiMenuGameFilters()
{
  if(!ViewController::Instance().CheckFilters())
   ManageSystems();
}

void GuiMenuGameFilters::SwitchComponentChanged(int id, bool status)
{
  switch((Components)id)
  {
    case Components::ShowOnlyLatestVersion:
      RecalboxConf::Instance().SetShowOnlyLatestVersion(status).Save();
      ManageSystems();
      break;
    case Components::FavoritesOnly:
      RecalboxConf::Instance().SetFavoritesOnly(status).Save();
      ManageSystems();
      break;
    case Components::ShowHidden:
      RecalboxConf::Instance().SetShowHidden(status).Save();
      ManageSystems();
      break;
    case Components::Adult:
      RecalboxConf::Instance().SetFilterAdultGames(status).Save();
      ManageSystems();
      break;
    case Components::HidePreinstalled:
      if(status)
      {
        RecalboxConf::Instance().SetGlobalHidePreinstalled(status).Save();
        ManageSystems();
        break;
      }
      else
      {
        RecalboxConf::Instance().SetGlobalHidePreinstalled(status).Save();
        ManageSystems();
        break;
      }
    case Components::NoGames:
      RecalboxConf::Instance().SetHideNoGames(status).Save();
      ManageSystems();
      break;
  }
}

void GuiMenuGameFilters::ManageSystems()
{
  SystemData* systemData = ViewController::Instance().getState().getSystem();
  ViewController::Instance().getGameListView(systemData)->refreshList();

  ViewController::Instance().setAllInvalidGamesList(nullptr);
  ViewController::Instance().getSystemListView().populate();
}
