#pragma once

#include "guis/Gui.h"
#include <guis/GuiNetPlay.h>
#include "components/MenuComponent.h"
#include "components/TextComponent.h"
#include <netplay/DefaultPasswords.h>
#include <games/FileData.h>

template<typename T>
class OptionListComponent;

// The starting point for a multi-game scrape.
// Allows the user to set various parameters.
class GuiNetPlayClientPasswords : public Gui
{
  public:
    explicit GuiNetPlayClientPasswords(WindowManager& window, LobbyGame& lobbygame);

    bool getHelpPrompts(Help& help) override;

  private:
    enum class PasswordType
    {
      Player,
      Viewer,
    };

    //! Lobby game
    LobbyGame mLobbyGame;

    //! Password type select
    std::shared_ptr<OptionListComponent<PasswordType>> mJoinAs;
    //! Viewer password select
    std::shared_ptr<OptionListComponent<int>> mPasswords;

    //! Underlying Menu component
  	MenuComponent mMenu;

  	/*
  	 * Component implementation
  	 */

    bool ProcessInput(const InputCompactEvent& event) override;
};
