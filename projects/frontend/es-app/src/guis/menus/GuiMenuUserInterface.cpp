//
// Created by bkg2k on 31/10/2020.
//

#include "GuiMenuUserInterface.h"
#include "GuiMenuThemeOptions.h"
#include "GuiMenuPopupSettings.h"
#include "GuiMenuScreensavers.h"
#include "GuiMenuThemeConfiguration.h"
#include <guis/MenuMessages.h>
#include <guis/GuiMsgBox.h>
#include <MainRunner.h>

GuiMenuUserInterface::GuiMenuUserInterface(WindowManager& window, SystemManager& systemManager)
  : GuiMenuBase(window, _("UI SETTINGS"), this)
  , mSystemManager(systemManager)
{
  // Brightness
  if (Board::Instance().HasBrightnessSupport())
    mBrightness = AddSlider(_("BRIGHTNESS"), 0.f, 8.f, 1.f, (float)RecalboxConf::Instance().GetBrightness(), "", (int)Components::Brightness, this);

  // Screensavers
  AddSubMenu(_("SCREENSAVER"), (int)Components::ScreenSaver, _(MENUMESSAGE_UI_SCREENSAVER_HELP_MSG));

  // display clock
  mClock = AddSwitch(_("CLOCK IN MENU"), RecalboxConf::Instance().GetClock(), (int)Components::Clock, this, _(MENUMESSAGE_UI_CLOCK_HELP_MSG));

  // show help
  mHelp = AddSwitch(_("ON-SCREEN HELP"), RecalboxConf::Instance().GetShowHelp(), (int)Components::Help, this, _(MENUMESSAGE_UI_ONSCREENHELP_HELP_MSG));

  // Popup settings
  AddSubMenu(_("POPUP SETTINGS"), (int)Components::Popups, _(MENUMESSAGE_UI_POPUP_HELP_MSG));

  // quick system select (left/right in game list view)
  mQuickSelect = AddSwitch(_("QUICK SYSTEM SELECT"), RecalboxConf::Instance().GetQuickSystemSelect(), (int)Components::QuickSelect, this, _(MENUMESSAGE_UI_QUICK_HELP_MSG));

  // Theme
  AddSubMenu(_("THEME"), (int)Components::Theme, _(MENUMESSAGE_UI_THEME_HELP_MSG));

  // Theme Options
  AddSubMenu(_("THEME CONFIGURATION"), (int)Components::ThemeConfig, _(MENUMESSAGE_UI_THEME_CONFIGURATION_MSG));

  // Game List Update
  AddSubMenu(_("UPDATE GAMES LISTS"), (int)Components::UpdateGamelist, _(MENUMESSAGE_UI_UPDATE_GAMELIST_HELP_MSG));
}

void GuiMenuUserInterface::ReloadGamelists()
{
  mWindow.pushGui(new GuiMsgBox(mWindow, _("REALLY UPDATE GAMES LISTS ?"), _("YES"), [] {
    MainRunner::RequestQuit(MainRunner::ExitState::Relaunch, true);
  }, _("NO"), nullptr));
}

void GuiMenuUserInterface::Update(int deltaTime)
{
  GuiMenuBase::Update(deltaTime);

  // Synchronize volume bar if the volume is modified elsewhere
  if (mBrightness)
  {
    int realBrightness = RecalboxConf::Instance().GetBrightness();
    if (realBrightness != (int) mBrightness->getSlider())
      mBrightness->setSlider((float) realBrightness);
  }
}

void GuiMenuUserInterface::SubMenuSelected(int id)
{
  switch((Components)id)
  {
    case Components::ScreenSaver: mWindow.pushGui(new GuiMenuScreensavers(mWindow, mSystemManager)); break;
    case Components::Popups: mWindow.pushGui(new GuiMenuPopupSettings(mWindow)); break;
    case Components::Theme: mWindow.pushGui(new GuiMenuThemeOptions(mWindow)); break;
    case Components::ThemeConfig: mWindow.pushGui(new GuiMenuThemeConfiguration(mWindow, RecalboxConf::Instance().GetThemeFolder())); break;
    case Components::UpdateGamelist: ReloadGamelists(); break;
    case Components::Brightness:
    case Components::Clock:
    case Components::Help:
    case Components::QuickSelect: break;
  }
}

void GuiMenuUserInterface::SliderMoved(int id, float value)
{
  if ((Components)id == Components::Brightness)
    if (RecalboxConf::Instance().GetBrightness() != (int)value)
    {
      Board::Instance().SetBrightness((int) value);
      RecalboxConf::Instance().SetBrightness((int) value).Save();
    }
}

void GuiMenuUserInterface::SwitchComponentChanged(int id, bool status)
{
  switch((Components)id)
  {
    case Components::Clock: RecalboxConf::Instance().SetClock(status).Save(); break;
    case Components::Help: RecalboxConf::Instance().SetShowHelp(status).Save(); break;
    case Components::QuickSelect: RecalboxConf::Instance().SetQuickSystemSelect(status).Save(); break;
    case Components::Popups:
    case Components::Theme:
    case Components::ThemeConfig:
    case Components::UpdateGamelist:
    case Components::ScreenSaver:
    case Components::Brightness: break;
  }
}
