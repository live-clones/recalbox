#pragma once

#include <string>
class GuiArcadeVirtualKeyboard;

class IGuiArcadeVirtualKeyboardInterface
{
  public:
    virtual ~IGuiArcadeVirtualKeyboardInterface() = default;

    /*!
     * @brief Called when the VK opens
     */
    virtual void ArcadeVirtualKeyboardShow(GuiArcadeVirtualKeyboard& vk) { (void)vk; }

    /*!
     * @brief Called when the edited text change.
     * Current text is available from the Text() method.
     */
    virtual void ArcadeVirtualKeyboardTextChange(GuiArcadeVirtualKeyboard& vk, const std::string& text) = 0;

    /*!
     * @brief Called when the edited text is validated (Enter or Start)
     * Current text is available from the Text() method.
     */
    virtual void ArcadeVirtualKeyboardValidated(GuiArcadeVirtualKeyboard& vk, const std::string& text) = 0;

    /*!
     * @brief Called when the edited text is cancelled.
     */
    virtual void ArcadeVirtualKeyboardCanceled(GuiArcadeVirtualKeyboard& vk) = 0;

    /*!
     * @brief Called when the VK closes
     */
    virtual void ArcadeVirtualKeyboardHide(GuiArcadeVirtualKeyboard& vk) { (void)vk; }
};
