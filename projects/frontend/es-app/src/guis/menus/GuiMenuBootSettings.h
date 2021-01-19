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

class GuiMenuBootSettings : public GuiMenuBase
{
  public:
    /*!
     * @brief Default constructor
     * @param window Global window
     */
    explicit GuiMenuBootSettings(WindowManager& window, SystemManager& systemManager);

  private:
    //! System manager
    SystemManager& mSystemManager;

    //! Run kodi at startup
    std::shared_ptr<SwitchComponent> mKodiAtStartup;
    //! Gamelist only
    std::shared_ptr<SwitchComponent> mGamelistOnly;
    //! Selected system
    std::shared_ptr<OptionListComponent<std::string>> mSelectedSystem;
    //! Start on gamelist
    std::shared_ptr<SwitchComponent> mStartOnGamelist;
    //! Hide system view
    std::shared_ptr<SwitchComponent> mHideSystemView;

    //! Get system list
    std::vector<ListEntry<std::string>> GetSystemEntries();

    //! Set & save Kodi at startup
    static void SetKodiAtStartup(bool on);
    //! Set & save Gamelist only
    static void SetGamelistOnly(bool on);
    //! Set & save Start on gamelist
    static void SetStartOnGamelist(bool on);
    //! Set & save Hide system view
    static void SetHideSystemView(bool on);
    //! Set & save selected system
    static void SetSelectedSystem(const std::string& system);
};



