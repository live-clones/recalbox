//
// Created by bkg2k on 04/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <guis/menus/GuiMenuBase.h>

// Forward declaration
class SystemManager;
class SwitchComponent;
template<class T> class OptionListComponent;

class GuiMenuKodiSettings : public GuiMenuBase
                          , private ISwitchComponent
{
  public:
    /*!
     * @brief Default constructor
     * @param window Global window
     */
    explicit GuiMenuKodiSettings(WindowManager& window);

  private:
    enum class Components
    {
      Enabled,
      RunAtStartup,
      ButtonX,
    };

    //! Enable kodi
    std::shared_ptr<SwitchComponent> mKodiEnabled;
    //! Run kodi at startup
    std::shared_ptr<SwitchComponent> mKodiAtStartup;
    //! Run kodi with X key
    std::shared_ptr<SwitchComponent> mKodiX;

    /*
     * ISwitchComponent implementation
     */

    void SwitchComponentChanged(int id, bool status) override;
};



