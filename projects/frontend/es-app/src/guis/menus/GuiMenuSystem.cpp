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
  std::string version = Upgrade::CurrentVersion();
  mVersion = AddText(_("VERSION"), version, _(MENUMESSAGE_VERSION_HELP_MSG));

  // Share space
  mFreeSpace = AddText(_("DISK USAGE"), RecalboxSystem::getFreeSpaceInfo(),
                       RecalboxSystem::isFreeSpaceLimit() ? 0xFF0000FF : mTheme.menuText.color, _(MENUMESSAGE_DISK_USAGE_HELP_MSG));

  // Storage device
  mStorages = AddList<StorageDevices::Device>(_("STORAGE DEVICE"), (int)Components::Storage, this, GetStorageEntries(), _(MENUMESSAGE_STORAGE_DEVICE_HELP_MSG));

  // language choice
  mCulture = AddList<std::string>(_("LANGUAGE"), (int)Components::Culture, this, GetCultureEntries(), _(MENUMESSAGE_LANGUAGE_HELP_MSG));

  // Keyboard
  mKeyboard = AddList<std::string>(_("KEYBOARD"), (int)Components::Keyboard, this, GetKeyboardEntries(), _(MENUMESSAGE_KEYBOARD_HELP_MSG));
}

GuiMenuSystem::~GuiMenuSystem()
{
  // Storage changed?
  if (mStorages->getSelected().UUID != mOriginalStorage)
    Files::SaveFile(Path("/overlay/.configs/skipvideosplash"), mStorages->getSelected().UUID);

  // Reboot?
  if ((mStorages->getSelected().UUID != mOriginalStorage) ||
      (mCulture->getSelected() != mOriginalCulture) ||
      (mKeyboard->getSelected() != mOriginalKeyboard))
    RequestReboot();
}

std::vector<GuiMenuBase::ListEntry<StorageDevices::Device>> GuiMenuSystem::GetStorageEntries()
{
  std::vector<ListEntry<StorageDevices::Device>> list;

  mOriginalStorage = mStorageDevices.GetStorageDevice();
  for(const StorageDevices::Device& device : mStorageDevices.GetStorageDevices())
  {
    std::string display = device.DisplayName;
    if (device.Size != 0)
      display.append(" (Free ")
             .append(device.HumanFree())
             .append(1, '/')
             .append(device.HumanSize())
             .append(1, ')');
    list.push_back({ display, device, device.Current });
  }

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
    case Components::Storage: break;
    case Components::Culture: RecalboxConf::Instance().SetSystemLanguage(value).Save(); break;
    case Components::Keyboard: RecalboxConf::Instance().SetSystemKbLayout(value).Save(); break;
  }
}

void GuiMenuSystem::OptionListComponentChanged(int id, int index, const StorageDevices::Device& value)
{
  (void)index;
  switch((Components)id)
  {
    case Components::Storage: mStorageDevices.SetStorageDevice(value); break;
    case Components::Culture:
    case Components::Keyboard: break;
  }
}

