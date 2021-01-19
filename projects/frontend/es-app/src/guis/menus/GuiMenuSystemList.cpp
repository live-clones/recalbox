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
  : GuiMenuBase(window, _("ADVANCED EMULATOR CONFIGURATION"))
  , mSystemManager(systemManager)
{
  // For each activated system
  for (SystemData* system : systemManager.GetAllSystemList())
    if (!system->IsVirtual())
      AddSubMenu(system->getFullName(), [this, system] { SelectSystem(*system); });
}

void GuiMenuSystemList::SelectSystem(SystemData& system)
{
  mWindow.pushGui(new GuiMenuSystemConfiguration(mWindow, system, mSystemManager));
}
