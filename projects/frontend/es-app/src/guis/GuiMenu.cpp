#include <guis/GuiMenu.h>
#include <guis/MenuMessages.h>
#include <recalbox/RecalboxSystem.h>
#include <animations/LambdaAnimation.h>
#include <utils/locale/LocaleHelper.h>
#include <WindowManager.h>

#include <guis/menus/GuiMenuSystem.h>
#include <guis/menus/GuiMenuUpdates.h>
#include <guis/menus/GuiMenuGameSettings.h>
#include <guis/menus/GuiMenuPads.h>
#include <guis/menus/GuiMenuUserInterface.h>
#include <guis/menus/GuiMenuSound.h>
#include <guis/menus/GuiMenuNetwork.h>
#include <guis/menus/GuiMenuAdvancedSettings.h>
#include "GuiScraperSelect.h"
#include "GuiBiosScan.h"
#include "guis/menus/GuiMenuQuit.h"

GuiMenu::GuiMenu(WindowManager& window, SystemManager& systemManager)
  : GuiMenuBase(window, _("MAIN MENU"))
  , mSystemManager(systemManager)
{
  // Bartop mode?
  bool bartop = RecalboxConf::Instance().GetMenuType() == RecalboxConf::Menu::Bartop;

  // Kodi
  if (RecalboxSystem::kodiExists() && RecalboxConf::Instance().GetKodiEnabled())
    AddSubMenu(_("KODI MEDIA CENTER"), mTheme.menuIconSet.kodi, std::bind(GuiMenu::RunKodi, this), _(MENUMESSAGE_START_KODI_HELP_MSG));

  // System menu
  if (!bartop)
    AddSubMenu(_("SYSTEM SETTINGS"), mTheme.menuIconSet.system, std::bind(GuiMenu::System, this), _(MENUMESSAGE_SYSTEM_HELP_MSG));

  // Update menu
  if (!bartop)
    AddSubMenu(_("UPDATES"), mTheme.menuIconSet.updates, std::bind(GuiMenu::Update, this), _(MENUMESSAGE_UPDATE_HELP_MSG));

  // Games menu
  AddSubMenu(_("GAMES SETTINGS"), mTheme.menuIconSet.games, std::bind(GuiMenu::Games, this), _(MENUMESSAGE_GAME_SETTINGS_HELP_MSG));

  // Controllers menu
  if (!bartop)
    AddSubMenu(_("CONTROLLERS SETTINGS"), mTheme.menuIconSet.controllers, std::bind(GuiMenu::Controllers, this), _(MENUMESSAGE_CONTROLLER_HELP_MSG));

  // UI Settings menu
  if (!bartop)
    AddSubMenu(_("UI SETTINGS"), mTheme.menuIconSet.ui, std::bind(GuiMenu::UISettings, this), _(MENUMESSAGE_UI_HELP_MSG));

  // Sound menu
  AddSubMenu(_("SOUND SETTINGS"), mTheme.menuIconSet.sound, std::bind(GuiMenu::Sound, this), _(MENUMESSAGE_SOUND_HELP_MSG));

  // Network
  if (!bartop)
    AddSubMenu(_("NETWORK SETTINGS"), mTheme.menuIconSet.network, std::bind(GuiMenu::Network, this), _(MENUMESSAGE_NETWORK_HELP_MSG));

  // Scrapper
  if (!bartop)
    AddSubMenu(_("SCRAPER"), mTheme.menuIconSet.scraper, std::bind(GuiMenu::Scrapper, this), _(MENUMESSAGE_SCRAPER_HELP_MSG));

  // Advanced
  if (!bartop)
    AddSubMenu(_("ADVANCED SETTINGS"), mTheme.menuIconSet.advanced, std::bind(GuiMenu::Advanced, this), _(MENUMESSAGE_ADVANCED_HELP_MSG));

  // Bios
  if (!bartop)
    AddSubMenu(_("BIOS CHECKING"), mTheme.menuIconSet.games, std::bind(GuiMenu::Bios, this), _(MENUMESSAGE_BIOS_HELP_MSG));

  // License
  AddSubMenu(_("OPEN-SOURCE LICENSE"), mTheme.menuIconSet.license, std::bind(GuiMenu::License, this));

  // Quit
  AddSubMenu(_("QUIT"), mTheme.menuIconSet.quit, std::bind(GuiMenu::Quit, this));

  // Animation
  auto fadeFunc = [this](float t)
  {
    setOpacity(lerp<float>(0, 255, t));
    setPosition(getPosition().x(),
                lerp<float>(Renderer::Instance().DisplayHeightAsFloat(), (Renderer::Instance().DisplayHeightAsFloat() - mSize.y()) / 2, t));
  };

  setOpacity(0);
  setAnimation(new LambdaAnimation(fadeFunc, 200), 0);
}

void GuiMenu::RunKodi(GuiMenu* thiz)
{
  if (!RecalboxSystem::launchKodi(thiz->mWindow))
    LOG(LogWarning) << "[Kodi] Error running Kodi.";
}

void GuiMenu::System(GuiMenu* thiz)
{
  thiz->mWindow.pushGui(new GuiMenuSystem(thiz->mWindow));
}

void GuiMenu::Update(GuiMenu* thiz)
{
  thiz->mWindow.pushGui(new GuiMenuUpdates(thiz->mWindow));
}

void GuiMenu::Games(GuiMenu* thiz)
{
  thiz->mWindow.pushGui(new GuiMenuGameSettings(thiz->mWindow, thiz->mSystemManager));
}

void GuiMenu::Controllers(GuiMenu* thiz)
{
  thiz->mWindow.pushGui(new GuiMenuPads(thiz->mWindow));
}

void GuiMenu::UISettings(GuiMenu* thiz)
{
  thiz->mWindow.pushGui(new GuiMenuUserInterface(thiz->mWindow, thiz->mSystemManager));
}

void GuiMenu::Sound(GuiMenu* thiz)
{
  thiz->mWindow.pushGui(new GuiMenuSound(thiz->mWindow));
}

void GuiMenu::Network(GuiMenu* thiz)
{
  thiz->mWindow.pushGui(new GuiMenuNetwork(thiz->mWindow));
}

void GuiMenu::Scrapper(GuiMenu* thiz)
{
  thiz->mWindow.pushGui(new GuiScraperSelect(thiz->mWindow, thiz->mSystemManager));
}

void GuiMenu::Advanced(GuiMenu* thiz)
{
  thiz->mWindow.pushGui(new GuiMenuAdvancedSettings(thiz->mWindow, thiz->mSystemManager));
}

void GuiMenu::Bios(GuiMenu* thiz)
{
  thiz->mWindow.pushGui(new GuiBiosScan(thiz->mWindow, thiz->mSystemManager));
}

void GuiMenu::Quit(GuiMenu* thiz)
{
  thiz->mWindow.pushGui(new GuiMenuQuit(thiz->mWindow));
}

void GuiMenu::License(GuiMenu* thiz)
{
  thiz->mWindow.pushGui(
    new GuiMsgBoxScroll(thiz->mWindow, "RECALBOX",
                            Strings::ScrambleSymetric2(std::string(MenuMessages::LICENCE_MSG, MenuMessages::LICENCE_MSG_SIZE), __MESSAGE_DECORATOR),
                            _("OK"), nullptr, "", nullptr, "", nullptr, TextAlignment::Left));
}
