//
// Created by bkg2k on 03/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include <guis/menus/GuiMenuRetroAchievements.h>
#include <guis/GuiArcadeVirtualKeyboard.h>
#include <utils/locale/LocaleHelper.h>
#include <guis/MenuMessages.h>

GuiMenuRetroAchievements::GuiMenuRetroAchievements(WindowManager& window)
  : GuiMenuBase(window, _("RETROACHIEVEMENTS SETTINGS"))
  , mCurrentEdition(EditedText::None)
{
  // Retroachievement on/off
  mRetroAchievement = std::make_shared<SwitchComponent>(mWindow, RecalboxConf::Instance().GetRetroAchievementOnOff());
  mRetroAchievement->setChangedCallback(SetOnOff);
  mMenu.addWithLabel(mRetroAchievement, _("RETROACHIEVEMENTS"), _(MENUMESSAGE_RA_ONOFF_HELP_MSG));

  // Retroachievement on/off
  mHardcore = std::make_shared<SwitchComponent>(mWindow, RecalboxConf::Instance().GetRetroAchievementHardcore());
  mRetroAchievement->setChangedCallback(SetHardcore);
  mMenu.addWithLabel(mHardcore, _("HARDCORE MODE"), _(MENUMESSAGE_RA_HARDCORE_HELP_MSG));

  // Login
  mLogin = std::make_shared<TextComponent>(mWindow, RecalboxConf::Instance().GetRetroAchievementLogin(),
                                           mTheme.menuText.font, mTheme.menuText.color);
  mMenu.addWithLabel(mLogin, _("USERNAME"), "", false, true,
                     std::bind(&GuiMenuRetroAchievements::EditLogin, this));

  // Password
  mPassword = std::make_shared<TextComponent>(mWindow, MaskedPassword(), mTheme.menuText.font, mTheme.menuText.color);
  mMenu.addWithLabel(mPassword, _("PASSWORD"), "", false, true,
                     std::bind(&GuiMenuRetroAchievements::EditPassword, this));
}

void GuiMenuRetroAchievements::EditLogin()
{
  mBackupedText = RecalboxConf::Instance().GetRetroAchievementLogin();
  mCurrentEdition = EditedText::Login;
  mWindow.pushGui(new GuiArcadeVirtualKeyboard(mWindow, _("USERNAME"), RecalboxConf::Instance().GetRetroAchievementLogin(), this));
}

void GuiMenuRetroAchievements::EditPassword()
{
  mBackupedText = RecalboxConf::Instance().GetRetroAchievementPassword();
  mCurrentEdition = EditedText::Password;
  mWindow.pushGui(new GuiArcadeVirtualKeyboard(mWindow, _("PASSWORD"), RecalboxConf::Instance().GetRetroAchievementPassword(), this));
}

void GuiMenuRetroAchievements::ArcadeVirtualKeyboardTextChange(GuiArcadeVirtualKeyboard& vk, const std::string& text)
{
  (void)vk;
  switch(mCurrentEdition)
  {
    case EditedText::Login:
    {
      // Update login
      RecalboxConf::Instance().SetRetroAchievementLogin(text);

      // Refresh login
      mLogin->setText(text);
      mMenu.onSizeChanged();
      break;
    }
    case EditedText::Password:
    {
      // Update password
      RecalboxConf::Instance().SetRetroAchievementPassword(text);

      // Refresh password
      mPassword->setText(MaskedPassword());
      mMenu.onSizeChanged();
      break;
    }
    case EditedText::None:
    default: break;
  }
}

void GuiMenuRetroAchievements::ArcadeVirtualKeyboardValidated(GuiArcadeVirtualKeyboard& vk, const std::string& text)
{
  ArcadeVirtualKeyboardTextChange(vk, text);
  RecalboxConf::Instance().Save();
  mCurrentEdition = EditedText::None;
}

void GuiMenuRetroAchievements::ArcadeVirtualKeyboardCanceled(GuiArcadeVirtualKeyboard& vk)
{
  ArcadeVirtualKeyboardTextChange(vk, mBackupedText);
  RecalboxConf::Instance().Save();
  mCurrentEdition = EditedText::None;
}

void GuiMenuRetroAchievements::SetOnOff(bool on)
{
  RecalboxConf::Instance().SetRetroAchievementOnOff(on).Save();
}

void GuiMenuRetroAchievements::SetHardcore(bool on)
{
  RecalboxConf::Instance().SetRetroAchievementHardcore(on).Save();
}
