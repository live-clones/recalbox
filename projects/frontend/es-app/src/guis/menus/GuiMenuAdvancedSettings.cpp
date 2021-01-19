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

bool GuiMenuAdvancedSettings::sLastHazardous = false;

GuiMenuAdvancedSettings::GuiMenuAdvancedSettings(WindowManager& window, SystemManager& systemManager)
  : GuiMenuBase(window, _("ADVANCED SETTINGS"))
  , mSystemManager(systemManager)
  , mDefaultOverclock({ _("NONE"), "none", false})
{
  sLastHazardous = false;

  // Overclock choice
  mOverclock = AddList<Overclocking>(_("OVERCLOCK"), SetOverclock, GetOverclockEntries(), _(MENUMESSAGE_ADVANCED_OVERCLOCK_HELP_MSG));

  // Boot
  AddSubMenu(_("BOOT SETTINGS"), [this] { mWindow.pushGui(new GuiMenuBootSettings(mWindow, mSystemManager)); }, _(MENUMESSAGE_ADVANCED_BOOT_HELP_MSG));

  // Virtual systems
  AddSubMenu(_("VIRTUAL SYSTEMS"), [this] { mWindow.pushGui(new GuiMenuVirtualSystems(mWindow, mSystemManager)); }, _(MENUMESSAGE_ADVANCED_VIRTUALSYSTEMS_HELP_MSG));

  // Adult games
  mAdult = AddSwitch(_("HIDE ADULT GAMES IN ALL SYSTEMS"), RecalboxConf::Instance().GetFilterAdultGames(), SetAdult, _(MENUMESSAGE_GAMELISTOPTION_HIDE_ADULT_MSG));

  // Custom config for systems
  AddSubMenu(_("ADVANCED EMULATOR CONFIGURATION"), [this] { mWindow.pushGui(new GuiMenuSystemList(mWindow, mSystemManager)); }, _(MENUMESSAGE_ADVANCED_EMULATOR_ADVANCED_HELP_MSG));

  //Kodi
  if (RecalboxSystem::kodiExists())
    AddSubMenu(_("KODI SETTINGS"), [this] { mWindow.pushGui(new GuiMenuKodiSettings(mWindow)); }, _(MENUMESSAGE_ADVANCED_KODI_HELP_MSG));

  //Security
  AddSubMenu(_("SECURITY"), [this] { mWindow.pushGui(new GuiMenuSecurity(mWindow)); }, _(MENUMESSAGE_ADVANCED_SECURITY_HELP_MSG));

  // overscan
  mOverscan = AddSwitch(_("OVERSCAN"), RecalboxConf::Instance().GetOverscan(), SetOverscan, _(MENUMESSAGE_ADVANCED_OVERSCAN_HELP_MSG));

  // framerate
  mShowFPS = AddSwitch(_("SHOW FRAMERATE"), RecalboxConf::Instance().GetGlobalShowFPS(), SetShowFPS, _(MENUMESSAGE_ADVANCED_FRAMERATE_HELP_MSG));

  // Recalbox Manager
  mWebManager = AddSwitch(_("RECALBOX MANAGER"), RecalboxConf::Instance().GetSystemManagerEnabled(), SetWebmanager, _(MENUMESSAGE_ADVANCED_MANAGER_HELP_MSG));
}

std::vector<GuiMenuBase::ListEntry<GuiMenuAdvancedSettings::Overclocking>> GuiMenuAdvancedSettings::GetOverclockEntries()
{
  std::vector<ListEntry<GuiMenuAdvancedSettings::Overclocking>> list;

  OverclockList oc = AvailableOverclocks();
  mOriginalOverclock = RecalboxConf::Instance().GetOverclocking();
  bool ocFound = false;
  for(const Overclocking& overclock : oc)
  {
    bool found = mOriginalOverclock == overclock.Description;
    ocFound |= found;
    list.push_back({ overclock.Description + (overclock.Hazardous ? " \u26a0" : ""), overclock, found });
  }
  list.push_back({ _("NONE"), mDefaultOverclock, !ocFound });
  if (!ocFound) mOriginalOverclock = "none";

  return list;
}

GuiMenuAdvancedSettings::~GuiMenuAdvancedSettings()
{
  if (mOriginalOverclock != mOverclock->getSelected().Description)
  {
    if (sLastHazardous)
      mWindow.pushGui(
        new GuiMsgBox(mWindow, _("TURBO AND EXTREM OVERCLOCK PRESETS MAY CAUSE SYSTEM UNSTABILITIES, SO USE THEM AT YOUR OWN RISK.\nIF YOU CONTINUE, THE SYSTEM WILL REBOOT NOW."),
                      _("YES"), [] { MainRunner::RequestQuit(MainRunner::ExitState::NormalReboot); }, _("NO"), [this] { ResetOverclock(); }));
    else
      RequestReboot();
  }
}

GuiMenuAdvancedSettings::OverclockList GuiMenuAdvancedSettings::AvailableOverclocks()
{
  OverclockList result;

  // Build final path to fetch overclocking configs from
  Path basePath(sOverclockBaseFolder);
  std::string boardFolder;
  switch(Board::Instance().GetBoardType())
  {
    case BoardType::Pi1: boardFolder = "rpi1"; break;
    case BoardType::Pi2: boardFolder = "rpi2"; break;
    case BoardType::Pi3: boardFolder = "rpi3"; break;
    case BoardType::Pi3plus: boardFolder = "rpi3plus"; break;
    case BoardType::Pi4: boardFolder = "rpi4"; break;
    case BoardType::Pi400: boardFolder = "rpi400"; break;
    case BoardType::UndetectedYet:
    case BoardType::Unknown:
    case BoardType::Pi0:
    case BoardType::UnknownPi:
    case BoardType::OdroidAdvanceGo2:
    case BoardType::PCx86:
    case BoardType::PCx64: break;
  }
  if (boardFolder.empty()) return result;
  Path finalPath = basePath / boardFolder;

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
      result.push_back({ path.ToString(), desc, hazard });
  }

  return result;
}

void GuiMenuAdvancedSettings::SetOverclock(const Overclocking& oc)
{
  sLastHazardous = oc.Hazardous;
  RecalboxConf::Instance().SetOverclocking(oc.File).Save();

  if (RecalboxSystem::MakeBootReadWrite())
  {
    Files::SaveFile(Path(sOverclockFile), Files::LoadFile(Path(oc.File)));
    RecalboxSystem::MakeBootReadOnly();
    LOG(LogInfo) << "[Overclock] Overclocking set to " << oc.Description;
  }
  else LOG(LogError) << "[Overclock] Cannot make boot read write";
}

void GuiMenuAdvancedSettings::ResetOverclock()
{
  SetOverclock(mDefaultOverclock);
  mOverclock->select(mDefaultOverclock);
}

void GuiMenuAdvancedSettings::SetAdult(bool on)
{
  RecalboxConf::Instance().SetFilterAdultGames(on).Save();
}

void GuiMenuAdvancedSettings::SetOverscan(bool on)
{
  RecalboxConf::Instance().SetOverscan(on);
  RecalboxSystem::setOverscan(on);
}

void GuiMenuAdvancedSettings::SetShowFPS(bool on)
{
  RecalboxConf::Instance().SetGlobalShowFPS(on).Save();
}

void GuiMenuAdvancedSettings::SetWebmanager(bool on)
{
  RecalboxConf::Instance().SetSystemManagerEnabled(on).Save();
}

