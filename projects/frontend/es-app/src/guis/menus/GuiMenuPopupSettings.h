//
// Created by bkg2k on 31/10/2020.
//
#pragma once

#include <guis/menus/GuiMenuBase.h>
#include <components/MenuComponent.h>
#include <components/SliderComponent.h>

class GuiMenuPopupSettings : public GuiMenuBase
                           , private ISliderComponent
{
  public:
    /*!
     * @brief Default constructor
     * @param window Global window
     */
    explicit GuiMenuPopupSettings(WindowManager& window);

  private:
    enum class Components
    {
      Help,
      Music,
      Netplay,
    };

    //! Help popup time
    std::shared_ptr<SliderComponent> mHelp;
    //! Music popup time
    std::shared_ptr<SliderComponent> mMusic;
    //! Netplay popup time
    std::shared_ptr<SliderComponent> mNetplay;

    /*
     * ISliderComponent implementation
     */

    void SliderMoved(int id, float value) override;
};

