#pragma once

#include "guis/Gui.h"
#include "components/MenuComponent.h"
#include "components/TextComponent.h"
#include <netplay/DefaultPasswords.h>
#include <games/FileData.h>

template<typename T>
class OptionListComponent;
class SwitchComponent;

// The starting point for a multi-game scrape.
// Allows the user to set various parameters.
class GuiNetPlayHostPasswords : public Gui
{
  public:
    explicit GuiNetPlayHostPasswords(WindowManager& window, FileData& game);

    bool getHelpPrompts(Help& help) override;

  private:
    //! Target game
    FileData& mGame;

    //! Player password on/off
    std::shared_ptr<SwitchComponent> mPlayerPasswordOnOff;
    //! Viewer password on/off
    std::shared_ptr<SwitchComponent> mViewerPasswordOnOff;

    //! Player password select
    std::shared_ptr<OptionListComponent<int>> mPlayerPassword;
    //! Viewer password select
    std::shared_ptr<OptionListComponent<int>> mViewerPassword;

    //! Underlying Menu component
  	MenuComponent mMenu;

  	/*
  	 * Component implementation
  	 */

    bool ProcessInput(const InputCompactEvent& event) override;
};
