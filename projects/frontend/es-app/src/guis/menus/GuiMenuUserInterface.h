//
// Created by bkg2k on 31/10/2020.
//
#pragma once

#include <components/MenuComponent.h>
#include <guis/menus/GuiMenuBase.h>
#include <components/SwitchComponent.h>
#include <components/SliderComponent.h>
#include <systems/SystemManager.h>

class GuiMenuUserInterface : public GuiMenuBase
                           , private IGuiMenuBase
                           , private ISliderComponent
                           , private ISwitchComponent
{
  public:
    /*!
     * @brief Default constructor
     * @param window Global window
     */
    explicit GuiMenuUserInterface(WindowManager& window, SystemManager& systemManager);

    /*!
     * @brief Called once per frame. Override to implement your own drawings.
     * Call your base::Update() to ensure animation and childrens are updated properly
     * @param deltaTime Elapsed time from the previous frame, in millisecond
     */
    void Update(int deltaTime) override;

  private:
    enum class Components
    {
      Brightness,
      ScreenSaver,
      Clock,
      SwapValidateAndCancel,
      Help,
      Popups,
      QuickSelect,
      Theme,
      ThemeConfig,
      UpdateGamelist,
    };

    //! System Manager
    SystemManager& mSystemManager;

    //! Brightness
    std::shared_ptr<SliderComponent> mBrightness;
    //! Show Clock
    std::shared_ptr<SwitchComponent> mClock;
    //! Swap validate cancel buttons
    std::shared_ptr<SwitchComponent> mSwapValidateAndCancel;
    //! Show Help
    std::shared_ptr<SwitchComponent> mHelp;
    //! Show Quick Help
    std::shared_ptr<SwitchComponent> mQuickSelect;

    /*!
     * @brief Reload gamelists
     */
    void ReloadGamelists();

    /*
     * IGuiMenuBase implementation
     */

    void SubMenuSelected(int id) override;

    /*
     * ISliderComponent implementation
     */

    void SliderMoved(int id, float value) override;

    /*
     * ISwitchComponent implementation
     */

    void SwitchComponentChanged(int id, bool status) override;
};

