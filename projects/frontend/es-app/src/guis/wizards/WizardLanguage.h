//
// Created by bkg2k on 13/02/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#pragma once

#include "Wizard.h"

class WizardLanguage : public Wizard
{
  public:
    /*!
     * @brief Constructor
     * @param window
     * @param title
     * @param pagecount
     */
    explicit WizardLanguage(WindowManager& window, const std::string& title, int pagecount)
    : Wizard(window, title, pagecount)
    {
    }

    //! Called when rebuilding the help bar
    void OnHelpRequired(int page, Help& help) override;

    /*!
     * @brief Change current language
     * @param increment True to go forward in the language list, false to go backward
     */
    void ChangeLanguage(bool increment);
};



