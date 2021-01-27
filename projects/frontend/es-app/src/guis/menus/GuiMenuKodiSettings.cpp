//
// Created by bkg2k on 04/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include "GuiMenuKodiSettings.h"
#include <components/SwitchComponent.h>
#include <WindowManager.h>
#include <utils/locale/LocaleHelper.h>
#include <guis/MenuMessages.h>

GuiMenuKodiSettings::GuiMenuKodiSettings(WindowManager& window)
  : GuiMenuBase(window, _("KODI SETTINGS"), nullptr)
{
  // Enable Kodi
  mKodiEnabled = AddSwitch(_("ENABLE KODI"), RecalboxConf::Instance().GetKodiEnabled(), (int)Components::Enabled, this, _(MENUMESSAGE_ADVANCED_KODI_ENABLE_HELP_MSG));

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
  }
}
