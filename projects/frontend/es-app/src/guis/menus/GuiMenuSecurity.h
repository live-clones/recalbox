//
// Created by bkg2k on 11/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <guis/menus/GuiMenuBase.h>

// Forward declaration
class SwitchComponent;
class TextComponent;

class GuiMenuSecurity : public GuiMenuBase
{
  public:
    /*!
     * @brief Default constructor
     * @param window Global window
     */
    explicit GuiMenuSecurity(WindowManager& window);

    //! Destructor
    ~GuiMenuSecurity() override;

  private:
    //! Security on/off
    std::shared_ptr<SwitchComponent> mEnabled;
    //! Read-only password
    std::shared_ptr<TextComponent> mPassword;

    //! Original enabled value
    bool mOriginalEnabled;

    //! Set Enabled on/off
    static void SetEnabled(bool on);

    //! Reboot method (must be static)
    static void Reboot();
};



