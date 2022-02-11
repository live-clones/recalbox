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
                            , private IGuiMenuBase
                            , private ISwitchComponent
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
    enum class Components
    {
      AllGames,
      Multiplayers,
      LastPlayed,
      VirtualPerGenre,
      VirtualArcade,
      LightGun,
      Ports,
    };

    //! System manager
    SystemManager& mSystemManager;

    //! All game switch component
    std::shared_ptr<SwitchComponent> mAllGames;
    //! Multiplayers switch component
    std::shared_ptr<SwitchComponent> mMultiplayers;
    //! Last played switch component
    std::shared_ptr<SwitchComponent> mLastPlayed;
    //! Lightgun switch component
    std::shared_ptr<SwitchComponent> mLightGun;
    //! Ports switch component
    std::shared_ptr<SwitchComponent> mPorts;

    //! All games original value
    bool mAllGamesOriginalValues;
    //! Multiplayers original value
    bool mMultiplayersOriginalValues;
    //! Last Played original value
    bool mLastPlayedOriginalValues;
    //! Lightgun original value
    bool mLightGunOriginalValues;
    //! Ports original value
    bool mPortsOriginalValues;

    /*
     * IGuiMenuBase implementation
     */

    void SubMenuSelected(int id) override;

    /*
     * ISwitchComponent implementation
     */

    void SwitchComponentChanged(int id, bool status) override;
};



