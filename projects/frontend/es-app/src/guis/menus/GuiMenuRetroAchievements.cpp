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
  : GuiMenuBase(window, _("RETROACHIEVEMENTS SETTINGS"), nullptr)
{
  // Retroachievement on/off
  mRetroAchievement = AddSwitch(_("RETROACHIEVEMENTS"), RecalboxConf::Instance().GetRetroAchievementOnOff(), (int)Components::Enabled, this, _(MENUMESSAGE_RA_ONOFF_HELP_MSG));

  // Hardcore mode on/off
  mHardcore = AddSwitch(_("HARDCORE MODE"), RecalboxConf::Instance().GetRetroAchievementHardcore(), (int)Components::Hardcore, this, _(MENUMESSAGE_RA_HARDCORE_HELP_MSG));

  // Login
  mLogin = AddEditable(_("USERNAME"), RecalboxConf::Instance().GetRetroAchievementLogin(), (int)Components::Login, this, false);

  // Password
  mPassword = AddEditable(_("PASSWORD"), RecalboxConf::Instance().GetRetroAchievementPassword(), (int)Components::Password, this, true);
}

void GuiMenuRetroAchievements::EditableComponentTextChanged(int id, const std::string& text)
{
  if ((Components)id == Components::Login) RecalboxConf::Instance().SetRetroAchievementLogin(text).Save();
  else if ((Components)id == Components::Password) RecalboxConf::Instance().SetRetroAchievementPassword(text).Save();
}

void GuiMenuRetroAchievements::SwitchComponentChanged(int id, bool status)
{
  if ((Components)id == Components::Enabled) RecalboxConf::Instance().SetRetroAchievementOnOff(status).Save();
  else if ((Components)id == Components::Hardcore) RecalboxConf::Instance().SetRetroAchievementHardcore(status).Save();
}
