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
  : GuiMenuBase(window, _("SYSTEM SETTINGS"))
{
  // Version
  std::string version = Strings::Trim(Files::LoadFile(Path(Upgrade::sLocalVersionFile)), " \t\r\n");
  mVersion = std::make_shared<TextComponent>(mWindow, version, mTheme.menuText.font, mTheme.menuText.color);
  mMenu.addWithLabel(mVersion, _("VERSION"), _(MENUMESSAGE_VERSION_HELP_MSG));

  // Share space
  mFreeSpace = std::make_shared<TextComponent>(mWindow, RecalboxSystem::getFreeSpaceInfo(), mTheme.menuText.font,
                                               RecalboxSystem::isFreeSpaceLimit() ? 0xFF0000FF : mTheme.menuText.color);
  mMenu.addWithLabel(mFreeSpace, _("DISK USAGE"), _(MENUMESSAGE_DISK_USAGE_HELP_MSG));

  // Available storages
  std::vector<std::string> availableStorage;
  if (Board::Instance().GetBoardType() == BoardType::OdroidAdvanceGo2) availableStorage.push_back("INTERNAL");
  else availableStorage = RecalboxSystem::getAvailableStorageDevices();
  mOriginalStorage = RecalboxSystem::getCurrentStorage();
  if (mOriginalStorage.empty()) mOriginalStorage = "INTERNAL";

  // Storage device
  mStorages = std::make_shared<OptionListComponent<std::string> >(mWindow, _("STORAGE DEVICE"), false);
  mStorages->setChangedCallback([this] { SetStorage(mStorages->getSelected()); });
  // Network starage available?
  IniFile bootConf(Path("/boot/recalbox-boot.conf"));
  if (bootConf.HasKeyStartingWith("sharenetwork_") || bootConf.AsString("sharedevice") == "NETWORK")
    mStorages->add("NETWORK", "NETWORK", true);
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
        mStorages->add(displayableName, device, mOriginalStorage == device);
      }
    }
    else
      if (it != "RAM") { mStorages->add(it, it, mOriginalStorage == it); }
  mMenu.addWithLabel(mStorages, _("STORAGE DEVICE"), _(MENUMESSAGE_STORAGE_DEVICE_HELP_MSG));

  // language choice
  mCulture = std::make_shared<OptionListComponent<std::string> >(mWindow, _("LANGUAGE"), false);
  mCulture->setChangedCallback([this] { SetCulture(mCulture->getSelected()); });
  mOriginalCulture = RecalboxConf::Instance().GetSystemLanguage();
  if (mOriginalCulture.empty()) mOriginalCulture = "en_US";
  for(const Culture& culture : AvailableCultures())
    mCulture->add(culture.Displayable, culture.Identifier, culture.Identifier == mOriginalCulture);
  mMenu.addWithLabel(mCulture, _("LANGUAGE"), _(MENUMESSAGE_LANGUAGE_HELP_MSG));

  // Keyboard
  mKeyboard = std::make_shared<OptionListComponent<std::string> >(mWindow, _("KEYBOARD"), false);
  mKeyboard->setChangedCallback([this] { SetKeyboard(mKeyboard->getSelected()); });
  mOriginalKeyboard = RecalboxConf::Instance().AsString("system.kblayout");
  if (mOriginalKeyboard.empty()) mOriginalKeyboard = "us";
  for(const Keyboard& keyboard : AvailableKeyboards())
    mKeyboard->add(keyboard.Displayable, keyboard.Identifier, keyboard.Identifier == mOriginalKeyboard);
  mMenu.addWithLabel(mKeyboard, _("KEYBOARD"), _(MENUMESSAGE_KEYBOARD_HELP_MSG));
}

GuiMenuSystem::~GuiMenuSystem()
{
  if ((mStorages->getSelected() != mOriginalStorage) ||
      (mCulture->getSelected() != mOriginalCulture) ||
      (mKeyboard->getSelected() != mOriginalKeyboard))
    mWindow.pushGui(new GuiMsgBox(mWindow, _("THE SYSTEM WILL NOW REBOOT"), _("OK"), [] { MainRunner::RequestQuit(MainRunner::ExitState::NormalReboot); }));
}

GuiMenuSystem::CultureList GuiMenuSystem::AvailableCultures()
{
  return CultureList
  ({
    { "EUSKARA"       , "eu_ES" },
    { "正體中文"       , "zh_TW" },
    { "简体中文"       , "zh_CN" },
    { "DEUTSCH"       , "de_DE" },
    { "ENGLISH"       , "en_US" },
    { "ESPAÑOL"       , "es_ES" },
    { "FRANÇAIS"      , "fr_FR" },
    { "ITALIANO"      , "it_IT" },
    { "PORTUGUES"     , "pt_BR" },
    { "SVENSKA"       , "sv_SE" },
    { "TÜRKÇE"        , "tr_TR" },
    { "CATALÀ"        , "ca_ES" },
    { "اللغة العربية" , "ar_YE" },
    { "NEDERLANDS"    , "nl_NL" },
    { "ελληνικά"      , "el_GR" },
    { "한국어"         , "ko_KR" },
    { "NORSK"         , "nn_NO" },
    { "BOKMAL"        , "nb_NO" },
    { "POLSKI"        , "pl_PL" },
    { "日本語"         , "ja_JP" },
    { "Русский"       , "ru_RU" },
    { "MAGYAR"        , "hu_HU" },
    { "čeština"       , "cs_CZ" },
    { "latviešu"      , "lv_LV" },
    { "Lëtzebuergesch", "lb_LU" },
  });
}

GuiMenuSystem::KeyboardList GuiMenuSystem::AvailableKeyboards()
{
  return KeyboardList
  ({
    { "DE (German QWERTZ)", "de" },
    { "DK (Denmark QWERTY)", "dk" },
    { "ES (Spanish QWERTY)", "es" },
    { "FR (French AZERTY)", "fr" },
    { "IT (Italian QWERTY)", "it" },
    { "US (Standard QWERTY)", "us" },
  });
}

void GuiMenuSystem::SetStorage(const std::string& storage)
{
  RecalboxSystem::setStorage(storage);
}

void GuiMenuSystem::SetCulture(const std::string& culture)
{
  RecalboxConf::Instance().SetSystemLanguage(culture).Save();
}

void GuiMenuSystem::SetKeyboard(const std::string& layout)
{
  RecalboxConf::Instance().SetSystemKbLayout(layout);
}

