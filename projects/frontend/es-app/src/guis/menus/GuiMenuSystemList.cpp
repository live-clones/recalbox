//
// Created by bkg2k on 04/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include "GuiMenuSystemList.h"
#include "GuiMenuSystemConfiguration.h"
#include <utils/locale/LocaleHelper.h>
#include <systems/SystemManager.h>
#include <systems/SystemData.h>

GuiMenuSystemList::GuiMenuSystemList(WindowManager& window, SystemManager& systemManager)
  : GuiMenuBase(window, _("ADVANCED EMULATOR CONFIGURATION"), this)
  , mSystemManager(systemManager)
{
  // For each activated system
  const std::vector<SystemData*> systems = systemManager.GetAllSystemList();
  for(int i = 0; i < (int)systems.size(); ++i)
    if (!systems[i]->IsVirtual())
      AddSubMenu(systems[i]->FullName(), i);
}

void GuiMenuSystemList::SubMenuSelected(int id)
{
  SystemData& system = *mSystemManager.GetAllSystemList()[id];
  GuiMenuSystemConfiguration::AdvancedMenuOptions options = GuiMenuSystemConfiguration::allOptions;
  if (Board::Instance().CrtBoard().IsCrtAdapterAttached())
    options = {.emulator = true, .ratio=false, .smooth=false, .rewind=true, .autosave=true, .shaders=false, .shaderSet=false};
  mWindow.pushGui(new GuiMenuSystemConfiguration(mWindow, system, mSystemManager, options));
}
