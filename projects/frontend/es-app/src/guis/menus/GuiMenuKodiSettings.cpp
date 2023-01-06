//
// Created by bkg2k on 04/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include "GuiMenuKodiSettings.h"
#include "ResolutionAdapter.h"
#include <components/SwitchComponent.h>
#include <WindowManager.h>
#include <utils/locale/LocaleHelper.h>
#include <guis/MenuMessages.h>

GuiMenuKodiSettings::GuiMenuKodiSettings(WindowManager& window)
  : GuiMenuBase(window, _("KODI SETTINGS"), nullptr)
{
  // Enable Kodi
  mKodiEnabled = AddSwitch(_("ENABLE KODI"), RecalboxConf::Instance().GetKodiEnabled(), (int)Components::Enabled, this, _(MENUMESSAGE_ADVANCED_KODI_ENABLE_HELP_MSG));

  // Select kodi video mode
  AddList<std::string>(_("KODI RESOLUTION"), (int)Components::KodiResolution, this, GetKodiResolutionsEntries(), _(MENUMESSAGE_ADVANCED_RESOLUTION_KODI_HELP_MSG));

  // Run Kodi at startup
  mKodiAtStartup = AddSwitch(_("KODI AT START"), RecalboxConf::Instance().GetKodiAtStartup(), (int)Components::RunAtStartup, this, _(MENUMESSAGE_ADVANCED_KODI_AT_START_HELP_MSG));

  // Run kodi using X
  mKodiX = AddSwitch(_("START KODI WITH X"), RecalboxConf::Instance().GetKodiXButton(), (int)Components::ButtonX, this, _(MENUMESSAGE_ADVANCED_KODI_X_HELP_MSG));
}

void GuiMenuKodiSettings::SwitchComponentChanged(int id, bool status)
{
  switch((Components)id)
  {
    case Components::Enabled: RecalboxConf::Instance().SetKodiEnabled(status).Save(); break;
    case Components::RunAtStartup: RecalboxConf::Instance().SetKodiAtStartup(status).Save(); break;
    case Components::ButtonX: RecalboxConf::Instance().SetKodiXButton(status).Save(); break;
    case Components::KodiResolution:
    default: break;
  }
}


std::vector<GuiMenuBase::ListEntry<std::string>> GuiMenuKodiSettings::GetKodiResolutionsEntries()
{
  std::vector<GuiMenuBase::ListEntry<std::string>> result;
  result.push_back({ _("NATIVE"), "default", RecalboxConf::Instance().GetKodiVideoMode() == "default" });
  result.push_back({ _("USE GLOBAL"), "", !RecalboxConf::Instance().IsDefinedKodiVideoMode() });
  for(const ResolutionAdapter::Resolution& resolution : mResolutionAdapter.Resolutions(false))
  {
    std::string reso = resolution.ToRawString();
    result.push_back({ resolution.ToString(), reso, reso == RecalboxConf::Instance().GetKodiVideoMode() });
  }
  return result;
}

void GuiMenuKodiSettings::OptionListComponentChanged(int id, int index, const std::string& value)
{
  (void)index;
  switch((Components)id)
  {
    case Components::KodiResolution:
    {
      if (value.empty()) RecalboxConf::Instance().DeleteKodiVideoMode().Save();
      else RecalboxConf::Instance().SetKodiVideoMode(value).Save();
      break;
    }
    case Components::Enabled:
    case Components::RunAtStartup:
    case Components::ButtonX:
    default: break;
  }
}