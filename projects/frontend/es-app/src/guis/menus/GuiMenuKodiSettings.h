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
{
  public:
    /*!
     * @brief Default constructor
     * @param window Global window
     */
    explicit GuiMenuKodiSettings(WindowManager& window);

  private:
    //! Enable kodi
    std::shared_ptr<SwitchComponent> mKodiEnabled;
    //! Run kodi at startup
    std::shared_ptr<SwitchComponent> mKodiAtStartup;
    //! Run kodi with X key
    std::shared_ptr<SwitchComponent> mKodiX;

    //! Set & save Kodi enabled
    static void SetKodiEnabled(bool on);
    //! Set & save Kodi at startup
    static void SetKodiAtStartup(bool on);
    //! Set & save Kodi laucched with X
    static void SetKodiX(bool on);
};



