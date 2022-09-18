//
// Created by gugue_u on 24/09/22.
//

#include "GuiMenuSystems.h"
#include "views/ViewController.h"

GuiMenuSystems::GuiMenuSystems(WindowManager& window, SystemManager& systemManager)
  : GuiMenuBase(window, _("SYSTEMS"), nullptr)
  , mSystemManager(systemManager)

{
  int id = 0;
  for(const auto & system : mSystemManager.GetVisibleSystemList())
  {
    AddSwitch(system->Descriptor().FullName(), !RecalboxConf::Instance().AsBool(system->Name() + ".ignore"), id, this, "");
    id++;
  }
}

GuiMenuSystems::~GuiMenuSystems()
{
  ViewController::Instance().RefreshSystems();
}

void GuiMenuSystems::SwitchComponentChanged(int id, bool status)
{
  SystemData* systemData;
  int loop = 0;

  for(const auto & system : mSystemManager.GetVisibleSystemList())
  {
    if (loop == id) {
      systemData = system;
    }
    loop++;
  }
  RecalboxConf::Instance().SetBool(systemData->Name() + ".ignore", !status);
  RecalboxConf::Instance().Save();
}
