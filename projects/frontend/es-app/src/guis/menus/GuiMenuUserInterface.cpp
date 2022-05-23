//
// Created by bkg2k on 31/10/2020.
//

#include "GuiMenuUserInterface.h"
#include "GuiMenuThemeOptions.h"
#include "GuiMenuPopupSettings.h"
#include "GuiMenuScreensavers.h"
#include "GuiMenuThemeConfiguration.h"
#include "GuiMenuGameFilters.h"
#include <guis/MenuMessages.h>
#include <guis/GuiMsgBox.h>
#include <MainRunner.h>

GuiMenuUserInterface::GuiMenuUserInterface(WindowManager& window, SystemManager& systemManager)
  : GuiMenuBase(window, _("UI SETTINGS"), this)
  , mSystemManager(systemManager)
  , mOriginalSort(RecalboxConf::Instance().GetSystemSorting())
{
  // Screensavers
  AddSubMenu(_("SCREENSAVER"), (int)Components::ScreenSaver, _(MENUMESSAGE_UI_SCREENSAVER_HELP_MSG));

  // Brightness
  if (Board::Instance().HasBrightnessSupport())
      mBrightness = AddSlider(_("BRIGHTNESS"), 0.f, 8.f, 1.f, (float)RecalboxConf::Instance().GetBrightness(), "", (int)Components::Brightness, this);

  // Theme
  AddSubMenu(_("THEME"), (int)Components::Theme, _(MENUMESSAGE_UI_THEME_HELP_MSG));

  // Theme Options
  AddSubMenu(_("THEME CONFIGURATION"), (int)Components::ThemeConfig, _(MENUMESSAGE_UI_THEME_CONFIGURATION_MSG));

  // System sort
  mSort = AddList<SystemSorting>(_("SYSTEM SORTING"), (int)Components::SystemSort, this, GetSortingEntries(), _(MENUMESSAGE_ADVANCED_SORTINGOPTION_HELP_MSG));

  // quick system select (left/right in game list view)
  mQuickSelect = AddSwitch(_("QUICK SYSTEM SELECT"), RecalboxConf::Instance().GetQuickSystemSelect(), (int)Components::QuickSelect, this, _(MENUMESSAGE_UI_QUICK_HELP_MSG));

  // show help
  mHelp = AddSwitch(_("ON-SCREEN HELP"), RecalboxConf::Instance().GetShowHelp(), (int)Components::Help, this, _(MENUMESSAGE_UI_ONSCREENHELP_HELP_MSG));

  // Swap validate and cancel buttons
  mSwapValidateAndCancel = AddSwitch(_("SWAP VALIDATE/CANCEL BUTTONS"), RecalboxConf::Instance().GetSwapValidateAndCancel(), (int)Components::SwapValidateAndCancel, this, _(MENUMESSAGE_UI_SWAP_VALIDATE_CANCEL_BUTTONS_HELP_MSG));

  // display clock
  mClock = AddSwitch(_("CLOCK IN MENU"), RecalboxConf::Instance().GetClock(), (int)Components::Clock, this, _(MENUMESSAGE_UI_CLOCK_HELP_MSG));

  // Popup settings
  AddSubMenu(_("POPUP SETTINGS"), (int)Components::Popups, _(MENUMESSAGE_UI_POPUP_HELP_MSG));

  // filters
  AddSubMenu(_("GAME FILTERS"),  (int)Components::Filters, _(MENUMESSAGE_UI_GAME_FILTERS_MSG));

  // Display filename
  AddSwitch(_("DISPLAY BY FILENAME"), RecalboxConf::Instance().GetDisplayByFileName(), (int)Components::DisplayByFileName, this, _(MENUMESSAGE_UI_FILE_NAME_MSG));


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
    case Components::Filters:
      mWindow.pushGui(new GuiMenuGameFilters(mWindow));
      break;
    case Components::Brightness:
    case Components::Clock:
    case Components::SwapValidateAndCancel:
    case Components::Help:
    case Components::SystemSort:
    case Components::DisplayByFileName:
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
  SystemData* systemData = ViewController::Instance().getState().getSystem();

  switch((Components)id)
  {
    case Components::Clock: RecalboxConf::Instance().SetClock(status).Save(); break;
    case Components::Help: RecalboxConf::Instance().SetShowHelp(status).Save(); break;
    case Components::QuickSelect: RecalboxConf::Instance().SetQuickSystemSelect(status).Save(); break;
    case Components::SwapValidateAndCancel:
    {
      RecalboxConf::Instance().SetSwapValidateAndCancel(status).Save();
      updateHelpPrompts();
      break;
    }
    case Components::DisplayByFileName:
      RecalboxConf::Instance().SetDisplayByFileName(status).Save();
      ViewController::Instance().getGameListView(systemData)->refreshList();
      ViewController::Instance().setAllInvalidGamesList(nullptr);
      break;
    case Components::Popups:
    case Components::Theme:
    case Components::ThemeConfig:
    case Components::UpdateGamelist:
    case Components::ScreenSaver:
    case Components::SystemSort:
    case Components::Filters:
    case Components::Brightness: break;
  }
}


void GuiMenuUserInterface::OptionListComponentChanged(int id, int index, const SystemSorting& value)
{
    (void)index;
    (void)id;
    RecalboxConf::Instance().SetSystemSorting(value).Save();

    mSystemManager.SystemSorting();
    ViewController::Instance().getSystemListView().Sort();
}

std::vector<GuiMenuBase::ListEntry<SystemSorting>> GuiMenuUserInterface::GetSortingEntries()
{
    mOriginalSort = RecalboxConf::Instance().GetSystemSorting();
    return std::vector<ListEntry<SystemSorting>>
    ({
        { _("DEFAULT")                                    , SystemSorting::Default                                    , mOriginalSort == SystemSorting::Default },
        { _("NAME")                                       , SystemSorting::Name                                       , mOriginalSort == SystemSorting::Name },
        { _("RELEASE DATE")                               , SystemSorting::ReleaseDate                                , mOriginalSort == SystemSorting::ReleaseDate },
        { _("TYPE, THEN NAME")                            , SystemSorting::SystemTypeThenName                         , mOriginalSort == SystemSorting::SystemTypeThenName },
        { _("TYPE, THEN RELEASE DATE")                    , SystemSorting::SystemTypeThenReleaseDate                  , mOriginalSort == SystemSorting::SystemTypeThenReleaseDate },
        { _("MANUFACTURER, THEN NAME")                    , SystemSorting::ManufacturerThenName                       , mOriginalSort == SystemSorting::ManufacturerThenName },
        { _("MANUFACTURER, THEN RELEASE DATE")            , SystemSorting::ManufacturerThenReleaseData                , mOriginalSort == SystemSorting::ManufacturerThenReleaseData },
        { _("TYPE, THEN MANUFACTURER, THEN NAME")         , SystemSorting::SystemTypeThenManufacturerThenName         , mOriginalSort == SystemSorting::SystemTypeThenManufacturerThenName },
        { _("TYPE, THEN MANUFACTURER, THEN RELEASE DATE") , SystemSorting::SystemTypeThenManufacturerThenReleasdeDate , mOriginalSort == SystemSorting::SystemTypeThenManufacturerThenReleasdeDate },
    });
}
