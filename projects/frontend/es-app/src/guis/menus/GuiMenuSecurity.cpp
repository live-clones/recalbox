//
// Created by bkg2k on 11/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include <guis/menus/GuiMenuSecurity.h>
#include <components/SwitchComponent.h>
#include <utils/locale/LocaleHelper.h>
#include <guis/MenuMessages.h>
#include <recalbox/RecalboxSystem.h>
#include <MainRunner.h>

GuiMenuSecurity::GuiMenuSecurity(WindowManager& window)
  : GuiMenuBase(window, _("SECURITY"), nullptr)
  , mOriginalEnabled(false)
{
  mEnabled = AddSwitch(_("ENFORCE SECURITY"), mOriginalEnabled = RecalboxConf::Instance().GetSecurityEnabled(), (int)Components::Enabled, this, _(MENUMESSAGE_ADVANCED_ENFORCE_SECURITY_HELP_MSG));

  mPassword = AddText(_("ROOT PASSWORD"), RecalboxSystem::getRootPassword(), _(MENUMESSAGE_ADVANCED_ROOT_PWD_HELP_MSG));
}

GuiMenuSecurity::~GuiMenuSecurity()
{
  if (mOriginalEnabled != mEnabled->getState()) RequestReboot();
}

void GuiMenuSecurity::SwitchComponentChanged(int id, bool status)
{
  if ((Components)id == Components::Enabled) RecalboxConf::Instance().SetSecurityEnabled(status).Save();
}
