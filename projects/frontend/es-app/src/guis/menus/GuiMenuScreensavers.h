//
// Created by bkg2k on 31/10/2020.
//
#pragma once

#include <guis/menus/GuiMenuBase.h>
#include <components/MenuComponent.h>
#include <components/SliderComponent.h>
#include <components/OptionListComponent.h>

class GuiMenuScreensavers : public GuiMenuBase
{
  public:
    /*!
     * @brief Default constructor
     * @param window Global window
     */
    explicit GuiMenuScreensavers(WindowManager& window, SystemManager& systemManager);

  private:
    //! Time before screensaver starts
    std::shared_ptr<SliderComponent> mTime;
    //! Screensaver type
    std::shared_ptr<OptionListComponent<std::string>> mType;
    //! System list for demo mode
    std::shared_ptr<OptionListComponent<std::string>> mSystemList;

    /*!
     * @brief Save & set screensaver time
     * @param time new screensaver time
     */
    static void SetTime(float time);

    /*!
     * @brief Set & save screensaver type
     */
    void SetType();

    /*!
     * @brief Set & save screensaver system list
     */
    void SetSystemList();
};

