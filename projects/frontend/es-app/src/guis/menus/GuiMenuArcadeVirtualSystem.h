#pragma once

#include "guis/Gui.h"
#include "systems/SystemData.h"
#include "components/MenuComponent.h"

template<typename T>
class OptionListComponent;
class SwitchComponent;

// The starting point for a multi-game scrape.
// Allows the user to set various parameters.
class GuiMenuArcadeVirtualSystem : public Gui
{
  public:
    explicit GuiMenuArcadeVirtualSystem(WindowManager& window, SystemManager& systemManager);

    bool getHelpPrompts(Help& help) override;

    /*
     * Component implementation
     */

    bool ProcessInput(const InputCompactEvent& event) override;

  private:
    //! SystemManager instance
    SystemManager& mSystemManager;

    std::shared_ptr<SwitchComponent>          mArcadeOnOff;
    std::shared_ptr<SwitchComponent>          mIncludeNeoGeo;
    std::shared_ptr<SwitchComponent>          mHideOriginals;
    std::shared_ptr<OptionListComponent<int>> mPosition;

    MenuComponent mMenu;
};
