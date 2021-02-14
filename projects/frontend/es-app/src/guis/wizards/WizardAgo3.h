//
// Created by bkg2k on 13/02/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#pragma once

#include <utils/locale/LocaleHelper.h>
#include "WizardLanguage.h"

class WizardAgo3 : public WizardLanguage
{
  public:
    enum class Pages
    {
        Intro,
        SelectButton,
        StartButton,
        VolumeButtons,
        BrightnessButtons,
        L3R3,
        PowerSuspend,
        PowerOff,
        Jack,
        Final,
        Count,
    };

    explicit WizardAgo3(WindowManager& window)
    : WizardLanguage(window, _("WELCOME TO RECALBOX!"), int(Pages::Count))
    {
    }

  private:
    static constexpr const char* sImagePath           = "/recalbox/system/resources/wizard/gos";
    static constexpr const char* sIntroImageFile      = "intro.jpg";
    static constexpr const char* sSelectImageFile     = "select.jpg";
    static constexpr const char* sStartImageFile      = "start.jpg";
    static constexpr const char* sVolumeImageFile     = "volume.jpg";
    static constexpr const char* sBrightnessImageFile = "brightness.jpg";
    static constexpr const char* sL3R3                = "l3r3.jpg";
    static constexpr const char* sPowerImageFile      = "power.jpg";
    static constexpr const char* sJackImageFile       = "jack.jpg";
    static constexpr const char* sFinalImageFile      = "final.jpg";

    //! Called when a page image is required
    Path OnImageRequired(int page) override;

    //! Called when a page text is required
    std::string OnTextRequired(int page) override;

    //! Called when z key event is received
    Move OnKeyReceived(int page, const InputCompactEvent& event) override;
};



