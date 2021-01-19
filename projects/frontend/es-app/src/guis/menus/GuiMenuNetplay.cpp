//
// Created by bkg2k on 07/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include "GuiMenuNetplay.h"
#include <guis/MenuMessages.h>
#include <utils/locale/LocaleHelper.h>
#include <systems/SystemManager.h>
#include <components/OptionListComponent.h>
#include <components/SwitchComponent.h>
#include <guis/GuiNetPlayEditPasswords.h>
#include <guis/GuiArcadeVirtualKeyboard.h>
#include <guis/GuiHashStart.h>

GuiMenuNetplay::GuiMenuNetplay(WindowManager& window, SystemManager& systemManager)
  : GuiMenuBase(window, _("NETPLAY SETTINGS"))
  , mSystemManager(systemManager)
  , mCurrentEdition(EditedText::None)
{
  // Netplay Enabled
  mEnabled = std::make_shared<SwitchComponent>(mWindow, RecalboxConf::Instance().GetNetplayEnabled());
  mEnabled->setChangedCallback(SetEnabled);
  mMenu.addWithLabel(mEnabled, _("NETPLAY"), _(MENUMESSAGE_NP_ONOFF_HELP_MSG));

  // netplay username
  mLogin = std::make_shared<TextComponent>(mWindow, RecalboxConf::Instance().GetNetplayLogin(), mTheme.menuText.font, mTheme.menuText.color);
  mMenu.addWithLabel(mLogin, _("NICKNAME"), "", false, true, std::bind(&GuiMenuNetplay::EditLogin, this));

  // netplay port
  mPort = std::make_shared<TextComponent>(mWindow, Strings::ToString(RecalboxConf::Instance().GetNetplayPort()), mTheme.menuText.font, mTheme.menuText.color);
  mMenu.addWithLabel(mPort, _("PORT"), "", false, true, std::bind(&GuiMenuNetplay::EditPort, this));

  //mitm
  mMitm = std::make_shared<OptionListComponent<RecalboxConf::Relay> >(mWindow,_("NETPLAY MITM"), false);
  mMitm->setChangedCallback([this] { SetMitm(mMitm->getSelected()); });
  RecalboxConf::Relay currentMitm = RecalboxConf::Instance().GetNetplayRelay();
  mMitm->add(_("NONE"), RecalboxConf::Relay::None, currentMitm == RecalboxConf::Relay::None);
  mMitm->add(_("NEW YORK"), RecalboxConf::Relay::NewYork, currentMitm == RecalboxConf::Relay::NewYork);
  mMitm->add(_("MADRID"), RecalboxConf::Relay::Madrid, currentMitm == RecalboxConf::Relay::Madrid);
  mMenu.addWithLabel(mMitm, _("NETPLAY MITM"), _(MENUMESSAGE_NP_RELAY_HELP_MSG));

  // Password edition
  AddSubMenu(_("PREDEFINED PASSWORDS"), [this] { mWindow.pushGui(new GuiNetPlayEditPasswords(mWindow)); });
  // Hash roms
  AddSubMenu(_("HASH ROMS"), [this] { mWindow.pushGui(new GuiHashStart(mWindow, mSystemManager)); }, _(MENUMESSAGE_NP_HASH_HELP_MSG));
}

void GuiMenuNetplay::EditLogin()
{
  mBackupedText = RecalboxConf::Instance().GetNetplayLogin();
  mCurrentEdition = EditedText::Login;
  mWindow.pushGui(new GuiArcadeVirtualKeyboard(mWindow, _("NICKNAME"), mBackupedText, this));
}

void GuiMenuNetplay::EditPort()
{
  mBackupedText = Strings::ToString(RecalboxConf::Instance().GetNetplayPort());
  mCurrentEdition = EditedText::Port;
  mWindow.pushGui(new GuiArcadeVirtualKeyboard(mWindow, _("PORT"), mBackupedText, this));
}

void GuiMenuNetplay::SetEnabled(bool on)
{
  RecalboxConf::Instance().SetNetplayEnabled(on).Save();
}

void GuiMenuNetplay::SetMitm(RecalboxConf::Relay mitm)
{
  RecalboxConf::Instance().SetNetplayRelay(mitm).Save();
}

void GuiMenuNetplay::ArcadeVirtualKeyboardTextChange(GuiArcadeVirtualKeyboard& vk, const std::string& text)
{
  (void)vk;
  switch(mCurrentEdition)
  {
    case EditedText::Login:
    {
      // Update login
      RecalboxConf::Instance().SetNetplayLogin(text);

      // Refresh login
      mLogin->setText(text);
      mMenu.onSizeChanged();
      break;
    }
    case EditedText::Port:
    {
      // Update password
      int value = RecalboxConf::sNetplayDefaultPort;
      RecalboxConf::Instance().SetNetplayPort(Strings::ToInt(text, value) ? value : RecalboxConf::sNetplayDefaultPort);

      // Refresh password
      mPort->setText(Strings::ToString(value));
      mMenu.onSizeChanged();
      break;
    }
    case EditedText::None:
    default: break;
  }
}

void GuiMenuNetplay::ArcadeVirtualKeyboardValidated(GuiArcadeVirtualKeyboard& vk, const std::string& text)
{
  ArcadeVirtualKeyboardTextChange(vk, text);
  RecalboxConf::Instance().Save();
  mCurrentEdition = EditedText::None;
}

void GuiMenuNetplay::ArcadeVirtualKeyboardCanceled(GuiArcadeVirtualKeyboard& vk)
{
  ArcadeVirtualKeyboardTextChange(vk, mBackupedText);
  RecalboxConf::Instance().Save();
  mCurrentEdition = EditedText::None;
}