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
  : GuiMenuBase(window, _("KODI SETTINGS"))
{
  // Enable Kodi
  mKodiEnabled = AddSwitch(_("ENABLE KODI"), RecalboxConf::Instance().GetKodiEnabled(), SetKodiEnabled, _(MENUMESSAGE_ADVANCED_KODI_ENABLE_HELP_MSG));

  // Run Kodi at startup
  mKodiAtStartup = AddSwitch(_("KODI AT START"), RecalboxConf::Instance().GetKodiAtStartup(), SetKodiAtStartup, _(MENUMESSAGE_ADVANCED_KODI_AT_START_HELP_MSG));

  // Run kodi using X
  mKodiX = AddSwitch(_("START KODI WITH X"), RecalboxConf::Instance().GetKodiXButton(), SetKodiX, _(MENUMESSAGE_ADVANCED_KODI_X_HELP_MSG));
}

void GuiMenuKodiSettings::SetKodiEnabled(bool on)
{
  RecalboxConf::Instance().SetKodiEnabled(on).Save();
}

void GuiMenuKodiSettings::SetKodiAtStartup(bool on)
{
  RecalboxConf::Instance().SetKodiAtStartup(on).Save();
}

void GuiMenuKodiSettings::SetKodiX(bool on)
{
  RecalboxConf::Instance().SetKodiXButton(on).Save();
}
