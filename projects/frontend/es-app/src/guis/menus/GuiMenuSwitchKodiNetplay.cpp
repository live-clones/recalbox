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
#include <emulators/run/GameRunner.h>

GuiMenuSwitchKodiNetplay::GuiMenuSwitchKodiNetplay(WindowManager& window, SystemManager& systemManager)
  : GuiMenuBase(window, _("KODI/NETPLAY"), this)
  , mSystemManager(systemManager)
{
  // Start kodi
  AddSubMenu("\uF1c3 " + _("KODI MEDIA CENTER"), (int)Components::Kodi);

  // Start Netplay
  AddSubMenu("\uF1c4 " + _("NETPLAY LOBBY"), (int)Components::Netplay);
}

void GuiMenuSwitchKodiNetplay::SubMenuSelected(int id)
{
  switch((Components)id)
  {
    case Components::Kodi:
    {
      if (!GameRunner::Instance().RunKodi())
      { LOG(LogWarning) << "[Kodi] Kodi terminated with non-zero result!"; }
      break;
    }
    case Components::Netplay:
    {
      mWindow.pushGui(new GuiNetPlay(mWindow, mSystemManager));
      Close();
      break;
    }
  }
}
