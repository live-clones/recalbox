#include "guis/GuiDetectDevice.h"
#include "Window.h"
#include "Renderer.h"
#include "resources/Font.h"
#include "guis/GuiInputConfig.h"
#include "components/TextComponent.h"
#include <iostream>
#include <string>
#include <sstream>
#include "Util.h"
#include "Locale.h"
#include "MenuThemeData.h"

#define HOLD_TIME 1000

using namespace Eigen;

GuiDetectDevice::GuiDetectDevice(Window* window, bool firstRun, const std::function<void()>& doneCallback) : GuiComponent(window), mFirstRun(firstRun), 
	mBackground(window, ":/frame.png"), mGrid(window, Vector2i(1, 5))
{
	mHoldingConfig = NULL;
	mHoldTime = 0;
	mDoneCallback = doneCallback;
	
	auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
	mColor = menuTheme->menuText.color & 0xFFFFFF00;
	
	mBackground.setImagePath(menuTheme->menuBackground.path);
	mBackground.setCenterColor(menuTheme->menuBackground.color);
	mBackground.setEdgeColor(menuTheme->menuBackground.color);
	
	addChild(&mBackground);
	addChild(&mGrid);
	
	// title
	mTitle = std::make_shared<TextComponent>(mWindow, firstRun ? _("WELCOME") : _("CONFIGURE INPUT"),
		 menuTheme->menuTitle.font, menuTheme->menuTitle.color, ALIGN_CENTER);
	//mGrid.setEntry(mTitle, Vector2i(0, 0), false, true, Vector2i(1, 1), GridFlags::BORDER_BOTTOM);
	mGrid.setEntry(mTitle, Vector2i(0, 0), false, true);
	
	// device info
	std::stringstream deviceInfo;
	int numDevices = InputManager::getInstance()->getNumJoysticks();
	
	if(numDevices > 0) {
	  char strbuf[256];
	  snprintf(strbuf, 256, ngettext("%i GAMEPAD DETECTED", "%i GAMEPADS DETECTED", numDevices).c_str(), numDevices);
	  deviceInfo << strbuf;
	}
	else
		deviceInfo << _("NO GAMEPADS DETECTED");
	mDeviceInfo = std::make_shared<TextComponent>(mWindow, deviceInfo.str(),  menuTheme->menuTextSmall.font, menuTheme->menuTextSmall.color, ALIGN_CENTER);
	mGrid.setEntry(mDeviceInfo, Vector2i(0, 1), false, true);

	// message
	mMsg1 = std::make_shared<TextComponent>(mWindow, _("HOLD A BUTTON ON YOUR DEVICE TO CONFIGURE IT."), menuTheme->menuTextSmall.font, menuTheme->menuTextSmall.color, ALIGN_CENTER);
	mGrid.setEntry(mMsg1, Vector2i(0, 2), false, true);

	if (firstRun)
		mMsg2 = std::make_shared<TextComponent>(mWindow, _("PRESS F4 TO QUIT AT ANY TIME.").c_str(), menuTheme->menuTextSmall.font, menuTheme->menuTextSmall.color, ALIGN_CENTER);
	else
		mMsg2 = std::make_shared<TextComponent>(mWindow, _("PRESS ESC OR THE HOTKEY TO CANCEL.").c_str(), menuTheme->menuTextSmall.font, menuTheme->menuTextSmall.color, ALIGN_CENTER);
	mGrid.setEntry(mMsg2, Vector2i(0, 3), false, true);

	// currently held device
	mDeviceHeld = std::make_shared<TextComponent>(mWindow, "", menuTheme->menuText.font, mColor, ALIGN_CENTER);
	mGrid.setEntry(mDeviceHeld, Vector2i(0, 4), false, true);

	float y= ((mMsg1->getFont()->getHeight() *4) + mTitle->getFont()->getHeight() ) / Renderer::getScreenHeight() + 0.08;

	setSize(Renderer::getScreenWidth() * 0.6f, Renderer::getScreenHeight() * y);
	setPosition((Renderer::getScreenWidth() - mSize.x()) / 2, (Renderer::getScreenHeight() - mSize.y()) / 2);
}

void GuiDetectDevice::onSizeChanged()
{
	mBackground.fitTo(mSize, Vector3f::Zero(), Vector2f(-32, -32));

	// grid
	mGrid.setSize(mSize);
	mGrid.setRowHeightPerc(0, mTitle->getFont()->getHeight() / mSize.y());
	mGrid.setRowHeightPerc(1, mDeviceInfo->getFont()->getHeight() / mSize.y());
	mGrid.setRowHeightPerc(2, mMsg1->getFont()->getHeight() / mSize.y());
	mGrid.setRowHeightPerc(3, mMsg2->getFont()->getHeight() / mSize.y());
	mGrid.setRowHeightPerc(4, mDeviceHeld->getFont()->getHeight() *1.5 / mSize.y());
}

bool GuiDetectDevice::input(InputConfig* config, Input input)
{
	if(!mFirstRun && (input.device == DEVICE_KEYBOARD && input.type == TYPE_KEY && input.value && input.id == SDLK_ESCAPE) ||
	                 (input.device != DEVICE_KEYBOARD && config->isMappedTo("hotkey", input)))
	{
		// cancel configuring
		delete this;
		return true;
	}

	if(input.type == TYPE_BUTTON || input.type == TYPE_KEY)
	{
		if(input.value && mHoldingConfig == NULL)
		{
			// started holding
			mHoldingConfig = config;
			mHoldTime = HOLD_TIME;
			mDeviceHeld->setText(strToUpper(config->getDeviceName()));
		}else if(!input.value && mHoldingConfig == config)
		{
			// cancel
			mHoldingConfig = NULL;
			mDeviceHeld->setText("");
			mAlpha = 0;
		}
	}

	return true;
}

void GuiDetectDevice::update(int deltaTime)
{
	if(mHoldingConfig)
	{
		mHoldTime -= deltaTime;
		int t = HOLD_TIME / deltaTime;
		mAlpha += 255 / t;
		mDeviceHeld->setColor(mColor | mAlpha);
		if(mHoldTime <= 0)
		{
			// picked one!
			mWindow->pushGui(new GuiInputConfig(mWindow, mHoldingConfig, mDoneCallback));
			delete this;
		}
	}
}
