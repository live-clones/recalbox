#pragma once

#include "components/base/Component.h"
#include <functional>
#include "resources/Font.h"
#include "components/NinePatchComponent.h"
#include <string>

class ButtonComponent : public Component
{
public:
	ButtonComponent(WindowManager&window, const std::string& text, const std::string& helpText, const std::function<void()>& func, bool upperCase);
  ButtonComponent(WindowManager&window, const std::string& text, const std::string& helpText, const std::function<void()>& func)
    : ButtonComponent(window, text, helpText, func, true)
  {
  }

	void setPressedFunc(std::function<void()> f);

	void setEnabled(bool enable);

	bool ProcessInput(const InputCompactEvent& event) override;
	void Render(const Transform4x4f& parentTrans) override;

	void setText(const std::string& text, const std::string& helpText, bool upperCase = true, bool resize = true, bool doUpdateHelpPrompts = true);
	void autoSizeFont();
	bool isTextOverlapping();

	inline const std::string& getText() const { return mText; };
	inline const std::function<void()>& getPressedFunc() const { return mPressedFunc; };

	void onSizeChanged() override;
	void onFocusGained() override;
	void onFocusLost() override;

	void setColorShift(unsigned int color) { mModdedColor = color; mNewColor = true; updateImage(); }
	void removeColorShift() { mNewColor = false; updateImage(); }

  bool getHelpPrompts(Help& help) override;

private:
	std::shared_ptr<Font> mFont;
	std::function<void()> mPressedFunc;

	bool mFocused;
	bool mEnabled;
	bool mNewColor = false;
	unsigned int mTextColorFocused;
	unsigned int mTextColorUnfocused;
	unsigned int mModdedColor;
	unsigned int mColor;
	
	unsigned int getCurTextColor() const;
	void updateImage();

	std::string mText;
	std::string mHelpText;
	Path mButton;
	Path mButton_filled;
	std::unique_ptr<TextCache> mTextCache;
	NinePatchComponent mBox;
};
