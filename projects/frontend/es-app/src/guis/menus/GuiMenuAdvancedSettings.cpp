//
// Created by bkg2k on 11/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include "GuiMenuAdvancedSettings.h"
#include "GuiMenuBootSettings.h"
#include "GuiMenuVirtualSystems.h"
#include "GuiMenuSystemList.h"
#include "GuiMenuKodiSettings.h"
#include "GuiMenuCRT.h"
#include "GuiMenuResolutionSettings.h"
#include "ResolutionAdapter.h"
#include <guis/MenuMessages.h>
#include <utils/locale/LocaleHelper.h>
//#include <components/OptionListComponent.h>
#include <components/SwitchComponent.h>
#include <utils/Files.h>
#include <guis/GuiMsgBox.h>
#include <MainRunner.h>
#include <recalbox/RecalboxSystem.h>
#include <hardware/Case.h>

GuiMenuAdvancedSettings::GuiMenuAdvancedSettings(WindowManager& window, SystemManager& systemManager)
  : GuiMenuBase(window, _("ADVANCED SETTINGS"), this)
  , mSystemManager(systemManager)
  , mDefaultOverclock({ "none", _("NONE"), false, 0})
  , mLastHazardous(false)
  , mValidOverclock(false)
{
  #if defined(BETA) || defined(DEBUG)
  AddSwitch(_("DEBUG LOGS"), RecalboxConf::Instance().GetDebugLogs(), (int)Components::DebugLogs, this);
  #endif

  bool isCrt = Board::Instance().CrtBoard().IsCrtAdapterAttached();

  // Overclock choice
  mOverclock = AddList<Overclocking>(_("OVERCLOCK"), (int)Components::OverclockList, this, GetOverclockEntries(), _(MENUMESSAGE_ADVANCED_OVERCLOCK_HELP_MSG));

  // Boot
  AddSubMenu(_("BOOT SETTINGS"), (int)Components::BootSubMenu, _(MENUMESSAGE_ADVANCED_BOOT_HELP_MSG));

  // Virtual systems
  AddSubMenu(_("VIRTUAL SYSTEMS"), (int)Components::VirtualSubMenu, _(MENUMESSAGE_ADVANCED_VIRTUALSYSTEMS_HELP_MSG));

  // CRT
  if (Board::Instance().CanHaveCRTBoard())
    AddSubMenu(_("RECALBOX CRT"), (int)Components::CrtSubMenu, _(MENUMESSAGE_ADVANCED_CRT_HELP_MSG));

  // RESOLUTION
  if (mResolutionAdapter.Resolutions(true).size() > 1 &&
      !isCrt &&
      Board::Instance().GetBoardType() != BoardType::PCx64)
    AddSubMenu(_("RESOLUTIONS"), (int)Components::ResolutionSubMenu, _(MENUMESSAGE_ADVANCED_RESOLUTION_HELP_MSG));

  // Custom config for systems
  AddSubMenu(_("ADVANCED EMULATOR CONFIGURATION"), (int)Components::AdvancedSubMenu, _(MENUMESSAGE_ADVANCED_EMULATOR_ADVANCED_HELP_MSG));

  //Kodi
  #ifndef DEBUG
  if (RecalboxSystem::kodiExists())
    AddSubMenu(_("KODI SETTINGS"), (int)Components::KodiSubMenu, _(MENUMESSAGE_ADVANCED_KODI_HELP_MSG));
  #else
  AddSubMenu(_("KODI SETTINGS"), (int)Components::KodiSubMenu, _(MENUMESSAGE_ADVANCED_KODI_HELP_MSG));
  #endif

  // Cases
  if(!Case::SupportedManualCases().empty())
    AddList<std::string>(_("CASE MANAGEMENT"),  (int)Components::Cases, this, GetCasesEntries(), _(MENUMESSAGE_ADVANCED_CASES_HELP_MSG));

  // overscan
  if(!isCrt)
    AddSwitch(_("OVERSCAN"), RecalboxConf::Instance().GetOverscan(), (int)Components::Overscan, this, _(MENUMESSAGE_ADVANCED_OVERSCAN_HELP_MSG));

  // framerate
  AddSwitch(_("SHOW FRAMERATE"), RecalboxConf::Instance().GetGlobalShowFPS(), (int)Components::ShowFPS, this, _(MENUMESSAGE_ADVANCED_FRAMERATE_HELP_MSG));

  // Recalbox Manager
  AddSwitch(_("RECALBOX MANAGER"), RecalboxConf::Instance().GetSystemManagerEnabled(), (int)Components::Manager, this, _(MENUMESSAGE_ADVANCED_MANAGER_HELP_MSG));

  // Reset Factory
  AddSubMenu(_("RESET TO FACTORY SETTINGS"), (int)Components::FactoryReset, _(MENUMESSAGE_ADVANCED_FACTORY_RESET));
}

GuiMenuAdvancedSettings::~GuiMenuAdvancedSettings()
{
  if (mValidOverclock)
    if (mOriginalOverclock != mOverclock->getSelected().File)
    {
      if (mLastHazardous)
        mWindow.pushGui(
          new GuiMsgBox(mWindow, _("TURBO AND EXTREM OVERCLOCK PRESETS MAY CAUSE SYSTEM UNSTABILITIES, SO USE THEM AT YOUR OWN RISK.\nIF YOU CONTINUE, THE SYSTEM WILL REBOOT NOW."),
                        _("YES"), [] { MainRunner::RequestQuit(MainRunner::ExitState::NormalReboot); }, _("NO"), [this] { ResetOverclock(); }));
      else
        RequestReboot();
    }
}

std::vector<GuiMenuBase::ListEntry<Overclocking>> GuiMenuAdvancedSettings::GetOverclockEntries()
{
  std::vector<ListEntry<Overclocking>> list;

  // Add entries
  OverclockList oc = AvailableOverclocks();
  mOriginalOverclock = RecalboxConf::Instance().GetOverclocking();
  bool ocFound = false;
  for(const Overclocking& overclock : oc)
  {
    bool found = mOriginalOverclock == overclock.File;
    ocFound |= found;
    std::string label = std::string(overclock.Description).append(" (", 2).append(Strings::ToString(overclock.Frequency)).append(" Mhz)", 5).append(overclock.Hazardous ? " \u26a0" : "");
    list.push_back({ label, overclock, found });
  }
  std::sort(list.begin(), list.end(), [](const ListEntry<Overclocking>& a, const ListEntry<Overclocking>& b) { return a.mValue.Frequency < b.mValue.Frequency; });
  // Add none
  mValidOverclock = !list.empty();
  if (mValidOverclock)
    list.insert(list.begin(), { _("NONE"), mDefaultOverclock, !ocFound });
  if (!ocFound) mOriginalOverclock = "none";

  return list;
}


GuiMenuAdvancedSettings::OverclockList GuiMenuAdvancedSettings::AvailableOverclocks()
{
  OverclockList result;

  // Build final path to fetch overclocking configs from
  Path basePath(sOverclockBaseFolder);
  std::string boardFolder;
  switch(Board::Instance().GetBoardType())
  {
    case BoardType::Pi0: boardFolder = "rpi0"; break;
    case BoardType::Pi02: boardFolder = "rpi02"; break;
    case BoardType::Pi1: boardFolder = "rpi1"; break;
    case BoardType::Pi2: boardFolder = "rpi2"; break;
    case BoardType::Pi3: boardFolder = "rpi3"; break;
    case BoardType::Pi3plus: boardFolder = "rpi3plus"; break;
    case BoardType::Pi4: boardFolder = "rpi4"; break;
    case BoardType::Pi400: boardFolder = "rpi400"; break;
    case BoardType::UndetectedYet:
    case BoardType::Unknown:
    case BoardType::UnknownPi:
    case BoardType::OdroidAdvanceGo:
    case BoardType::OdroidAdvanceGoSuper:
    case BoardType::RG353P:
    case BoardType::RG353V:
    case BoardType::RG353M:
    case BoardType::RG503:
    case BoardType::PCx86:
    case BoardType::PCx64: break;
  }
  { LOG(LogDebug) << "[Overclock] Board: " << (int)Board::Instance().GetBoardType(); }
  if (boardFolder.empty()) return result;
  Path finalPath = basePath / boardFolder;
  { LOG(LogDebug) << "[Overclock] Final source folder: " <<finalPath.ToString(); }

  // Get O/C config list
  Path::PathList list = finalPath.GetDirectoryContent();

  // Load and get description
  for(const Path& path : list)
  {
    std::string trash;
    std::string desc;
    bool hazard = false;
    int freq = 0;
    // Extract lines - doesn't matter if the file does not load, it returns an empty string
    for(const std::string& line : Strings::Split(Files::LoadFile(path), '\n'))
    {
      if (Strings::StartsWith(line, LEGACY_STRING("# Description: ")))
        Strings::SplitAt(line, ':', trash, desc, true);
      else if (Strings::StartsWith(line, LEGACY_STRING("# Warning")))
        hazard = true;
      else if (Strings::StartsWith(line, LEGACY_STRING("# Frequency: ")))
      {
        std::string freqStr;
        Strings::SplitAt(line, ':', trash,freqStr, true);
        Strings::ToInt(freqStr, freq);
      }
    }
    // Record?
    if (!desc.empty() && freq != 0)
    {
      result.push_back({ path.ToString(), desc, hazard, freq });
      { LOG(LogInfo) << "[Overclock] File " << path.ToString() << " validated."; }
    }
    else { LOG(LogError) << "[Overclock] Invalid file " << path.ToString(); }
  }

  return result;
}

void GuiMenuAdvancedSettings::ResetOverclock()
{
  mOverclock->select(mDefaultOverclock);
}

void GuiMenuAdvancedSettings::OptionListComponentChanged(int id, int index, const Overclocking& value)
{
  (void)index;
  if ((Components)id == Components::OverclockList)
  {
    RecalboxConf::Instance().SetOverclocking(value.File).Save();

    if (RecalboxSystem::MakeBootReadWrite())
    {
      Files::SaveFile(Path(sOverclockFile), Files::LoadFile(Path(value.File)));
      RecalboxSystem::MakeBootReadOnly();
      mLastHazardous = value.Hazardous;
      { LOG(LogInfo) << "[Overclock] Overclocking set to " << value.Description; }
    }
    else { LOG(LogError) << "[Overclock] Cannot make boot read write"; }
  }
}

void GuiMenuAdvancedSettings::SwitchComponentChanged(int id, bool status)
{
  switch ((Components)id)
  {
    case Components::DebugLogs:
    {
      MainRunner::SetDebugLogs(status);
      RecalboxConf::Instance().SetDebugLogs(status).Save();
      break;
    }
    case Components::Overscan:
    {
      RecalboxConf::Instance().SetOverscan(status);
      RecalboxSystem::setOverscan(status);
      break;
    }
    case Components::ShowFPS: RecalboxConf::Instance().SetGlobalShowFPS(status).Save(); break;
    case Components::Manager: RecalboxConf::Instance().SetSystemManagerEnabled(status).Save(); break;
    case Components::OverclockList:
    case Components::BootSubMenu:
    case Components::VirtualSubMenu:
    case Components::AdvancedSubMenu:
    case Components::KodiSubMenu:
    case Components::Cases:
    case Components::SecuritySubMenu:
    case Components::FactoryReset:
    case Components::CrtSubMenu:
    case Components::ResolutionSubMenu:
    default: break;
  }
}

void GuiMenuAdvancedSettings::SubMenuSelected(int id)
{
  switch ((Components)id)
  {
    case Components::BootSubMenu: mWindow.pushGui(new GuiMenuBootSettings(mWindow, mSystemManager)); break;
    case Components::CrtSubMenu: mWindow.pushGui(new GuiMenuCRT(mWindow)); break;
    case Components::VirtualSubMenu: mWindow.pushGui(new GuiMenuVirtualSystems(mWindow, mSystemManager)); break;
    case Components::AdvancedSubMenu: mWindow.pushGui(new GuiMenuSystemList(mWindow, mSystemManager)); break;
    case Components::KodiSubMenu: mWindow.pushGui(new GuiMenuKodiSettings(mWindow)); break;
    case Components::ResolutionSubMenu: mWindow.pushGui(new GuiMenuResolutionSettings(mWindow, mSystemManager)); break;
    case Components::FactoryReset: ResetFactory(); break;
    case Components::OverclockList:
    case Components::Overscan:
    case Components::ShowFPS:
    case Components::SecuritySubMenu:
    case Components::Manager:
    case Components::DebugLogs:
    case Components::Cases:
    default: break;
  }
}

void GuiMenuAdvancedSettings::ResetFactory()
{
  mWindow.pushGui(new GuiMsgBox(mWindow, _("RESET TO FACTORY SETTINGS\n\nYOU'RE ABOUT TO RESET RECALBOX AND EMULATOR SETTINGS TO DEFAULT VALUES.\nALL YOUR DATA LIKE GAMES, SAVES, MUSIC, SCREENSHOTS AND SO ON, WILL BE KEPT.\n\nTHIS OPERATION CANNOT BE UNDONE!\nARE YOU SURE YOU WANT TO RESET ALL YOUR SETTINGS?"),
                                _("NO"), nullptr,
                                _("YES"), std::bind(GuiMenuAdvancedSettings::ResetFactoryReally, &mWindow)));
}

void GuiMenuAdvancedSettings::ResetFactoryReally(WindowManager* window)
{
  std::string text("\u26a0 %TITLE% \u26a0\n\n%TEXT%");
  Strings::ReplaceAllIn(text, "%TITLE%", _("WARNING!"));
  Strings::ReplaceAllIn(text, "%TEXT%", _("YOU'RE ONE CLICK AWAY FROM RESETTING YOUR RECALBOX TO FACTORY SETTINGS!\n\nARE YOU REALLY SURE YOU WANT TO DO THIS?"));
  window->pushGui(new GuiMsgBox(*window, text,
                              _("NO"), nullptr,
                              _("YES"), DoResetFactory));
}

void GuiMenuAdvancedSettings::DoResetFactory()
{
  std::vector<std::string> deleteMe
  ({
    "/recalbox/share/system",             // Recalbox & emulator configurations
    "/overlay/upper/*",                   // System overlay
    "/overlay/.configs/*",                // System configurations
    "/overlay/upper.old",                 // System overlay backup
    "/overlay/.config",                   // Old system configurations
    "/boot/recalbox-backup.conf",         // Recalbox configuration backup
    "/boot/crt",                          // CRT Configuration
  });

  // Make boot partition writable
  if (!RecalboxSystem::MakeBootReadWrite())
  { LOG(LogError) << "[ResetFactory] Error making boot r/w"; }

  // Delete all required folder/files
  for(const std::string& path : deleteMe)
    if (system(std::string("rm -rf ").append(path).data()) != 0)
    { LOG(LogError) << "[ResetFactory] Error removing folder " << path; }

  // Reset case to force detection again
  IniFile recalboxBoot(Path("/boot/recalbox-boot.conf"), false);
  recalboxBoot.SetString("case", "");
  recalboxBoot.Save();

  // Reset!
  if (system("shutdown -r now") != 0)
  { LOG(LogError) << "[ResetFactory] Error rebooting system"; }
}

std::vector<GuiMenuBase::ListEntry<std::string>> GuiMenuAdvancedSettings::GetCasesEntries()
{
  std::vector<ListEntry<std::string>> list;
  Case currentCase = Case::CurrentCase();
  if(currentCase.Automatic())
  {
    list.push_back({currentCase.DisplayName() + " (AUTO)", currentCase.ShortName(), true});
  }
  else {
    for(const Case& theCase : Case::SupportedManualCases())
      list.push_back({theCase.DisplayName(), theCase.ShortName(), theCase.Model() == currentCase.Model()});
  }

  return list;
}

void GuiMenuAdvancedSettings::OptionListComponentChanged(int id, int index, const std::string& value)
{
  (void)index;
  if ((Components)id == Components::Cases)
  {
    Case selectedCase = Case::FromShortName(value);
    Case currentCase = Case::CurrentCase();
    const auto install = [this, selectedCase] {
      bool installed = selectedCase.Install();
      { LOG(LogInfo) << "[Settings - Cases] Installed case " << selectedCase.DisplayName() << (installed ? " successfully" : " failed!"); }
      RequestReboot();
    };
    if(selectedCase.Model() != currentCase.Model())
    {
      const std::string installMessage = selectedCase.GetInstallMessage();
      if (installMessage != "")
        mWindow.pushGui(new GuiMsgBox(mWindow, installMessage, _("OK"), install));
      else
        install();
    }
  }
}
