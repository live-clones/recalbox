//
// Created by bkg2k on 19/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include <guis/menus/GuiMenuSwitchKodiNetplay.h>
#include <guis/GuiNetPlay.h>
#include <utils/locale/LocaleHelper.h>
#include <systems/SystemManager.h>

GuiMenuSwitchKodiNetplay::GuiMenuSwitchKodiNetplay(WindowManager& window, SystemManager& systemManager)
  : GuiMenuBase(window, _("KODI/NETPLAY"))
  , mSystemManager(systemManager)
{
  // Start kodi
  AddSubMenu("\uF1c3 " + _("KODI MEDIA CENTER"), std::bind(StartKodi, this));

  // Start Netplay
  AddSubMenu("\uF1c4 " + _("NETPLAY LOBBY"), std::bind(StartNetplay, this));
}

void GuiMenuSwitchKodiNetplay::StartKodi(GuiMenuSwitchKodiNetplay* thiz)
{
  if (!RecalboxSystem::launchKodi(thiz->mWindow))
    LOG(LogWarning) << "Shutdown terminated with non-zero result!";
}

void GuiMenuSwitchKodiNetplay::StartNetplay(GuiMenuSwitchKodiNetplay* thiz)
{
  thiz->mWindow.pushGui(new GuiNetPlay(thiz->mWindow, thiz->mSystemManager));
  thiz->Close();
}
