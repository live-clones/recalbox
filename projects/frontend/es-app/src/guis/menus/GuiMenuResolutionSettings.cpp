//
// Created by bkg2k on 31/01/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include "GuiMenuResolutionSettings.h"
#include "GuiMenuResolutionByEmulator.h"
#include <guis/MenuMessages.h>
#include <systems/SystemManager.h>

GuiMenuResolutionSettings::GuiMenuResolutionSettings(WindowManager& window, SystemManager& systemManager)
  : GuiMenuBase(window, _("RESOLUTIONS"), this)
  , mSystemManager(systemManager)
{
  // Add global video mode
  AddList<std::string>(_("GLOBAL RESOLUTION"), (int)Components::GlobalResolution, this, GetGlobalResolutionEntries(), _(MENUMESSAGE_ADVANCED_RESOLUTION_GLOBAL_HELP_MSG));

  // Add frontend video mode
  AddList<std::string>(_("EMULATIONSTATION RESOLUTION"), (int)Components::FrontendResolution, this, GetFrontEndResolutionEntries(), _(MENUMESSAGE_ADVANCED_RESOLUTION_FRONTEND_HELP_MSG));

  // Emulator resolutions
  AddSubMenu(_("EMULATORS SPECIFIC RESOLUTIONS"), (int)Components::Emulators, _(MENUMESSAGE_ADVANCED_RESOLUTION_EMULATORS_HELP_MSG));
}

std::vector<GuiMenuBase::ListEntry<std::string>> GuiMenuResolutionSettings::GetGlobalResolutionEntries()
{
  std::vector<GuiMenuBase::ListEntry<std::string>> result;
  result.push_back({ _("NATIVE"), "default", !RecalboxConf::Instance().IsDefinedGlobalVideoMode() || RecalboxConf::Instance().GetGlobalVideoMode() == "default" });
  for(const Renderer::Resolution& resolution : Renderer::GetFilteredResolutionList())
  {
    std::string reso = resolution.ToString();
    result.push_back({ reso, reso, reso == RecalboxConf::Instance().GetGlobalVideoMode() });
  }
  return result;
}

std::vector<GuiMenuBase::ListEntry<std::string>> GuiMenuResolutionSettings::GetFrontEndResolutionEntries()
{
  std::vector<GuiMenuBase::ListEntry<std::string>> result;
  result.push_back({ _("USE GLOBAL"), "", !RecalboxConf::Instance().IsDefinedEmulationstationVideoMode() });
  result.push_back({ _("NATIVE"), "default", RecalboxConf::Instance().GetEmulationstationVideoMode() == "default" });
  for(const Renderer::Resolution& resolution : Renderer::GetFilteredResolutionList())
  {
    std::string reso = resolution.ToString();
    result.push_back({ reso, reso, reso == RecalboxConf::Instance().GetEmulationstationVideoMode() });
  }
  return result;
}

void GuiMenuResolutionSettings::OptionListComponentChanged(int id, int index, const std::string& value)
{
  (void)index;
  switch((Components)id)
  {
    case Components::GlobalResolution:
    {
      RecalboxConf::Instance().SetGlobalVideoMode(value).Save();
      break;
    }
    case Components::FrontendResolution:
    {
      if (value.empty()) RecalboxConf::Instance().DeleteEmulationstationVideoMode().Save();
      else RecalboxConf::Instance().SetEmulationstationVideoMode(value).Save();
      break;
    }
    case Components::Emulators:
    default: break;
  }
}

void GuiMenuResolutionSettings::SubMenuSelected(int id)
{
  if ((Components)id == Components::Emulators)
    mWindow.pushGui(new GuiMenuResolutionByEmulator(mWindow, mSystemManager));
}
