#pragma once

#include "GuiComponent.h"
#include "components/NinePatchComponent.h"
#include "components/ComponentGrid.h"
#include "resources/Font.h"

class TextComponent;
class ButtonComponent;
class ScrollableContainer;

class GuiMsgBoxScroll : public GuiComponent
{
public:
	GuiMsgBoxScroll(Window* window, const std::string& title, const std::string& text,
                  const std::string& name1, const std::function<void()>& func1,
                  const std::string& name2, const std::function<void()>& func2,
                  const std::string& name3, const std::function<void()>& func3,
                  TextAlignment align, float size);
  GuiMsgBoxScroll(Window* window, const std::string& title, const std::string& text,
                  const std::string& name1, const std::function<void()>& func1,
                  const std::string& name2, const std::function<void()>& func2,
                  const std::string& name3, const std::function<void()>& func3,
                  TextAlignment align)
    : GuiMsgBoxScroll(window, title, text, name1, func1, name2, func2, name3, func3, align, 0)
  {
  }
  GuiMsgBoxScroll(Window* window, const std::string& title, const std::string& text,
                  const std::string& name1, const std::function<void()>& func1,
                  const std::string& name2, const std::function<void()>& func2,
                  const std::string& name3, const std::function<void()>& func3)
    : GuiMsgBoxScroll(window, title, text, name1, func1, name2, func2, name3, func3, TextAlignment::Center, 0.0f)
  {
  }

	bool input(InputConfig* config, Input input) override;
	void onSizeChanged() override;
	std::vector<HelpPrompt> getHelpPrompts() override { return mGrid.getHelpPrompts(); }

private:
	void deleteMeAndCall(const std::function<void()>& func);

	NinePatchComponent mBackground;
	ComponentGrid mGrid;

	std::shared_ptr<TextComponent> mTitle;
	std::shared_ptr<TextComponent> mMsg;
	std::vector< std::shared_ptr<ButtonComponent> > mButtons;
	std::shared_ptr<ComponentGrid> mButtonGrid;
	std::function<void()> mAcceleratorFunc;
	std::shared_ptr<ScrollableContainer> mMsgContainer;
};
