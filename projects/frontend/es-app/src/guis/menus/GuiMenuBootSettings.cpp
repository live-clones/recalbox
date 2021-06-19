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
  : GuiMenuBase(window, _("BOOT SETTINGS"), nullptr)
  , mSystemManager(systemManager)
{
  // Kodi at startup
  if (RecalboxSystem::kodiExists() && RecalboxConf::Instance().GetKodiEnabled())
    mKodiAtStartup = AddSwitch(_("KODI AT START"), RecalboxConf::Instance().GetKodiAtStartup(), (int)Components::KodiAtStartup, this, _(MENUMESSAGE_ADVANCED_KODI_AT_START_HELP_MSG));

  // Gamelists only
  mGamelistOnly = AddSwitch(_("GAMELIST ONLY"), RecalboxConf::Instance().GetStartupGamelistOnly(), (int)Components::GamelistOnly, this, _(MENUMESSAGE_ADVANCED_GAMELISTONLY_HELP_MSG));

  // Selected System
  mSelectedSystem = AddList<std::string>(_("BOOT ON SYSTEM"), (int)Components::SelectedSystem, this, GetSystemEntries(), _(MENUMESSAGE_ADVANCED_BOOT_ON_SYSTEM_HELP_MSG));

  // Boot on gamelist
  mStartOnGamelist = AddSwitch(_("BOOT ON GAMELIST"), RecalboxConf::Instance().GetStartupStartOnGamelist(), (int)Components::StartOnGamelist, this, _(MENUMESSAGE_ADVANCED_BOOTGAMELIST_HELP_MSG));

  // Hide system view
  mHideSystemView = AddSwitch(_("HIDE SYSTEM VIEW"), RecalboxConf::Instance().GetStartupHideSystemView(), (int)Components::HideSystemView, this, _(MENUMESSAGE_ADVANCED_HIDESYSTEMVIEW_HELP_MSG));
}

std::vector<GuiMenuBase::ListEntry<std::string>> GuiMenuBootSettings::GetSystemEntries()
{
  std::vector<GuiMenuBase::ListEntry<std::string>> list;

  std::string selectedsystem = RecalboxConf::Instance().GetStartupSelectedSystem();
  if (selectedsystem.empty()) selectedsystem = SystemManager::sFavoriteSystemShortName;
  // For each activated system
  bool found = false;
  for (SystemData* system : mSystemManager.GetVisibleSystemList())
    if(system->Name() != SystemManager::sFavoriteSystemShortName)
    {
      bool match = (selectedsystem == system->Name());
      found |= match;
      list.push_back({ system->FullName(), system->Name(), match });
    }
  list.push_back({ SystemManager::sFavoriteSystemShortName, SystemManager::sFavoriteSystemShortName, !found });

  return list;
}

void GuiMenuBootSettings::OptionListComponentChanged(int id, int index, const std::string& value)
{
  (void)index;
  if ((Components)id == Components::SelectedSystem)
  {
    RecalboxConf::Instance().SetStartupSelectedSystem(value).Save();
  }
}

void GuiMenuBootSettings::SwitchComponentChanged(int id, bool status)
{
  switch((Components)id)
  {
    case Components::KodiAtStartup: RecalboxConf::Instance().SetKodiAtStartup(status).Save(); break;
    case Components::GamelistOnly: RecalboxConf::Instance().SetStartupGamelistOnly(status).Save(); break;
    case Components::StartOnGamelist: RecalboxConf::Instance().SetStartupStartOnGamelist(status).Save(); break;
    case Components::HideSystemView: RecalboxConf::Instance().SetStartupHideSystemView(status).Save(); break;
    case Components::SelectedSystem:break;
  }
}
