//
// Created by gugue_u on 24/09/22.
//
#pragma once

#include "GuiMenuBase.h"
#include "systems/SystemManager.h"

class GuiMenuSystems : public GuiMenuBase, private ISwitchComponent

{
  public:
    explicit GuiMenuSystems(WindowManager& window, SystemManager& systemManager);

  private:
    //! System manager
    SystemManager& mSystemManager;

    void SwitchComponentChanged(int id, bool status) override;
};

