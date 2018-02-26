#pragma once

#include "GuiComponent.h"
#include "components/NinePatchComponent.h"
#include "components/ComponentGrid.h"

class TextComponent;

class GuiDetectDevice : public GuiComponent
{
public:
	GuiDetectDevice(Window* window, bool firstRun, const std::function<void()>& doneCallback);

	bool input(InputConfig* config, Input input) override;
	void update(int deltaTime) override;
	void onSizeChanged() override;

private:
	bool mFirstRun;
	InputConfig* mHoldingConfig;
	int mHoldTime;

	NinePatchComponent mBackground;
	ComponentGrid mGrid;

	std::shared_ptr<TextComponent> mTitle;
	std::shared_ptr<TextComponent> mMsg1;
	std::shared_ptr<TextComponent> mMsg2;
	std::shared_ptr<TextComponent> mDeviceInfo;
	std::shared_ptr<TextComponent> mDeviceHeld;
	unsigned int mColor;
	unsigned int mAlpha = 0;

	std::function<void()> mDoneCallback;
};
