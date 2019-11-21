#pragma once

#include "GuiComponent.h"
#include "components/ImageComponent.h"

// A very simple "on/off" switch.
// Should hopefully be switched to use images instead of text in the future.
class SwitchComponent : public GuiComponent
{
public:
	explicit SwitchComponent(Window&window);
  SwitchComponent(Window&window, bool state)
    : SwitchComponent(window)
  {
    mState = state;
    onStateChanged();
  }

	bool ProcessInput(const InputCompactEvent& event) override;
	void render(const Transform4x4f& parentTrans) override;
	void onSizeChanged() override;
	void setColor(unsigned int color) override;
	inline void setOriginColor(unsigned int color){mOriginColor = color;};
	inline unsigned int getOriginColor() override{return mOriginColor;};

	bool getState() const  { return mState; }

  void setState(bool state);
	std::string getValue() const override;
	bool changed();

	bool getHelpPrompts(Help& help) override;

private:
	void onStateChanged();

	ImageComponent mImage;
  unsigned int mOriginColor;
	bool mState;
	bool mInitialState;
};
