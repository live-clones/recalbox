//
// Created by bkg2k on 26/10/2020.
//
#pragma once

#include <guis/menus/GuiMenuBase.h>
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

class GuiMenuNetwork : public GuiMenuBase, private ILongExecution<NetworkOperation, bool>, private IGuiArcadeVirtualKeyboardInterface
{
  public:
    /*!
     * @brief Default constructor
     * @param window Global window
     */
    explicit GuiMenuNetwork(WindowManager& window);

  private:
    //! Text being currently edited
    enum class EditedText
    {
      None,
      WifiSSID,
      WifiKey,
      Hostname,
    };

    //! SSID list
    std::shared_ptr<OptionListComponent<std::string>> mSSIDList;
    //! Hostname
    std::shared_ptr<TextComponent> mHostname;
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
     * @brief Called when the user selects the Hostname line
     */
    void EditHostname();

    /*!
     * @brief Called when the user selects the password line
     */
    void EditPassword();

    /*!
     * @brief Called when the user selects the SSID line
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
    static std::string MaskedPassword() { return std::string(RecalboxConf::Instance().GetWifiKey().size(), '*'); }

    /*
     * Convenient storage access
     */

    /*!
     * @brief Set Hostname
     * @param status new hostname
     * @param save True to save configuration immediately
     */
    static void SetHostname(const std::string& hostname, bool save)
    {
      RecalboxConf::Instance().SetHostname(hostname);
      if (save) RecalboxConf::Instance().Save();
    }

    /*!
     * @brief Set WIFI status
     * @param status new wifi status
     * @param save True to save configuration immediately
     */
    static void SetWifiStatus(bool status, bool save)
    {
      RecalboxConf::Instance().SetWifiEnabled(status);
      if (save) RecalboxConf::Instance().Save();
    }

    /*!
     * @brief Set WIFI password
     * @param password new wifi password
     * @param save True to save configuration immediately
     */
    static void SetWifiPassword(const std::string& password, bool save)
    {
      RecalboxConf::Instance().SetWifiKey(password);
      if (save) RecalboxConf::Instance().Save();
    }

    /*!
     * @brief Set WIFI ssid
     * @param ssid new wifi ssid
     * @param save True to save configuration immediately
     */
    static void SetWifiSSID(const std::string& ssid, bool save)
    {
      RecalboxConf::Instance().SetWifiSSID(ssid);
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

