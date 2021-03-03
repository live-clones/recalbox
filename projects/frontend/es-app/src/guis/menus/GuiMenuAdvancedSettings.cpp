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
#include "GuiMenuSecurity.h"
#include <guis/MenuMessages.h>
#include <utils/locale/LocaleHelper.h>
#include <components/OptionListComponent.h>
#include <components/SwitchComponent.h>
#include <utils/Files.h>
#include <guis/GuiMsgBox.h>
#include <MainRunner.h>
#include <recalbox/RecalboxSystem.h>

GuiMenuAdvancedSettings::GuiMenuAdvancedSettings(WindowManager& window, SystemManager& systemManager)
  : GuiMenuBase(window, _("ADVANCED SETTINGS"), this)
  , mSystemManager(systemManager)
  , mDefaultOverclock({ _("NONE"), "none", false})
  , mLastHazardous(false)
  , mValidOverclock(false)
{
  // Overclock choice
  mOverclock = AddList<Overclocking>(_("OVERCLOCK"), (int)Components::OverclockList, this, GetOverclockEntries(), _(MENUMESSAGE_ADVANCED_OVERCLOCK_HELP_MSG));

  // Boot
  AddSubMenu(_("BOOT SETTINGS"), (int)Components::BootSubMenu, _(MENUMESSAGE_ADVANCED_BOOT_HELP_MSG));

  // Virtual systems
  AddSubMenu(_("VIRTUAL SYSTEMS"), (int)Components::VirtualSubMenu, _(MENUMESSAGE_ADVANCED_VIRTUALSYSTEMS_HELP_MSG));

  // Adult games
  mAdult = AddSwitch(_("HIDE ADULT GAMES IN ALL SYSTEMS"), RecalboxConf::Instance().GetFilterAdultGames(), (int)Components::AdultGames, this, _(MENUMESSAGE_GAMELISTOPTION_HIDE_ADULT_MSG));

  // Custom config for systems
  AddSubMenu(_("ADVANCED EMULATOR CONFIGURATION"), (int)Components::AdvancedSubMenu, _(MENUMESSAGE_ADVANCED_EMULATOR_ADVANCED_HELP_MSG));

  //Kodi
  if (RecalboxSystem::kodiExists() && RecalboxConf::Instance().GetKodiEnabled())
    AddSubMenu(_("KODI SETTINGS"), (int)Components::KodiSubMenu, _(MENUMESSAGE_ADVANCED_KODI_HELP_MSG));

  //Security
  AddSubMenu(_("SECURITY"), (int)Components::SecuritySubMenu, _(MENUMESSAGE_ADVANCED_SECURITY_HELP_MSG));

  // overscan
  mOverscan = AddSwitch(_("OVERSCAN"), RecalboxConf::Instance().GetOverscan(), (int)Components::Overscan, this, _(MENUMESSAGE_ADVANCED_OVERSCAN_HELP_MSG));

  // framerate
  mShowFPS = AddSwitch(_("SHOW FRAMERATE"), RecalboxConf::Instance().GetGlobalShowFPS(), (int)Components::ShowFPS, this, _(MENUMESSAGE_ADVANCED_FRAMERATE_HELP_MSG));

  // Recalbox Manager
  mWebManager = AddSwitch(_("RECALBOX MANAGER"), RecalboxConf::Instance().GetSystemManagerEnabled(), (int)Components::Manager, this, _(MENUMESSAGE_ADVANCED_MANAGER_HELP_MSG));
}

GuiMenuAdvancedSettings::~GuiMenuAdvancedSettings()
{
  if (mValidOverclock)
    if (mOriginalOverclock != mOverclock->getSelected().Description)
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
    list.push_back({ overclock.Description + (overclock.Hazardous ? " \u26a0" : ""), overclock, found });
  }
  // Add none
  mValidOverclock = !list.empty();
  if (mValidOverclock)
    list.push_back({ _("NONE"), mDefaultOverclock, !ocFound });
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
    // Extract lines - doesn't matter if the file does not load, it returns an empty string
    for(const std::string& line : Strings::Split(Files::LoadFile(path), '\n'))
      if (Strings::StartsWith(line, LEGACY_STRING("# Description: ")))
        Strings::SplitAt(line, ':', trash, desc, true);
      else if (Strings::StartsWith(line, LEGACY_STRING("# Warning")))
        hazard = true;
    // Record?
    if (!desc.empty())
    {
      result.push_back({ path.ToString(), desc, hazard });
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
    case Components::AdultGames: RecalboxConf::Instance().SetFilterAdultGames(status).Save(); break;
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
    case Components::SecuritySubMenu:break;
  }
}

void GuiMenuAdvancedSettings::SubMenuSelected(int id)
{
  switch ((Components)id)
  {
    case Components::BootSubMenu: mWindow.pushGui(new GuiMenuBootSettings(mWindow, mSystemManager)); break;
    case Components::VirtualSubMenu: mWindow.pushGui(new GuiMenuVirtualSystems(mWindow, mSystemManager)); break;
    case Components::AdvancedSubMenu: mWindow.pushGui(new GuiMenuSystemList(mWindow, mSystemManager)); break;
    case Components::KodiSubMenu: mWindow.pushGui(new GuiMenuKodiSettings(mWindow)); break;
    case Components::SecuritySubMenu: mWindow.pushGui(new GuiMenuSecurity(mWindow)); break;
    case Components::OverclockList:
    case Components::AdultGames:
    case Components::Overscan:
    case Components::ShowFPS:
    case Components::Manager:break;
  }
}
