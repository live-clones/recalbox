#include <guis/GuiMsgBoxScroll.h>
#include <Renderer.h>
#include <components/TextComponent.h>
#include <components/ButtonComponent.h>
#include <components/MenuComponent.h> // for makeButtonGrid
#include <components/ScrollableContainer.h>

#define HORIZONTAL_PADDING_PX 20

GuiMsgBoxScroll::GuiMsgBoxScroll(WindowManager& window,
                                 const std::string& title, const std::string& text,
                                 const std::string& name1, const std::function<void()>& func1,
                                 const std::string& name2, const std::function<void()>& func2,
                                 const std::string& name3, const std::function<void()>& func3,
                                 TextAlignment align, float height)
  : Gui(window),
    mBackground(window, Path(":/frame.png")),
    mGrid(window, Vector2i(1, 3))
{
	(void)height;

	float width = Renderer::Instance().DisplayWidthAsFloat() * 0.8f; // max width
	float minWidth = Renderer::Instance().DisplayWidthAsFloat() * 0.4f; // minimum width


	auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();

	mBackground.setImagePath(menuTheme->menuBackground.path);
	mBackground.setCenterColor(menuTheme->menuBackground.color);
	mBackground.setEdgeColor(menuTheme->menuBackground.color);

	mTitle = std::make_shared<TextComponent>(mWindow, title, menuTheme->menuTitle.font, menuTheme->menuTitle.color, TextAlignment::Center);
	mGrid.setEntry(mTitle, Vector2i(0, 0), false, true);

	mMsg = std::make_shared<TextComponent>(mWindow, text, menuTheme->menuTextSmall.font, menuTheme->menuTextSmall.color, align);

	mMsgContainer = std::make_shared<ScrollableContainer>(mWindow);
	mMsgContainer->setAutoScroll(true);
	mMsgContainer->mAutoScrollSpeed = 20; // ms between scrolls
	mMsgContainer->mAutoScrollDelay = 3000; // ms to wait before we start to scroll
	mMsgContainer->mAutoScrollResetAccumulator = 5000; // ms to reset to top after we reach the bottom
	mMsgContainer->addChild(mMsg.get());

	mGrid.setEntry(mMsgContainer, Vector2i(0, 1), false, false);

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
		for (auto& mButton : mButtons)
		{
			if(Strings::ToUpperASCII(mButton->getText()) == "OK" || Strings::ToUpperASCII(mButton->getText()) == "NO")
			{
				mAcceleratorFunc = mButton->getPressedFunc();
				break;
			}
		}
	}

	// put the buttons into a ComponentGrid
	mButtonGrid = makeButtonGrid(mWindow, mButtons);
	mGrid.setEntry(mButtonGrid, Vector2i(0, 2), true, false, Vector2i(1, 1));

	// decide final width
	if(mMsg->getSize().x() < width && mButtonGrid->getSize().x() < width)
	{
		// mMsg and buttons are narrower than width
		width = Math::max(mButtonGrid->getSize().x(), mMsg->getSize().x());
		width = Math::max(width, minWidth);
	}

	mMsg->setSize(width, 0);
	const float msgHeight = Math::min(Renderer::Instance().DisplayHeightAsFloat() * 0.5f, mMsg->getSize().y());
	mMsgContainer->setSize(width, msgHeight);
	setSize(width + HORIZONTAL_PADDING_PX*2, mButtonGrid->getSize().y() + msgHeight + mTitle->getSize().y());

	// center for good measure
	setPosition((Renderer::Instance().DisplayWidthAsFloat() - mSize.x()) / 2, (Renderer::Instance().DisplayHeightAsFloat() - mSize.y()) / 2);

	addChild(&mBackground);
	addChild(&mGrid);
}

bool GuiMsgBoxScroll::ProcessInput(const InputCompactEvent& event)
{
	// special case for when GuiMsgBox comes up to report errors before anything has been configured
	/* when it's not configured, allow to remove the message box too to allow the configdevice window a chance */
	if(mAcceleratorFunc && event.AskForConfiguration())
	{
		mAcceleratorFunc();
		return true;
	}

	return Component::ProcessInput(event);
}

void GuiMsgBoxScroll::onSizeChanged()
{
	mGrid.setSize(mSize);
	mGrid.setRowHeightPerc(0, mTitle->getFont()->getHeight() / mSize.y());
	mGrid.setRowHeightPerc(2, mButtonGrid->getSize().y() / mSize.y());

	// update messagebox size
	mMsg->setSize(mMsgContainer->getSize().x(), 0); // make desc text wrap at edge of container
	mGrid.onSizeChanged();

	mBackground.fitTo(mSize, Vector3f::Zero(), Vector2f(-32, -32));
}

void GuiMsgBoxScroll::deleteMeAndCall(const std::function<void()>& func)
{
	Close();
	if(func) func();
}

GuiMsgBoxScroll* GuiMsgBoxScroll::SetDefaultButton(int index)
{
  if ((unsigned int)index < (unsigned int)mButtons.size())
    mButtonGrid->setCursorTo(mButtons[index]);

  return this;
}

