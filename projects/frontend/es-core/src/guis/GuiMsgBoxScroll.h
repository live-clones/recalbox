#pragma once

#include <guis/Gui.h>
#include <components/NinePatchComponent.h>
#include <components/ComponentGrid.h>
#include <resources/Font.h>

class TextComponent;
class ButtonComponent;
class ScrollableContainer;

class GuiMsgBoxScroll : public Gui
{
public:
	GuiMsgBoxScroll(WindowManager& window, const std::string& title, const std::string& text,
                  const std::string& name1, const std::function<void()>& func1,
                  const std::string& name2, const std::function<void()>& func2,
                  const std::string& name3, const std::function<void()>& func3,
                  TextAlignment align, float size);
  GuiMsgBoxScroll(WindowManager& window, const std::string& title, const std::string& text,
                  const std::string& name1, const std::function<void()>& func1,
                  const std::string& name2, const std::function<void()>& func2,
                  const std::string& name3, const std::function<void()>& func3,
                  TextAlignment align)
    : GuiMsgBoxScroll(window, title, text, name1, func1, name2, func2, name3, func3, align, 0)
  {
  }
  GuiMsgBoxScroll(WindowManager& window, const std::string& title, const std::string& text,
                  const std::string& name1, const std::function<void()>& func1,
                  const std::string& name2, const std::function<void()>& func2,
                  const std::string& name3, const std::function<void()>& func3)
    : GuiMsgBoxScroll(window, title, text, name1, func1, name2, func2, name3, func3, TextAlignment::Center, 0.0f)
  {
  }

	bool ProcessInput(const InputCompactEvent& event) override;
	void onSizeChanged() override;
  bool getHelpPrompts(Help& help)  override { return mGrid.getHelpPrompts(help); }

  GuiMsgBoxScroll* SetDefaultButton(int index);

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
