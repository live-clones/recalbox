//
// Created by bkg2k on 31/10/2020.
//
#pragma once

#include <guis/menus/GuiMenuBase.h>
#include <components/MenuComponent.h>
#include <components/SliderComponent.h>
#include <components/OptionListComponent.h>

class GuiMenuScreensavers : public GuiMenuBase
                          , private ISliderComponent
                          , private IOptionListComponent<std::string>
                          , private IOptionListMultiComponent<std::string>
{
  public:
    /*!
     * @brief Default constructor
     * @param window Global window
     */
    explicit GuiMenuScreensavers(WindowManager& window, SystemManager& systemManager);

  private:
    enum class Components
    {
        Time,
        Type,
        SystemList,
    };

    //! System Manager reference
    SystemManager& mSystemManager;

    //! Time before screensaver starts
    std::shared_ptr<SliderComponent> mTime;
    //! Screensaver type
    std::shared_ptr<OptionListComponent<std::string>> mType;
    //! System list for demo mode
    std::shared_ptr<OptionListComponent<std::string>> mSystemList;

    //! Get Screensaver type List
    std::vector<ListEntry<std::string>> GetTypeEntries();

    //! Get System List
    std::vector<ListEntry<std::string>> GetSystemEntries();

    /*
     * ISliderComponent implementation
     */

    void SliderMoved(int id, float value) override;

    /*
     * IOptionListComponent<std::string> implementation
     */

    void OptionListComponentChanged(int id, int index, const std::string& value) override;

    /*
     * IOptionListMultiComponent<std::string> implementation
     */

    void OptionListMultiComponentChanged(int id, const std::vector<std::string>& value) override;
};

