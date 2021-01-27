//
// Created by bkg2k on 08/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include "GuiMenuSystem.h"
#include <guis/MenuMessages.h>
#include <components/OptionListComponent.h>
#include <components/SwitchComponent.h>
#include <Upgrade.h>
#include <utils/Files.h>
#include <recalbox/RecalboxSystem.h>
#include <guis/GuiMsgBox.h>
#include <MainRunner.h>

GuiMenuSystem::GuiMenuSystem(WindowManager& window)
  : GuiMenuBase(window, _("SYSTEM SETTINGS"), nullptr)
{
  // Version
  std::string version = Strings::Trim(Files::LoadFile(Path(Upgrade::sLocalVersionFile)), " \t\r\n");
  mVersion = AddText(_("VERSION"), version, _(MENUMESSAGE_VERSION_HELP_MSG));

  // Share space
  mFreeSpace = AddText(_("DISK USAGE"), RecalboxSystem::getFreeSpaceInfo(),
                       RecalboxSystem::isFreeSpaceLimit() ? 0xFF0000FF : mTheme.menuText.color, _(MENUMESSAGE_DISK_USAGE_HELP_MSG));

  // Storage device
  mStorages = AddList(_("STORAGE DEVICE"), (int)Components::Storage, this, GetStorageEntries(), _(MENUMESSAGE_STORAGE_DEVICE_HELP_MSG));

  // language choice
  mCulture = AddList(_("LANGUAGE"), (int)Components::Culture, this, GetCultureEntries(), _(MENUMESSAGE_LANGUAGE_HELP_MSG));

  // Keyboard
  mKeyboard = AddList(_("KEYBOARD"), (int)Components::Keyboard, this, GetKeyboardEntries(), _(MENUMESSAGE_KEYBOARD_HELP_MSG));
}

GuiMenuSystem::~GuiMenuSystem()
{
  if ((mStorages->getSelected() != mOriginalStorage) ||
      (mCulture->getSelected() != mOriginalCulture) ||
      (mKeyboard->getSelected() != mOriginalKeyboard))
    mWindow.pushGui(new GuiMsgBox(mWindow, _("THE SYSTEM WILL NOW REBOOT"), _("OK"), [] { MainRunner::RequestQuit(MainRunner::ExitState::NormalReboot); }));
}

std::vector<GuiMenuBase::ListEntry<std::string>> GuiMenuSystem::GetStorageEntries()
{
  std::vector<ListEntry<std::string>> list;

  std::vector<std::string> availableStorage;
  if (Board::Instance().GetBoardType() == BoardType::OdroidAdvanceGo2) availableStorage.push_back("INTERNAL");
  else availableStorage = RecalboxSystem::getAvailableStorageDevices();
  mOriginalStorage = RecalboxSystem::getCurrentStorage();
  if (mOriginalStorage.empty()) mOriginalStorage = "INTERNAL";

  // Network starage available?
  IniFile bootConf(Path("/boot/recalbox-boot.conf"));
  if (bootConf.HasKeyStartingWith("sharenetwork_") || bootConf.AsString("sharedevice") == "NETWORK")
    list.push_back({ "NETWORK", "NETWORK", true });
  // Storage devices
  for (auto& it : availableStorage)
    if (Strings::StartsWith(it, "DEV"))
    {
      std::vector<std::string> tokens = Strings::Split(it, ' ');
      if (tokens.size() >= 3)
      {
        std::string displayableName = _("Device %n (%p)");
        Strings::ReplaceAllIn(displayableName, "%n", tokens[1]);
        Strings::ReplaceAllIn(displayableName, "%p", tokens[2]);
        std::string device(tokens[0]);
        device.append(1, ' ').append(tokens[1]);
        list.push_back({ displayableName, device, mOriginalStorage == device });
      }
    }
    else
      if (it != "RAM") { list.push_back({ it, it, mOriginalStorage == it }); }

  return list;
}

std::vector<GuiMenuBase::ListEntry<std::string>> GuiMenuSystem::GetCultureEntries()
{
  mOriginalCulture = RecalboxConf::Instance().GetSystemLanguage();
  return std::vector<ListEntry<std::string>>
  ({
    { "EUSKARA"       , "eu_ES" , mOriginalCulture == "eu_ES" },
    { "正體中文"       , "zh_TW" , mOriginalCulture == "zh_TW" },
    { "简体中文"       , "zh_CN" , mOriginalCulture == "zh_CN" },
    { "DEUTSCH"       , "de_DE" , mOriginalCulture == "de_DE" },
    { "ENGLISH"       , "en_US" , mOriginalCulture == "en_US" },
    { "ESPAÑOL"       , "es_ES" , mOriginalCulture == "es_ES" },
    { "FRANÇAIS"      , "fr_FR" , mOriginalCulture == "fr_FR" },
    { "ITALIANO"      , "it_IT" , mOriginalCulture == "it_IT" },
    { "PORTUGUES"     , "pt_BR" , mOriginalCulture == "pt_BR" },
    { "SVENSKA"       , "sv_SE" , mOriginalCulture == "sv_SE" },
    { "TÜRKÇE"        , "tr_TR" , mOriginalCulture == "tr_TR" },
    { "CATALÀ"        , "ca_ES" , mOriginalCulture == "ca_ES" },
    { "اللغة العربية" , "ar_YE" , mOriginalCulture == "ar_YE" },
    { "NEDERLANDS"    , "nl_NL" , mOriginalCulture == "nl_NL" },
    { "ελληνικά"      , "el_GR" , mOriginalCulture == "el_GR" },
    { "한국어"         , "ko_KR" , mOriginalCulture == "ko_KR" },
    { "NORSK"         , "nn_NO" , mOriginalCulture == "nn_NO" },
    { "BOKMAL"        , "nb_NO" , mOriginalCulture == "nb_NO" },
    { "POLSKI"        , "pl_PL" , mOriginalCulture == "pl_PL" },
    { "日本語"         , "ja_JP" , mOriginalCulture == "ja_JP" },
    { "Русский"       , "ru_RU" , mOriginalCulture == "ru_RU" },
    { "MAGYAR"        , "hu_HU" , mOriginalCulture == "hu_HU" },
    { "čeština"       , "cs_CZ" , mOriginalCulture == "cs_CZ" },
    { "latviešu"      , "lv_LV" , mOriginalCulture == "lv_LV" },
    { "Lëtzebuergesch", "lb_LU" , mOriginalCulture == "lb_LU" },
  });
}

std::vector<GuiMenuBase::ListEntry<std::string>> GuiMenuSystem::GetKeyboardEntries()
{
  mOriginalKeyboard = RecalboxConf::Instance().GetSystemKbLayout();
  return std::vector<ListEntry<std::string>>
  ({
    { "DE (German QWERTZ)"  , "de" , mOriginalKeyboard == "de" },
    { "DK (Denmark QWERTY)" , "dk" , mOriginalKeyboard == "dk" },
    { "ES (Spanish QWERTY)" , "es" , mOriginalKeyboard == "es" },
    { "FR (French AZERTY)"  , "fr" , mOriginalKeyboard == "fr" },
    { "IT (Italian QWERTY)" , "it" , mOriginalKeyboard == "it" },
    { "US (Standard QWERTY)", "us" , mOriginalKeyboard == "us" },
  });
}

void GuiMenuSystem::OptionListComponentChanged(int id, int index, const std::string& value)
{
  (void)index;
  switch((Components)id)
  {
    case Components::Storage: RecalboxSystem::setStorage(value); break;
    case Components::Culture: RecalboxConf::Instance().SetSystemLanguage(value).Save(); break;
    case Components::Keyboard: RecalboxConf::Instance().SetSystemKbLayout(value); break;
  }
}

