#pragma once

#include "guis/Gui.h"
#include "components/MenuComponent.h"
#include "components/TextComponent.h"
#include "guis/GuiArcadeVirtualKeyboardInterface.h"

// The starting point for a multi-game scrape.
// Allows the user to set various parameters.
class GuiNetPlayEditPasswords : public Gui, private GuiArcadeVirtualKeyboardInterface
{
  public:
    explicit GuiNetPlayEditPasswords(Window& window);

    bool getHelpPrompts(Help& help) override;

  private:
    //! Max passwords
    static constexpr int sPasswordCount = 15;

    //! Password storage
    std::shared_ptr<TextComponent> mPasswords[sPasswordCount];

    //! Current password index being edited
    int mCurrentPasswordIndex;

    //! Underlying Menu component
  	MenuComponent mMenu;

  	/*
  	 * Component implementation
  	 */

    bool ProcessInput(const InputCompactEvent& event) override;

    /*
     * GuiArcadeVirtualKeyboardInterface implementation
     */

    /*!
     * @brief Called when the edited text change.
     * Current text is available from the Text() method.
     */
    void ArcadeVirtualKeyboardTextChange(GuiArcadeVirtualKeyboard& vk, const std::string& text) override;

    /*!
     * @brief Called when the edited text is validated (Enter or Start)
     * Current text is available from the Text() method.
     */
    void ArcadeVirtualKeyboardValidated(GuiArcadeVirtualKeyboard& vk, const std::string& text) override;

    /*!
     * @brief Called when the edited text is cancelled.
     */
    void ArcadeVirtualKeyboardCanceled(GuiArcadeVirtualKeyboard& vk) override;
};
