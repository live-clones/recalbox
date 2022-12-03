//
// Created by gugue_u on 29/03/2022.
//
//

#include "GuiMenuGameFilters.h"
#include <RecalboxConf.h>
#include <games/GameFilesUtils.h>
#include "views/ViewController.h"
#include "components/SwitchComponent.h"
#include "utils/locale/LocaleHelper.h"
#include "guis/MenuMessages.h"
#include "GuiMenu.h"

GuiMenuGameFilters::GuiMenuGameFilters(WindowManager& window)
  :	GuiMenuBase(window, _("GAME FILTERS"), nullptr)
{

  AddSwitch(_("SHOW ONLY LATEST VERSION") + " (BETA)", RecalboxConf::Instance().GetShowOnlyLatestVersion(), (int)Components::ShowOnlyLatestVersion, this, _(MENUMESSAGE_UI_ONLY_LAST_VERSION_MSG));

  AddSwitch(_("SHOW ONLY FAVORITES"), RecalboxConf::Instance().GetFavoritesOnly(), (int)Components::FavoritesOnly, this, _(MENUMESSAGE_UI_FAVORITES_ONLY_MSG));

  AddSwitch(_("SHOW HIDDEN GAMES"), RecalboxConf::Instance().GetShowHidden(), (int)Components::ShowHidden, this, _(MENUMESSAGE_UI_SHOW_HIDDEN_MSG));

  AddSwitch(_("HIDE ADULT GAMES"), RecalboxConf::Instance().GetFilterAdultGames(), (int)Components::Adult, this, _(MENUMESSAGE_UI_HIDE_ADULT_MSG));

  AddSwitch(_("HIDE PREINSTALLED GAMES"), RecalboxConf::Instance().GetGlobalHidePreinstalled(), (int)Components::HidePreinstalled, this, _(MENUMESSAGE_UI_HIDE_PREINSTALLED_MSG));

  AddSwitch(_("HIDE NO GAMES"), RecalboxConf::Instance().GetHideNoGames(), (int)Components::NoGames, this, _(MENUMESSAGE_UI_HIDE_NO_GAMES_MSG));

}

GuiMenuGameFilters::~GuiMenuGameFilters()
{
  if(!ViewController::Instance().CheckFilters())
    ViewController::Instance().ManageSystems();
}

void GuiMenuGameFilters::SwitchComponentChanged(int id, bool status)
{
  switch((Components)id)
  {
    case Components::ShowOnlyLatestVersion:
      RecalboxConf::Instance().SetShowOnlyLatestVersion(status).Save();
      ViewController::Instance().ManageSystems();
      break;
    case Components::FavoritesOnly:
      RecalboxConf::Instance().SetFavoritesOnly(status).Save();
      ViewController::Instance().ManageSystems();
      break;
    case Components::ShowHidden:
      RecalboxConf::Instance().SetShowHidden(status).Save();
      ViewController::Instance().ManageSystems();
      break;
    case Components::Adult:
      RecalboxConf::Instance().SetFilterAdultGames(status).Save();
      ViewController::Instance().ManageSystems();
      break;
    case Components::HidePreinstalled:
      RecalboxConf::Instance().SetGlobalHidePreinstalled(status).Save();
      ViewController::Instance().ManageSystems();
      break;
    case Components::NoGames:
      RecalboxConf::Instance().SetHideNoGames(status).Save();
      ViewController::Instance().ManageSystems();
      break;
  }
}
