//
// Created by bkg2k on 31/10/2020.
//
#pragma once

#include <components/MenuComponent.h>
#include <components/OptionListComponent.h>
#include <guis/menus/GuiMenuBase.h>
#include <components/SwitchComponent.h>
#include <components/SliderComponent.h>
#include <systems/SystemManager.h>

class GuiMenuUserInterface : public GuiMenuBase
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
    SystemManager& mSystemManager;

    //! Brightness
    std::shared_ptr<SliderComponent> mBrightness;
    //! Show Clock
    std::shared_ptr<SwitchComponent> mClock;
    //! Show Help
    std::shared_ptr<SwitchComponent> mHelp;
    //! Show Quick Help
    std::shared_ptr<SwitchComponent> mQuickSelect;

    /*!
     * @brief Save & set brightness
     * @param time new brightness
     */
    static void SetBrightness(const float& brightness);

    /*!
     * @brief Save & set Clock on/off
     * @param time new Clock state
     */
    static void SetClock(bool on);

    /*!
     * @brief Save & set Show help on/off
     * @param time new Show help state
     */
    static void SetHelp(bool on);

    /*!
     * @brief Save & set Quick system select
     * @param time new Quick system select state
     */
    static void SetQuickSystemSelect(bool on);

    /*!
     * @brief Reload gamelists
     */
    void ReloadGamelists();
};

