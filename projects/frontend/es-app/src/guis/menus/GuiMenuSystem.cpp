//
// Created by bkg2k on 08/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include "GuiMenuSystem.h"
#include <guis/menus/GuiMenuDiskUsage.h>
#include <guis/MenuMessages.h>
#include <components/OptionListComponent.h>
#include <components/SwitchComponent.h>
#include <systems/SystemManager.h>
#include <Upgrade.h>
#include <utils/Files.h>
#include <recalbox/RecalboxSystem.h>
#include <MainRunner.h>

GuiMenuSystem::GuiMenuSystem(WindowManager& window, SystemManager& systemManager)
  : GuiMenuBase(window, _("SYSTEM SETTINGS"), this)
  , mSystemManager(systemManager)
{
  // Version
  std::string version = Upgrade::CurrentVersion();
  std::string arch = "unknown";
  switch(Board::Instance().GetBoardType())
  {
    case BoardType::UndetectedYet:        arch = "undetected"; break;
    case BoardType::Unknown:              arch = "unknown"; break;
    case BoardType::Pi0:                  arch = "RPi zero"; break;
    case BoardType::Pi02:                 arch = "RPi zero 2"; break;
    case BoardType::Pi1:                  arch = "RPi 1"; break;
    case BoardType::Pi2:                  arch = "RPi 2"; break;
    case BoardType::Pi3:                  arch = "RPi 3"; break;
    case BoardType::Pi3plus:              arch = "RPi 3B+"; break;
    case BoardType::Pi4:                  arch = "RPi 4"; break;
    case BoardType::Pi400:                arch = "RPi 400"; break;
    case BoardType::UnknownPi:            arch = "unknown RPi"; break;
    case BoardType::OdroidAdvanceGo:      arch = "Odroid GO Advance"; break;
    case BoardType::OdroidAdvanceGoSuper: arch = "Odroid GO Super"; break;
    case BoardType::PCx86:                arch = "x86"; break;
    case BoardType::PCx64:                arch = "x64"; break;
    case BoardType::RG353P:               arch = "RG353P"; break;
    case BoardType::RG353V:               arch = "RG353V"; break;
    case BoardType::RG353M:               arch = "RG353M"; break;
    case BoardType::RG503:                arch = "RG503"; break;
  }
  arch.append(1, ' ').append(sizeof(void*) == 4 ? "32bits" : "64bits");
  AddText(_("VERSION"), version.append(" (").append(arch).append(1,')'), _(MENUMESSAGE_VERSION_HELP_MSG));

  // Share space
  MountMonitor::DeviceMountReferences mounts = systemManager.GetMountMonitor().AllMountPoints();
  if (mounts.size() == 1)
  {
    mounts[0]->UpdateSize();
    AddText(_("DISK USAGE (FREE/TOTAL)"), mounts[0]->DisplayableFreeSpace(), RecalboxSystem::isFreeSpaceUnderLimit(mounts[0]->FreeSize()) ? 0xFF0000FF : mTheme.menuText.color, _(MENUMESSAGE_DISK_USAGE_HELP_MSG));
  }
  else
    AddSubMenu(_("DISK USAGE (FREE/TOTAL)"), (int)Components::DiskUsage);

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
      display.append(" (")
             .append(_("Free"))
             .append(" ")
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

    { "اللغة العربية" , "ar_YE" , mOriginalCulture == "ar_YE" },
    { "CATALÀ"        , "ca_ES" , mOriginalCulture == "ca_ES" },
    { "čeština"       , "cs_CZ" , mOriginalCulture == "cs_CZ" },
    { "DEUTSCH"       , "de_DE" , mOriginalCulture == "de_DE" },
    { "DEUTSCH (CH)"  , "de_CH" , mOriginalCulture == "de_CH" },
    { "ελληνικά"      , "el_GR" , mOriginalCulture == "el_GR" },
    { "ENGLISH"       , "en_US" , mOriginalCulture == "en_US" },
    { "ESPAÑOL"       , "es_ES" , mOriginalCulture == "es_ES" },
    { "EUSKARA"       , "eu_ES" , mOriginalCulture == "eu_ES" },
    { "PERSIAN"       , "fa_IR" , mOriginalCulture == "fa_IR" },
    { "FRANÇAIS"      , "fr_FR" , mOriginalCulture == "fr_FR" },
    { "GALICIAN"      , "gl_ES" , mOriginalCulture == "gl_ES" },
    { "MAGYAR"        , "hu_HU" , mOriginalCulture == "hu_HU" },
    { "ITALIANO"      , "it_IT" , mOriginalCulture == "it_IT" },
    { "日本語"         , "ja_JP" , mOriginalCulture == "ja_JP" },
    { "한국어"         , "ko_KR" , mOriginalCulture == "ko_KR" },
    { "Lëtzebuergesch", "lb_LU" , mOriginalCulture == "lb_LU" },
    { "latviešu"      , "lv_LV" , mOriginalCulture == "lv_LV" },
    { "BOKMAL"        , "nb_NO" , mOriginalCulture == "nb_NO" },
    { "NEDERLANDS"    , "nl_NL" , mOriginalCulture == "nl_NL" },
    { "NORSK"         , "nn_NO" , mOriginalCulture == "nn_NO" },
    { "POLSKI"        , "pl_PL" , mOriginalCulture == "pl_PL" },
    { "PORTUGUES"     , "pt_BR" , mOriginalCulture == "pt_BR" },
    { "ROMANIAN"      , "ro_RO" , mOriginalCulture == "ro_RO" },
    { "Русский"       , "ru_RU" , mOriginalCulture == "ru_RU" },
    { "SLOVAK"        , "sk_SK" , mOriginalCulture == "sk_SK" },
    { "SVENSKA"       , "sv_SE" , mOriginalCulture == "sv_SE" },
    { "TÜRKÇE"        , "tr_TR" , mOriginalCulture == "tr_TR" },
    { "UKRAINIAN"     , "uk_UA" , mOriginalCulture == "uk_UA" },
    { "简体中文"       , "zh_CN" , mOriginalCulture == "zh_CN" },
    { "正體中文"       , "zh_TW" , mOriginalCulture == "zh_TW" },
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
    case Components::DiskUsage:
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
    case Components::DiskUsage:
    case Components::Culture:
    case Components::Keyboard: break;
  }
}

void GuiMenuSystem::SubMenuSelected(int id)
{
  if ((Components)id == Components::DiskUsage)
    mWindow.pushGui(new GuiMenuDiskUsage(mWindow, mSystemManager.GetMountMonitor()));
}

