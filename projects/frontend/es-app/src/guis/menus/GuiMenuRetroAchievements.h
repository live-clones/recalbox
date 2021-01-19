//
// Created by bkg2k on 03/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <guis/IGuiArcadeVirtualKeyboardInterface.h>
#include <guis/menus/GuiMenuBase.h>
#include <components/SwitchComponent.h>

class GuiMenuRetroAchievements : public GuiMenuBase, private IGuiArcadeVirtualKeyboardInterface
{
  public:
    /*!
     * @brief Default constructor
     * @param window Global window
     */
    explicit GuiMenuRetroAchievements(WindowManager& window);

  private:
    //! Text being currently edited
    enum class EditedText
    {
        None,
        Login,
        Password,
    };

    //! Retroarchievement on/off
    std::shared_ptr<SwitchComponent> mRetroAchievement;
    //! Hardcore mode
    std::shared_ptr<SwitchComponent> mHardcore;
    //! Login
    std::shared_ptr<TextComponent> mLogin;
    //! Password
    std::shared_ptr<TextComponent> mPassword;

    //! Current edited text before edition
    std::string mBackupedText;
    //! Current text being edited
    EditedText mCurrentEdition;

    /*!
     * @brief Called when the user selects the Hostname line
     */
    void EditLogin();

    /*!
     * @brief Called when the user selects the password line
     */
    void EditPassword();

    //! Set On/off value
    static void SetOnOff(bool on);

    //! Set hardcore value
    static void SetHardcore(bool on);

    /*!
     * @brief Build a masked passwors string
     * @return Password string
     */
    static std::string MaskedPassword() { return std::string(RecalboxConf::Instance().GetRetroAchievementPassword().size(), '*'); }

    /*
     * IGuiArcadeVirtualKeyboardInterface implementation
     */

    /*!
     * @brief Called when the edited text change.
     * Current text is available from the Text() method.
     */
    void ArcadeVirtualKeyboardTextChange(GuiArcadeVirtualKeyboard& vk, const std::string& text) final;

    /*!
     * @brief Called when the edited text is validated (Enter or Start)
     * Current text is available from the Text() method.
     */
    void ArcadeVirtualKeyboardValidated(GuiArcadeVirtualKeyboard& vk, const std::string& text) final;

    /*!
     * @brief Called when the edited text is cancelled.
     */
    void ArcadeVirtualKeyboardCanceled(GuiArcadeVirtualKeyboard& vk) final;
};



