//
// Created by bkg2k on 08/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <guis/menus/GuiMenuBase.h>

// Forward declaration
class SystemManager;
template<class T> class OptionListComponent;
class SwitchComponent;

class GuiMenuVirtualSystems : public GuiMenuBase
{
  public:
    /*!
     * @brief Default constructor
     * @param window Global window
     */
    explicit GuiMenuVirtualSystems(WindowManager& window, SystemManager& systemManager);

    //! Destructor
    ~GuiMenuVirtualSystems() override;

  private:
    //! System manager
    SystemManager& mSystemManager;

    //! All game switch component
    std::shared_ptr<SwitchComponent> mAllGames;
    //! Multiplayers switch component
    std::shared_ptr<SwitchComponent> mMultiplayers;
    //! Last played switch component
    std::shared_ptr<SwitchComponent> mLastPlayed;

    //! All games original value
    bool mAllGamesOriginalValues;
    //! Multiplayers original value
    bool mMultiplayersOriginalValues;
    //! Last Played original value
    bool mLastPlayedOriginalValues;

    //! Set All games value
    static void SetAllGames(bool on);
    //! Set Multiplayers value
    static void SetMultiplayers(bool on);
    //! Set Last Played value
    static void SetLastPlayed(bool on);
};



