//
// Created by bkg2k on 04/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include "GuiMenuBootSettings.h"
#include <utils/locale/LocaleHelper.h>
#include <guis/MenuMessages.h>
#include <recalbox/RecalboxSystem.h>
#include <systems/SystemManager.h>
#include <components/SwitchComponent.h>

GuiMenuBootSettings::GuiMenuBootSettings(WindowManager& window, SystemManager& systemManager)
  : GuiMenuBase(window, _("BOOT SETTINGS"))
  , mSystemManager(systemManager)
{
  // Kodi at startup
  mKodiAtStartup = AddSwitch(_("KODI AT START"), RecalboxConf::Instance().GetKodiAtStartup(), SetKodiAtStartup, _(MENUMESSAGE_ADVANCED_KODI_AT_START_HELP_MSG));

  // Gamelists only
  mGamelistOnly = AddSwitch(_("GAMELIST ONLY"), RecalboxConf::Instance().GetStartupGamelistOnly(), SetGamelistOnly, _(MENUMESSAGE_ADVANCED_GAMELISTONLY_HELP_MSG));

  // Selected System
  mSelectedSystem = AddList<std::string>(_("BOOT ON SYSTEM"), SetSelectedSystem, GetSystemEntries(), _(MENUMESSAGE_ADVANCED_BOOT_ON_SYSTEM_HELP_MSG));

  // Boot on gamelist
  mStartOnGamelist = AddSwitch(_("BOOT ON GAMELIST"), RecalboxConf::Instance().GetStartupStartOnGamelist(), SetStartOnGamelist, _(MENUMESSAGE_ADVANCED_BOOTGAMELIST_HELP_MSG));

  // Hide system view
  mHideSystemView = AddSwitch(_("HIDE SYSTEM VIEW"), RecalboxConf::Instance().GetStartupHideSystemView(), SetHideSystemView, _(MENUMESSAGE_ADVANCED_HIDESYSTEMVIEW_HELP_MSG));
}

std::vector<GuiMenuBase::ListEntry<std::string>> GuiMenuBootSettings::GetSystemEntries()
{
  std::vector<GuiMenuBase::ListEntry<std::string>> list;

  std::string selectedsystem = RecalboxConf::Instance().GetStartupSelectedSystem();
  if (selectedsystem.empty()) selectedsystem = SystemManager::sFavoriteSystemShortName;
  // For each activated system
  bool found = false;
  for (SystemData* system : mSystemManager.GetVisibleSystemList())
    if(system->getName() != SystemManager::sFavoriteSystemShortName)
    {
      bool match = (selectedsystem == system->getName());
      found |= match;
      list.push_back({ system->getFullName(), system->getName(), match });
    }
  list.push_back({ SystemManager::sFavoriteSystemShortName, SystemManager::sFavoriteSystemShortName, !found });

  return list;
}

void GuiMenuBootSettings::SetKodiAtStartup(bool on)
{
  RecalboxConf::Instance().SetKodiAtStartup(on).Save();
}

void GuiMenuBootSettings::SetGamelistOnly(bool on)
{
  RecalboxConf::Instance().SetStartupGamelistOnly(on).Save();
}

void GuiMenuBootSettings::SetStartOnGamelist(bool on)
{
  RecalboxConf::Instance().SetStartupStartOnGamelist(on).Save();
}

void GuiMenuBootSettings::SetHideSystemView(bool on)
{
  RecalboxConf::Instance().SetStartupHideSystemView(on).Save();
}

void GuiMenuBootSettings::SetSelectedSystem(const std::string& system)
{
  RecalboxConf::Instance().SetStartupSelectedSystem(system).Save();
}
