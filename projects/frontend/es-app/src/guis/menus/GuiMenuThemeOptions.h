//
// Created by bkg2k on 31/10/2020.
//
#pragma once

#include <components/MenuComponent.h>
#include <components/OptionListComponent.h>
#include <guis/menus/GuiMenuBase.h>
#include <components/SwitchComponent.h>

class GuiMenuThemeOptions : public GuiMenuBase
                          , private ISwitchComponent
                          , private IOptionListComponent<std::string>
{
  public:
    /*!
     * @brief Default constructor
     * @param window Global window
     */
    explicit GuiMenuThemeOptions(WindowManager& window);

    //! Destructor
    ~GuiMenuThemeOptions() override;

  private:
    enum class Components
    {
      Carousel,
      Transition,
      Theme,
    };

    //! Carousel
    std::shared_ptr<SwitchComponent> mCarousel;
    //! Transition
    std::shared_ptr<OptionListComponent<std::string>> mTransition;
    //! Teeme
    std::shared_ptr<OptionListComponent<std::string>> mTheme;

    //! Carousel original value
    bool mOriginalCaroussel;
    //! Transition original value
    std::string mOriginalTransition;
    //! Theme original value
    std::string mOriginalTheme;

    //! Get O/C List
    std::vector<ListEntry<std::string>> GetTransitionEntries();
    //! Get Theme List
    std::vector<ListEntry<std::string>> GetThemeEntries();

    /*
     * IOptionListComponent<std::string> implementation
     */

    void OptionListComponentChanged(int id, int index, const std::string& value) override;

    /*
     * ISwitchComponent implementation
     */

    void SwitchComponentChanged(int id, bool status) override;
};
