//
// Created by bkg2k on 07/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <guis/menus/GuiMenuBase.h>
#include <guis/IGuiArcadeVirtualKeyboardInterface.h>

// Forward declaration
class SystemManager;
class SystemData;
template<class T> class OptionListComponent;
class SwitchComponent;

class GuiMenuNetplay : public GuiMenuBase, private IGuiArcadeVirtualKeyboardInterface
{
  public:
    /*!
     * @brief Default constructor
     * @param window Global window
     */
    explicit GuiMenuNetplay(WindowManager& window, SystemManager& systemManager);

  private:
    //! System manager
    SystemManager& mSystemManager;

    //! Text being currently edited
    enum class EditedText
    {
        None,
        Login,
        Port,
    };

    //! Enabled
    std::shared_ptr<SwitchComponent> mEnabled;
    //! Login
    std::shared_ptr<TextComponent> mLogin;
    //! Port
    std::shared_ptr<TextComponent> mPort;
    //! Mitm
    std::shared_ptr<OptionListComponent<RecalboxConf::Relay>> mMitm;

    //! Backuped text
    std::string mBackupedText;
    //! Current edition
    EditedText mCurrentEdition;

    //! Set enabled on/off
    static void SetEnabled(bool on);
    //! Set MITM server
    static void SetMitm(RecalboxConf::Relay mitm);

    //! Start editing the Login
    void EditLogin();
    //! Start editing the port
    void EditPort();

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



