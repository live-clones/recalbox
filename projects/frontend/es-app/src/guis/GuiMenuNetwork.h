//
// Created by bkg2k on 26/10/2020.
//
#pragma once

#include <guis/Gui.h>
#include <components/MenuComponent.h>
#include <components/OptionListComponent.h>
#include <guis/GuiWaitLongExecution.h>
#include <guis/IGuiArcadeVirtualKeyboardInterface.h>

enum class NetworkOperation
{
  StartWIFI,
  StopWIFI,
  NewConnection,
  ScanSSID,
  WPS,
};

class GuiMenuNetwork : public Gui, private ILongExecution<NetworkOperation, bool>, private IGuiArcadeVirtualKeyboardInterface
{
  public:
    /*!
     * @brief Default constructor
     * @param window Global window
     */
    explicit GuiMenuNetwork(Window& window);

    /*!
     * @brief Proces input
     * @param event Inpuit event
     * @return True if the input has been processed
     */
    bool ProcessInput(const InputCompactEvent& event) override;

    /*!
     * @brief Fill in help
     * @param help Help
     * @return true of the help system has been files
     */
    bool getHelpPrompts(Help& help) override;

  private:
    enum class EditedText
    {
      None,
      WifiSSID,
      WifiKey,
    };

    //! Menu
    MenuComponent mMenu;
    //! SSID list
    std::shared_ptr<OptionListComponent<std::string>> mSSIDList;
    //! WIFI On/Off
    std::shared_ptr<SwitchComponent> mWifiOnOff;
    //! WIFI Key
    std::shared_ptr<TextComponent> mWifiKey;
    //! Status
    std::shared_ptr<TextComponent> mStatus;
    //! IP
    std::shared_ptr<TextComponent> mIP;

    //! Captured WPS SSID
    std::string mWpsSSID;
    //! Captured WPS PSK
    std::string mWpsPSK;

    //! Last SSID Scan
    Strings::Vector mScannedSSIDList;

    //! Currently edited text backup
    std::string mBackupedText;
    //! Current text edition
    EditedText mCurrentEdition;

    /*!
     * @brief Called when the user selectsthe  password line
     */
    void EditPassword();

    /*!
     * @brief Called when the user selectsthe  password line
     */
    void EditSSID();

    /*!
     * @brief Called when the SSID has changed
     */
    void OnSSIDChanged();

    /*!
     * @brief Called when the WIFI state is required to change
     */
    void OnWifiStateChanged();

    /*!
     * @brief Try WPS connection
     */
    void TryWPS();

    /*!
     * @brief Build a masked passwors string
     * @return Password string
     */
    static std::string MaskedPassword() { return std::string(GetWifiPassword().size(), '*'); }

    /*
     * Convenient storage access
     */

    //! Get the current WIFI status
    static bool GetWifiStatus() { return RecalboxConf::Instance().AsBool("wifi.enabled"); }

    /*!
     * @brief Set WIFI status
     * @param status new wifi status
     * @param save True to save configuration immediately
     */
    static void SetWifiStatus(bool status, bool save)
    {
      RecalboxConf::Instance().SetBool("wifi.enabled", status);
      if (save) RecalboxConf::Instance().Save();
    }

    //! Get the current WIFI password
    static std::string GetWifiPassword() { return RecalboxConf::Instance().AsString("wifi.key"); }

    /*!
     * @brief Set WIFI password
     * @param password new wifi password
     * @param save True to save configuration immediately
     */
    static void SetWifiPassword(const std::string& password, bool save)
    {
      RecalboxConf::Instance().SetString("wifi.key", password);
      if (save) RecalboxConf::Instance().Save();
    }

    //! Get the current WIFI SSID
    static std::string GetWifiSSID() { return RecalboxConf::Instance().AsString("wifi.ssid"); }

    /*!
     * @brief Set WIFI ssid
     * @param ssid new wifi ssid
     * @param save True to save configuration immediately
     */
    static void SetWifiSSID(const std::string& ssid, bool save)
    {
      RecalboxConf::Instance().SetString("wifi.ssid", ssid);
      if (save) RecalboxConf::Instance().Save();
    }

    /*
     * ILongExecution implementation
     */

    /*!
     * @brief Execture network operation
     * @param parameter Network operation required to execute
     * @return True if the operation was successful
     */
    bool Execute(GuiWaitLongExecution<NetworkOperation, bool>& from, const NetworkOperation& parameter) final;

    /*!
     * @brief Receive the status of network operations
     * @param parameter original input parameter
     * @param result Result state
     */
    void Completed(const NetworkOperation& parameter, const bool& result) final;

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

