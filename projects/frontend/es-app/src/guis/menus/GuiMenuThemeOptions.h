//
// Created by bkg2k on 31/10/2020.
//
#pragma once

#include <components/MenuComponent.h>
#include <components/OptionListComponent.h>
#include <guis/menus/GuiMenuBase.h>
#include <components/SwitchComponent.h>

class GuiMenuThemeOptions : public GuiMenuBase
{
  public:
    /*!
     * @brief Default constructor
     * @param window Global window
     */
    explicit GuiMenuThemeOptions(Window& window);

    //! Destructor
    ~GuiMenuThemeOptions() override;

  private:
    //! Changed flag
    bool mReloadRequired;

    //! Carousel
    std::shared_ptr<SwitchComponent> mCarousel;
    //! Transition
    std::shared_ptr<OptionListComponent<std::string>> mTransition;
    //! Teeme
    std::shared_ptr<OptionListComponent<std::string>> mTheme;

    /*!
     * @brief Save & set Carousel state
     * @param time new Carousel state
     */
    void SetCarousel(bool on);

    /*!
     * @brief Save & set Transition
     * @param time new Transition
     */
    void SetTransition(const std::string& transition);

    /*!
     * @brief Save & set Theme
     * @param time new Theme
     */
    void SetTheme(const std::string& theme);
};
