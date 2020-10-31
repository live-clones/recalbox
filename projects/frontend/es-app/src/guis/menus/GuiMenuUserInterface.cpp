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

GuiMenuUserInterface::GuiMenuUserInterface(Window& window, SystemManager& systemManager)
  : GuiMenuBase(window, _("UI SETTINGS"))
  , mSystemManager(systemManager)
{
  // Brightness
  if (Board::BrightnessSupport())
  {
    mBrightness = std::make_shared<SliderComponent>(mWindow, 0.f, 8.f, 1.f, "");
    mBrightness->setSlider((float)RecalboxConf::Instance().GetBrightness());
    mBrightness->setSelectedChangedCallback(SetBrightness);
    mMenu.addWithLabel(mBrightness, _("BRIGHTNESS"));
  }

  // Screensavers
  AddSubMenu(_("SCREENSAVER"), [this] { mWindow.pushGui(new GuiMenuScreensavers(mWindow, mSystemManager)); }, _(MENUMESSAGE_UI_SCREENSAVER_HELP_MSG));

  // display clock
  mClock = std::make_shared<SwitchComponent>(mWindow, RecalboxConf::Instance().GetClock());
  mClock->setChangedCallback(SetClock);
  mMenu.addWithLabel(mClock, _("CLOCK IN MENU"), _(MENUMESSAGE_UI_CLOCK_HELP_MSG));

  // show help
  mHelp = std::make_shared<SwitchComponent>(mWindow, RecalboxConf::Instance().GetShowHelp());
  mHelp->setChangedCallback(SetHelp);
  mMenu.addWithLabel(mHelp, _("ON-SCREEN HELP"), _(MENUMESSAGE_UI_ONSCREENHELP_HELP_MSG));

  // Popup settings
  AddSubMenu(_("POPUP SETTINGS"), [this] { mWindow.pushGui(new GuiMenuPopupSettings(mWindow)); }, _(MENUMESSAGE_UI_POPUP_HELP_MSG));

  // quick system select (left/right in game list view)
  mQuickSelect = std::make_shared<SwitchComponent>(mWindow, RecalboxConf::Instance().GetQuickSystemSelect());
  mHelp->setChangedCallback(SetQuickSystemSelect);
  mMenu.addWithLabel(mQuickSelect, _("QUICK SYSTEM SELECT"), _(MENUMESSAGE_UI_QUICK_HELP_MSG));

  // Theme
  AddSubMenu(_("THEME"), [this] { mWindow.pushGui(new GuiMenuThemeOptions(mWindow)); }, _(MENUMESSAGE_UI_THEME_HELP_MSG));

  // Theme Options
  AddSubMenu(_("THEME CONFIGURATION"), [this] { mWindow.pushGui(new GuiMenuThemeConfiguration(mWindow, RecalboxConf::Instance().GetThemeFolder())); }, _(MENUMESSAGE_UI_THEME_CONFIGURATION_MSG));

  // Game List Update
  AddSubMenu(_("UPDATE GAMES LISTS"), [this] { ReloadGamelists(); }, _(MENUMESSAGE_UI_UPDATE_GAMELIST_HELP_MSG));
}

void GuiMenuUserInterface::SetBrightness(const float& brightness)
{
  if (RecalboxConf::Instance().GetBrightness() != (int)brightness)
  {
    Board::SetBrightness((int) brightness);
    RecalboxConf::Instance().SetBrightness((int) brightness);
    RecalboxConf::Instance().Save();
  }
}

void GuiMenuUserInterface::SetClock(bool on)
{
  RecalboxConf::Instance().SetClock(on);
  RecalboxConf::Instance().Save();
}

void GuiMenuUserInterface::SetHelp(bool on)
{
  RecalboxConf::Instance().SetShowHelp(on);
  RecalboxConf::Instance().Save();
}

void GuiMenuUserInterface::SetQuickSystemSelect(bool on)
{
  RecalboxConf::Instance().SetQuickSystemSelect(on);
  RecalboxConf::Instance().Save();
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
