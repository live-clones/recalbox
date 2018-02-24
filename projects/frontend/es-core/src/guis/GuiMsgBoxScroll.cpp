#include "guis/GuiMsgBoxScroll.h"
#include "Renderer.h"
#include "components/TextComponent.h"
#include "components/ButtonComponent.h"
#include "components/MenuComponent.h" // for makeButtonGrid
#include "components/ScrollableContainer.h"
#include "Util.h"
#include "Log.h"

#define HORIZONTAL_PADDING_PX 20

GuiMsgBoxScroll::GuiMsgBoxScroll(Window* window,
								 const std::string& title, const std::string& text,
								 const std::string& name1, const std::function<void()>& func1,
	const std::string& name2, const std::function<void()>& func2, 
	const std::string& name3, const std::function<void()>& func3,
        Alignment align, float height) : GuiComponent(window),
	mBackground(window, ":/frame.png"), mGrid(window, Eigen::Vector2i(1, 3))
{
	float width = Renderer::getScreenWidth() * 0.8f; // max width
	float minWidth = Renderer::getScreenWidth() * 0.3f; // minimum width


	auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();

	mBackground.setImagePath(menuTheme->menuBackground.path);
	mBackground.setCenterColor(menuTheme->menuBackground.color);
	mBackground.setEdgeColor(menuTheme->menuBackground.color);

	mTitle = std::make_shared<TextComponent>(mWindow, title, menuTheme->menuTitle.font, menuTheme->menuTitle.color, ALIGN_CENTER);
	mGrid.setEntry(mTitle, Eigen::Vector2i(0, 0), false, true);

	mMsg = std::make_shared<TextComponent>(mWindow, text, menuTheme->menuTextSmall.font, menuTheme->menuTextSmall.color, align);

	mMsgContainer = std::make_shared<ScrollableContainer>(mWindow);
	mMsgContainer->setAutoScroll(true);
	mMsgContainer->mAutoScrollSpeed = 20; // ms between scrolls
	mMsgContainer->mAutoScrollDelay = 3000; // ms to wait before we start to scroll
	mMsgContainer->mAutoScrollResetAccumulator = 5000; // ms to reset to top after we reach the bottom
	mMsgContainer->addChild(mMsg.get());

	mGrid.setEntry(mMsgContainer, Eigen::Vector2i(0, 1), false, false);

	// create the buttons
	mButtons.push_back(std::make_shared<ButtonComponent>(mWindow, name1, name1, std::bind(&GuiMsgBoxScroll::deleteMeAndCall, this, func1)));
	if(!name2.empty())
		mButtons.push_back(std::make_shared<ButtonComponent>(mWindow, name2, name3, std::bind(&GuiMsgBoxScroll::deleteMeAndCall, this, func2)));
	if(!name3.empty())
		mButtons.push_back(std::make_shared<ButtonComponent>(mWindow, name3, name3, std::bind(&GuiMsgBoxScroll::deleteMeAndCall, this, func3)));

	// set accelerator automatically (button to press when "b" is pressed)
	if(mButtons.size() == 1)
	{
		mAcceleratorFunc = mButtons.front()->getPressedFunc();
	}else{
		for(auto it = mButtons.begin(); it != mButtons.end(); it++)
		{
			if(strToUpper((*it)->getText()) == "OK" || strToUpper((*it)->getText()) == "NO")
			{
				mAcceleratorFunc = (*it)->getPressedFunc();
				break;
			}
		}
	}

	// put the buttons into a ComponentGrid
	mButtonGrid = makeButtonGrid(mWindow, mButtons);
	mGrid.setEntry(mButtonGrid, Eigen::Vector2i(0, 2), true, false, Eigen::Vector2i(1, 1));

	// decide final width
	if(mMsg->getSize().x() < width && mButtonGrid->getSize().x() < width)
	{
		// mMsg and buttons are narrower than width
		width = std::max(mButtonGrid->getSize().x(), mMsg->getSize().x());
		width = std::max(width, minWidth);
	}

	mMsg->setSize(width, 0);
	const float msgHeight = std::min(Renderer::getScreenHeight() * 0.5f, mMsg->getSize().y());
	mMsgContainer->setSize(width, msgHeight);
	setSize(width + HORIZONTAL_PADDING_PX*2, mButtonGrid->getSize().y() + msgHeight + mTitle->getSize().y());

	// center for good measure
	setPosition((Renderer::getScreenWidth() - mSize.x()) / 2, (Renderer::getScreenHeight() - mSize.y()) / 2);

	addChild(&mBackground);
	addChild(&mGrid);
}

bool GuiMsgBoxScroll::input(InputConfig* config, Input input)
{
	// special case for when GuiMsgBox comes up to report errors before anything has been configured
	if(config->getDeviceId() == DEVICE_KEYBOARD && !config->isConfigured() && input.value && 
		(input.id == SDLK_RETURN || input.id == SDLK_ESCAPE || input.id == SDLK_SPACE))
	{
		mAcceleratorFunc();
		return true;
	}

	/* when it's not configured, allow to remove the message box too to allow the configdevice window a chance */
	if(mAcceleratorFunc && ((config->isMappedTo("a", input) && input.value != 0) || (config->isConfigured() == false && input.type == TYPE_BUTTON)))
	{
		mAcceleratorFunc();
		return true;
	}

	return GuiComponent::input(config, input);
}

void GuiMsgBoxScroll::onSizeChanged()
{
	mGrid.setSize(mSize);
	mGrid.setRowHeightPerc(0, mTitle->getFont()->getHeight() / mSize.y());
	mGrid.setRowHeightPerc(2, mButtonGrid->getSize().y() / mSize.y());

	// update messagebox size
	mMsg->setSize(mMsgContainer->getSize().x(), 0); // make desc text wrap at edge of container
	mGrid.onSizeChanged();

	mBackground.fitTo(mSize, Eigen::Vector3f::Zero(), Eigen::Vector2f(-32, -32));
}

void GuiMsgBoxScroll::deleteMeAndCall(const std::function<void()>& func)
{
	auto funcCopy = func;
	delete this;

	if(funcCopy)
		funcCopy();

}

std::vector<HelpPrompt> GuiMsgBoxScroll::getHelpPrompts()
{
	return mGrid.getHelpPrompts();
}
