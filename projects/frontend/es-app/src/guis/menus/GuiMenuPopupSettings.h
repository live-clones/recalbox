//
// Created by bkg2k on 31/10/2020.
//
#pragma once

#include <guis/menus/GuiMenuBase.h>
#include <components/MenuComponent.h>
#include <components/SliderComponent.h>

class GuiMenuPopupSettings : public GuiMenuBase
{
  public:
    /*!
     * @brief Default constructor
     * @param window Global window
     */
    explicit GuiMenuPopupSettings(Window& window);

  private:
    //! Help popup time
    std::shared_ptr<SliderComponent> mHelp;
    //! Music popup time
    std::shared_ptr<SliderComponent> mMusic;
    //! Netplay popup time
    std::shared_ptr<SliderComponent> mNetplay;

    /*!
     * @brief Save & set help popup time
     * @param time new popup time
     */
    static void SetHelpPopupTime(float time);

    /*!
     * @brief Set & save Music popup time
     * @param time new popup time
     */
    static void SetMusicPopupTime(float time);

    /*!
     * @brief Set & save Netplay popup time
     * @param time new popup time
     */
    static void SetNetplayPopupTime(float time);
};

