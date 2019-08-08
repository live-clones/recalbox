#pragma once

#include "GuiComponent.h"
#include "components/ImageComponent.h"

// A very simple "on/off" switch.
// Should hopefully be switched to use images instead of text in the future.
class SwitchComponent : public GuiComponent
{
public:
	explicit SwitchComponent(Window* window, bool state = false);

	bool input(InputConfig* config, Input input) override;
	void render(const Transform4x4f& parentTrans) override;
	void onSizeChanged() override;
	void setColor(unsigned int color) override;
	inline void setOriginColor(unsigned int color){mOriginColor = color;};
	inline unsigned int getOriginColor() override{return mOriginColor;};

	bool getState() const;
	void setState(bool state);
	std::string getValue() const override;
	bool changed();

	std::vector<HelpPrompt> getHelpPrompts() override;

private:
	void onStateChanged();

	ImageComponent mImage;
	bool mState;
	bool mInitialState;
	unsigned int mOriginColor;
};
