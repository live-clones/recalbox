//
// Created by bkg2k on 11/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include <guis/menus/GuiMenuSecurity.h>
#include <components/SwitchComponent.h>
#include <components/TextComponent.h>
#include <utils/locale/LocaleHelper.h>
#include <guis/MenuMessages.h>
#include <recalbox/RecalboxSystem.h>
#include <guis/GuiMsgBox.h>
#include <MainRunner.h>

GuiMenuSecurity::GuiMenuSecurity(WindowManager& window)
  : GuiMenuBase(window, _("SECURITY"))
  , mOriginalEnabled(false)
{
  mEnabled = std::make_shared<SwitchComponent>(mWindow, mOriginalEnabled = RecalboxConf::Instance().GetSecurityEnabled());
  mEnabled->setChangedCallback(SetEnabled);
  mMenu.addWithLabel(mEnabled, _("ENFORCE SECURITY"), _(MENUMESSAGE_ADVANCED_ENFORCE_SECURITY_HELP_MSG));

  mPassword = std::make_shared<TextComponent>(mWindow, RecalboxSystem::getRootPassword(), mTheme.menuText.font, mTheme.menuText.color);
  mMenu.addWithLabel(mPassword, _("ROOT PASSWORD"), _(MENUMESSAGE_ADVANCED_ROOT_PWD_HELP_MSG));
}

GuiMenuSecurity::~GuiMenuSecurity()
{
  if (mOriginalEnabled != mEnabled->getState())
    mWindow.pushGui(new GuiMsgBox(mWindow, _("THE SYSTEM WILL NOW REBOOT"), _("OK"), Reboot));
}

void GuiMenuSecurity::SetEnabled(bool on)
{
  RecalboxConf::Instance().SetSecurityEnabled(on).Save();
}

void GuiMenuSecurity::Reboot()
{
  MainRunner::RequestQuit(MainRunner::ExitState::NormalReboot);
}
