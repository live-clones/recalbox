#pragma once

#include <guis/Gui.h>
#include <components/NinePatchComponent.h>
#include <components/ComponentGrid.h>
#include <input/InputDevice.h>

class TextComponent;

class GuiDetectDevice : public Gui
{
public:
	GuiDetectDevice(WindowManager& window, bool firstRun, const std::function<void()>& doneCallback);

	bool ProcessInput(const InputCompactEvent& event) override;
	void Update(int deltaTime) override;
	void onSizeChanged() override;

private:
	bool mFirstRun;
	InputDevice* mHoldingConfig;
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
