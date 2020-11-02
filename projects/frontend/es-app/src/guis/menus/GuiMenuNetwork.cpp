//
// Created by bkg2k on 26/10/2020.
//

#include <components/TextComponent.h>
#include <recalbox/RecalboxSystem.h>
#include <utils/locale/LocaleHelper.h>
#include <MenuThemeData.h>
#include <components/SwitchComponent.h>
#include "GuiMenuNetwork.h"
#include "guis/MenuMessages.h"
#include <guis/GuiArcadeVirtualKeyboard.h>

GuiMenuNetwork::GuiMenuNetwork(Window& window)
  : GuiMenuBase(window, _("NETWORK SETTINGS"))
  , mCurrentEdition(EditedText::None)
{
  auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();

  // Network status
  mStatus = std::make_shared<TextComponent>(mWindow, _("NOT CONNECTED"), menuTheme->menuText.font, menuTheme->menuText.color);
  mMenu.addWithLabel(mStatus, _("STATUS"), _(MENUMESSAGE_NETWORK_STATUS_HELP_MSG));

  // IP
  mIP = std::make_shared<TextComponent>(mWindow, RecalboxSystem::getIpAdress(), menuTheme->menuText.font, menuTheme->menuText.color);
  mMenu.addWithLabel(mIP, _("IP ADDRESS"), _(MENUMESSAGE_NETWORK_IP_HELP_MSG));

  // Hostname
  mHostname = std::make_shared<TextComponent>(mWindow, RecalboxConf::Instance().GetHostname(), menuTheme->menuText.font, menuTheme->menuText.color);
  mMenu.addWithLabel(mHostname, _("HOSTNAME"), _(MENUMESSAGE_NETWORK_HOST_HELP_MSG), false, true, std::bind(&GuiMenuNetwork::EditHostname, this));

  // WIFI ON/OFF
  mWifiOnOff = std::make_shared<SwitchComponent>(mWindow, RecalboxConf::Instance().GetWifiEnabled());
  mWifiOnOff->setChangedCallback(std::bind(&GuiMenuNetwork::OnWifiStateChanged, this));
  mMenu.addWithLabel(mWifiOnOff, _("ENABLE WIFI"), _(MENUMESSAGE_NETWORK_WIFI_HELP_MSG));

  // SSID
  mSSIDList = std::make_shared<OptionListComponent<std::string>>(mWindow, _("WIFI SSID"), false, FONT_SIZE_MEDIUM);
  mMenu.addWithLabel(mSSIDList, _("WIFI SSID"), _(MENUMESSAGE_NETWORK_KEY_HELP_MSG));

  // Password
  mWifiKey = std::make_shared<TextComponent>(mWindow, MaskedPassword(), menuTheme->menuText.font, menuTheme->menuText.color);
  mMenu.addWithLabel(mWifiKey, _("WIFI KEY"), _(MENUMESSAGE_NETWORK_KEY_HELP_MSG), false, true, std::bind(&GuiMenuNetwork::EditPassword, this));

  // Buttons
  mMenu.addButton(_("WPS CONNECTION"), _("AUTOMATIC WPS CONNECTION"), std::bind(&GuiMenuNetwork::TryWPS, this) );

  // Set current SSID
  Completed(NetworkOperation::ScanSSID, true);
}

void GuiMenuNetwork::EditHostname()
{
  mCurrentEdition = EditedText::Hostname;
  mWindow.pushGui(new GuiArcadeVirtualKeyboard(mWindow, _("HOSTNAME"), RecalboxConf::Instance().GetHostname(), this));
}

void GuiMenuNetwork::EditPassword()
{
  mCurrentEdition = EditedText::WifiKey;
  mWindow.pushGui(new GuiArcadeVirtualKeyboard(mWindow, _("WIFI KEY"), RecalboxConf::Instance().GetWifiKey(), this));
}

void GuiMenuNetwork::EditSSID()
{
  mCurrentEdition = EditedText::WifiSSID;
  mWindow.pushGui(new GuiArcadeVirtualKeyboard(mWindow, _("WIFI SSID"), RecalboxConf::Instance().GetWifiSSID(), this));
}

void GuiMenuNetwork::OnSSIDChanged()
{
  if (mSSIDList->getSelectedIndex() == 0) EditSSID();
  else
  {
    // Store new SSID
    SetWifiSSID(mSSIDList->getSelected(), true);

    // Reconnext using new parameters
    if (mWifiOnOff->getState())
      mWindow.pushGui((new GuiWaitLongExecution<NetworkOperation, bool>(mWindow, *this))->Execute(NetworkOperation::NewConnection,_("Connecting to WIFI...")));
  }
}

void GuiMenuNetwork::TryWPS()
{
  mWpsSSID.clear();
  mWpsPSK.clear();
  mWindow.pushGui((new GuiWaitLongExecution<NetworkOperation, bool>(mWindow, *this))->Execute(NetworkOperation::WPS,""));
}

void GuiMenuNetwork::OnWifiStateChanged()
{
  // Store new state
  SetWifiStatus(mWifiOnOff->getState(), true);

  // Connect or disconnect
  NetworkOperation operation = mWifiOnOff->getState() ? NetworkOperation::StartWIFI : NetworkOperation::StopWIFI;
  std::string text = mWifiOnOff->getState() ? _("Connecting to WIFI...") : _("Disconnecting from WIFI...");
  mWindow.pushGui((new GuiWaitLongExecution<NetworkOperation, bool>(mWindow, *this))->Execute(operation, text));
}

bool GuiMenuNetwork::Execute(GuiWaitLongExecution<NetworkOperation, bool>& from, const NetworkOperation& parameter)
{
  switch(parameter)
  {
    case NetworkOperation::StartWIFI:
    {
      LOG(LogDebug) << "[Network] Starting WIFI using current configuration";
      if (!RecalboxSystem::enableWifi("", "")) return false;
      from.SetText(_("Scanning WIFI networks..."));
      mScannedSSIDList = RecalboxSystem::getAvailableWiFiSSID(mWifiOnOff->getState());
      return true;
    }
    case NetworkOperation::StopWIFI: return RecalboxSystem::disableWifi();
    case NetworkOperation::NewConnection: return RecalboxSystem::enableWifi(RecalboxConf::Instance().GetWifiSSID(), RecalboxConf::Instance().GetWifiKey());
    case NetworkOperation::ScanSSID:
    {
      mScannedSSIDList = RecalboxSystem::getAvailableWiFiSSID(mWifiOnOff->getState());
      return true;
    }
    case NetworkOperation::WPS:
    {
      LOG(LogDebug) << "[WPS] Start WPS connection";
      from.SetText(_("Reseting WIFI configuration..."));
      LOG(LogDebug) << "[WPS] " << "Reseting WIFI configuration...";
      if (!RecalboxSystem::enableWifi("", "")) return false;
      from.SetText(_("Waiting for WPS configuration..."));
      LOG(LogDebug) << "[WPS] " << "Waiting for WPS configuration...";
      if (!RecalboxSystem::getWifiWps()) return false;
      sleep(2); // Give time to read the message :)
      std::string ip = _("Waiting for IP address... (%is)");
      for(int i = 30; --i >= 0; )
      {
        std::string finalText(Strings::Replace(ip, "%i", Strings::ToString(i)));
        from.SetText(finalText);
        LOG(LogDebug) << "[WPS] " << finalText;
        if (RecalboxSystem::hasIpAdress(true)) break;
        sleep(1);
      }
      if (!RecalboxSystem::hasIpAdress(true)) return false;
      from.SetText(_("Saving WIFI configuration"));
      LOG(LogDebug) << "[WPS] " << "Saving WIFI configuration";
      if (!RecalboxSystem::saveWifiWps()) return false;
      sleep(2); // Give time to read the message :)
      from.SetText(_("Fetching WIFI parameters"));
      LOG(LogDebug) << "[WPS] " << "Fetching WIFI parameters";
      if (!RecalboxSystem::getWifiConfiguration(mWpsSSID, mWpsPSK)) return false;
      sleep(2); // Give time to read the message :)

      LOG(LogInfo) << "[WPS] WPS Configuration OK!";
      return true;
    }
    default: break;
  }

  return false;
}

void GuiMenuNetwork::Completed(const NetworkOperation& parameter, const bool& result)
{
  switch(parameter)
  {
    case NetworkOperation::StartWIFI:
    {
      if (!result)
        mWindow.displayMessage(_("WIFI CONFIGURATION ERROR"));
      Completed(NetworkOperation::ScanSSID, result);
      break;
    }
    case NetworkOperation::StopWIFI: break;
    case NetworkOperation::NewConnection:
    {
      mWindow.displayMessage(result ? _("WIFI ENABLED") : _("WIFI CONFIGURATION ERROR"));
      break;
    }
    case NetworkOperation::ScanSSID:
    {
      // Kill notifier
      mSSIDList->setChangedCallback(nullptr);

      // Fill SSID list
      std::string currentSSID = RecalboxConf::Instance().GetWifiSSID();
      std::string currentSSIDView = currentSSID.empty() ? _("EDIT MANUALLY")
                                                        : currentSSID.append(" (").append(_("EDIT MANUALLY")).append(1, ')');
      bool found = false;
      for(const std::string& ssid : mScannedSSIDList)
        if (ssid == currentSSID) { found = true; break; }
      mSSIDList->clear();
      mSSIDList->add(currentSSIDView, currentSSID, !found);
      for(const std::string& ssid : mScannedSSIDList)
        mSSIDList->add(ssid, ssid, currentSSID == ssid);

      // Set back the change notifier
      mSSIDList->setChangedCallback(std::bind(&GuiMenuNetwork::OnSSIDChanged, this));
      break;
    }
    case NetworkOperation::WPS:
    {
      if (mWpsPSK.empty() || mWpsSSID.empty())
      {
        mWindow.displayMessage(_("NO WPS CONFIGURATION FOUND!"));
      }
      else
      {
        // Save new ssid/password
        SetWifiSSID(mWpsSSID, false);
        SetWifiPassword(mWpsPSK, true);
        // Refresh SSID List
        Completed(NetworkOperation::ScanSSID, true);
        // Refresh psk
        mWifiKey->setText(MaskedPassword());

        mWindow.displayMessage(_("WPS CONFIGURATION SUCCESSFUL!"));
      }
      break;
    }
    default: break;
  }

  // Update status & IP
  mStatus->setText(RecalboxSystem::ping() ? _( "CONNECTED") : _("NOT CONNECTED"));
  mIP->setText(RecalboxSystem::getIpAdress());
  mMenu.onSizeChanged();
}

void GuiMenuNetwork::ArcadeVirtualKeyboardTextChange(GuiArcadeVirtualKeyboard& vk, const std::string& text)
{
  (void)vk;
  switch(mCurrentEdition)
  {
    case EditedText::Hostname:
    {
      // Update hostname
      SetHostname(text, false);

      // Refresh hostname
      mHostname->setText(text);
      mMenu.onSizeChanged();
      break;
    }
    case EditedText::WifiSSID:
    {
      // Update SSID
      SetWifiSSID(text, false);

      // Refresh SSID list
      Completed(NetworkOperation::ScanSSID, true);
      break;
    }
    case EditedText::WifiKey:
    {
      // Update password
      SetWifiPassword(text, false);

      // Refresh password
      mWifiKey->setText(MaskedPassword());
      mMenu.onSizeChanged();
      break;
    }
    case EditedText::None:
    default: break;
  }
}

void GuiMenuNetwork::ArcadeVirtualKeyboardValidated(GuiArcadeVirtualKeyboard& vk, const std::string& text)
{
  ArcadeVirtualKeyboardTextChange(vk, text);
  RecalboxConf::Instance().Save();

  switch (mCurrentEdition)
  {
    case EditedText::Hostname:
    {
      mWindow.displayMessage("Hostname changes will not be effective until next reboot");
      break;
    }
    case EditedText::WifiSSID:
    case EditedText::WifiKey:
    {
      // Reconnext using new parameters
      if (mWifiOnOff->getState())
        mWindow.pushGui((new GuiWaitLongExecution<NetworkOperation, bool>(mWindow, *this))->Execute(NetworkOperation::NewConnection,_("Connecting to WIFI...")));

      break;
    }
    case EditedText::None:
    default: break;
  }
  mCurrentEdition = EditedText::None;
}

void GuiMenuNetwork::ArcadeVirtualKeyboardCanceled(GuiArcadeVirtualKeyboard& vk)
{
  ArcadeVirtualKeyboardTextChange(vk, mBackupedText);
  RecalboxConf::Instance().Save();
  mCurrentEdition = EditedText::None;
}
