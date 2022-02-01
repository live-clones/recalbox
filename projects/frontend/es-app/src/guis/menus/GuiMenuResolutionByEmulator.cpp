//
// Created by bkg2k on 31/01/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include "GuiMenuResolutionByEmulator.h"
#include "guis/MenuMessages.h"
#include <systems/SystemManager.h>

GuiMenuResolutionByEmulator::GuiMenuResolutionByEmulator(WindowManager& window, SystemManager& systemManager)
  : GuiMenuBase(window, _("SYSTEM RESOLUTIONS"), nullptr)
  , mSystemManager(systemManager)
{
  // For each activated system
  const std::vector<SystemData*> systems = systemManager.GetAllSystemList();
  for(int i = 0; i < (int)systems.size(); ++i)
    if (!systems[i]->IsVirtual())
      AddList<std::string>(systems[i]->FullName(), i, this, GetResolutionEntries(*systems[i]), _(MENUMESSAGE_ADVANCED_RESOLUTION_SYSTEM_HELP_MSG));

}

std::vector<GuiMenuBase::ListEntry<std::string>> GuiMenuResolutionByEmulator::GetResolutionEntries(SystemData& system)
{
  std::vector<GuiMenuBase::ListEntry<std::string>> result;
  result.push_back({ _("USE GLOBAL"), "", !RecalboxConf::Instance().IsDefinedSystemVideoMode(system) });
  result.push_back({ _("NATIVE"), "default", RecalboxConf::Instance().GetSystemVideoMode(system) == "default" });
  for(const Renderer::Resolution& resolution : Renderer::GetFilteredResolutionList())
  {
    std::string reso = resolution.ToString();
    result.push_back({ reso, reso, reso == RecalboxConf::Instance().GetSystemVideoMode(system) });
  }
  return result;
}

void GuiMenuResolutionByEmulator::OptionListComponentChanged(int id, int index, const std::string& value)
{
  (void)index;
  if (value.empty()) RecalboxConf::Instance().DeleteSystemVideoMode(*mSystemManager.GetAllSystemList()[id]).Save();
  else RecalboxConf::Instance().SetSystemVideoMode(*mSystemManager.GetAllSystemList()[id], value).Save();
}
