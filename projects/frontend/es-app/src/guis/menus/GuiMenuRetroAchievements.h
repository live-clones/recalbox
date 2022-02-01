//
// Created by bkg2k on 03/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <guis/menus/GuiMenuBase.h>
#include <components/SwitchComponent.h>

class GuiMenuRetroAchievements : public GuiMenuBase
                               , private IEditableComponent
                               , private ISwitchComponent
{
  public:
    /*!
     * @brief Default constructor
     * @param window Global window
     */
    explicit GuiMenuRetroAchievements(WindowManager& window);

  private:
    enum class Components
    {
        Enabled,
        Hardcore,
        Login,
        Password,
    };

    //! Retroarchievement on/off
    std::shared_ptr<SwitchComponent> mRetroAchievement;
    //! Hardcore mode
    std::shared_ptr<SwitchComponent> mHardcore;
    //! Login
    std::shared_ptr<EditableComponent> mLogin;
    //! Password
    std::shared_ptr<EditableComponent> mPassword;

    /*
     * IEditableComponent implementation
     */

    void EditableComponentTextChanged(int id, const std::string& text) override;

    /*
     * ISwitchComponent implementation
     */

    void SwitchComponentChanged(int id, bool status) override;
};



