#pragma once

#include "Gui.h"
#include "components/NinePatchComponent.h"
#include "components/ComponentGrid.h"
#include "resources/Font.h"

class TextComponent;
class ButtonComponent;

class GuiMsgBox : public Gui
{
public:
	GuiMsgBox(Window& window, const std::string& text,
            const std::string& name1, const std::function<void()>& func1,
            const std::string& name2, const std::function<void()>& func2,
            const std::string& name3, const std::function<void()>& func3,
            TextAlignment align = TextAlignment::Center);
  GuiMsgBox(Window& window, const std::string& text,
            const std::string& name1, const std::function<void()>& func1,
            const std::string& name2, const std::function<void()>& func2);
  GuiMsgBox(Window& window, const std::string& text,
            const std::string& name1, const std::function<void()>& func1);
  GuiMsgBox(Window& window, const std::string& text,
            const std::string& name1);
  GuiMsgBox(Window& window, const std::string& text);

	bool ProcessInput(const InputCompactEvent& event) override;
	void onSizeChanged() override;
	bool getHelpPrompts(Help& help) override { return mGrid.getHelpPrompts(help); }

private:
  explicit GuiMsgBox(Window& window);

	void deleteMeAndCall(const std::function<void()>& func);

  void build(const std::string& text, TextAlignment align,
             const std::string& name1, const std::function<void()>& func1,
             const std::string& name2, const std::function<void()>& func2,
             const std::string& name3, const std::function<void()>& func3);

	NinePatchComponent mBackground;
	ComponentGrid mGrid;


	std::shared_ptr<TextComponent> mMsg;
	std::vector< std::shared_ptr<ButtonComponent> > mButtons;
	std::shared_ptr<ComponentGrid> mButtonGrid;
	std::function<void()> mAcceleratorFunc;
};
