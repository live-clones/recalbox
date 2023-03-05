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
                           , private IOptionListComponent<SystemSorting>
                           , private IOptionListComponent<std::string>
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
      SystemSort,
      UpdateGamelist,
      Filters,
      DisplayByFileName,
      ScreenRotation
      DisplayFileNameType,
      DisplayGameRegions
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
    //! Original Sorting type
    SystemSorting mOriginalSort;
    //! recalbox-boot.cfg init file
    IniFile mBootIniFile;
    //! System Sort
    std::shared_ptr<OptionListComponent<SystemSorting>> mSort;
    //! Get Sorting List
    std::vector<ListEntry<SystemSorting>> GetSortingEntries();
    //! Get Sorting List
    std::vector<ListEntry<std::string>> GetRotationEntries();
    /*!
     * Get game name display type List
     */
    std::vector<ListEntry<std::string>> GetDisplayTypes();

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

    /*
     * IOptionListComponent<SystemSorting> implementation
     */

    void OptionListComponentChanged(int id, int index, const SystemSorting& value) override;

    /*
     * IOptionListComponent<std::string> implementation
     */
    void OptionListComponentChanged(int id, int index, const std::string& value) override;
};

