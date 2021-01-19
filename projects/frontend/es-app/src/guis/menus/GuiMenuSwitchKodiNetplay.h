//
// Created by bkg2k on 19/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <guis/menus/GuiMenuBase.h>

// Forward declaration
class SystemManager;

class GuiMenuSwitchKodiNetplay : public GuiMenuBase
{
  public:
    /*!
     * @brief Default constructor
     * @param window Global window
     */
    explicit GuiMenuSwitchKodiNetplay(WindowManager& window, SystemManager& systemManager);

  private:
    //! System manager
    SystemManager& mSystemManager;

    //! Start Kodi
    static void StartKodi(GuiMenuSwitchKodiNetplay* thiz);
    //! Start Netplay
    static void StartNetplay(GuiMenuSwitchKodiNetplay* thiz);
};



