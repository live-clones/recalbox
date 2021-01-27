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
                               , private IGuiMenuBase
{
  public:
    /*!
     * @brief Default constructor
     * @param window Global window
     */
    explicit GuiMenuSwitchKodiNetplay(WindowManager& window, SystemManager& systemManager);

  private:
    enum class Components
    {
      Kodi,
      Netplay,
    };

    //! System manager
    SystemManager& mSystemManager;

    /*
     * IGuiMenuBase implementation
     */

    void SubMenuSelected(int id) override;
};



