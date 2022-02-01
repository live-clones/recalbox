#include <guis/GuiMsgBox.h>
#include <Renderer.h>
#include <components/TextComponent.h>
#include <components/ButtonComponent.h>
#include <components/MenuComponent.h> // for makeButtonGrid
#include <utils/Log.h>
#include <themes/MenuThemeData.h>

#define HORIZONTAL_PADDING_PX 20

GuiMsgBox::GuiMsgBox(WindowManager& window)
  : Gui(window),
    mBackground(window, Path(":/frame.png")),
    mGrid(window, Vector2i(1, 2))
{
}

GuiMsgBox::GuiMsgBox(WindowManager& window, const std::string& text,
                     const std::string& name1, const std::function<void()>& func1,
                     const std::string& name2, const std::function<void()>& func2,
                     const std::string& name3, const std::function<void()>& func3,
                     TextAlignment align)
  : GuiMsgBox(window)
{
  build(text, align, name1, func1, name2, func2, name3, func3);
}

GuiMsgBox::GuiMsgBox(WindowManager& window, const std::string& text,
                     const std::string& name1, const std::function<void()>& func1,
                     const std::string& name2, const std::function<void()>& func2)
  : GuiMsgBox(window)
{
  build(text, TextAlignment::Center, name1, func1, name2, func2, std::string(), nullptr);
}

GuiMsgBox::GuiMsgBox(WindowManager& window, const std::string& text,
                     const std::string& name1, const std::function<void()>& func1)
  : GuiMsgBox(window)
{
  build(text, TextAlignment::Center, name1, func1, std::string(), nullptr, std::string(), nullptr);
}

GuiMsgBox::GuiMsgBox(WindowManager& window, const std::string& text,
                     const std::string& name1, TextAlignment align)
  : GuiMsgBox(window)
{
  build(text, align, name1, nullptr, std::string(), nullptr, std::string(), nullptr);
}

GuiMsgBox::GuiMsgBox(WindowManager& window, const std::string& text,
                     const std::string& name1)
  : GuiMsgBox(window)
{
  build(text, TextAlignment::Center, name1, nullptr, std::string(), nullptr, std::string(), nullptr);
}

GuiMsgBox::GuiMsgBox(WindowManager& window, const std::string& text)
  : GuiMsgBox(window)
{
  build(text, TextAlignment::Center, "OK", nullptr, std::string(), nullptr, std::string(), nullptr);
}

void GuiMsgBox::build(const std::string& text, TextAlignment align,
                 const std::string& name1, const std::function<void()>& func1,
                 const std::string& name2, const std::function<void()>& func2,
                 const std::string& name3, const std::function<void()>& func3)
{
	float width = Renderer::Instance().DisplayWidthAsFloat() * 0.7f; // max width
	float minWidth = Renderer::Instance().DisplayWidthAsFloat() * 0.4f; // minimum width
	
	auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
	
	mBackground.setImagePath(menuTheme->menuBackground.path);
	mBackground.setCenterColor(menuTheme->menuBackground.color);
	mBackground.setEdgeColor(menuTheme->menuBackground.color);

	mMsg = std::make_shared<TextComponent>(mWindow, text, menuTheme->menuText.font, menuTheme->menuText.color, align);
	mGrid.setEntry(mMsg, Vector2i(0, 0), false, false);

	// create the buttons
	mButtons.push_back(std::make_shared<ButtonComponent>(mWindow, name1, name1, std::bind(&GuiMsgBox::CloseAndCall, this, func1)));
	if(!name2.empty())
		mButtons.push_back(std::make_shared<ButtonComponent>(mWindow, name2, name2, std::bind(&GuiMsgBox::CloseAndCall, this, func2)));
	if(!name3.empty())
		mButtons.push_back(std::make_shared<ButtonComponent>(mWindow, name3, name3, std::bind(&GuiMsgBox::CloseAndCall, this, func3)));

	// set accelerator automatically (button to press when "b" is pressed)
	if(mButtons.size() == 1)
	{
		mAcceleratorFunc = mButtons.front()->getPressedFunc();
	}
	else
	{
		for (auto& button : mButtons)
			if(Strings::ToUpperASCII(button->getText()) == "OK" || Strings::ToUpperASCII(button->getText()) == "NO")
			{
				mAcceleratorFunc = button->getPressedFunc();
				break;
			}
	}

	// put the buttons into a ComponentGrid
	mButtonGrid = makeButtonGrid(mWindow, mButtons);
	mGrid.setEntry(mButtonGrid, Vector2i(0, 1), true, false, Vector2i(1, 1));

	// decide final width
	if(mMsg->getSize().x() < width && mButtonGrid->getSize().x() < width)
	{
		// mMsg and buttons are narrower than width
		width = Math::max(mButtonGrid->getSize().x(), mMsg->getSize().x());
		width = Math::max(width, minWidth);
	}

	// now that we know width, we can find height
	mMsg->setSize(width, 0); // mMsg->getSize.y() now returns the proper length
	const float msgHeight = Math::max(Font::get(FONT_SIZE_LARGE)->getHeight(), mMsg->getSize().y() * 1.225f);
	setSize(width + HORIZONTAL_PADDING_PX*2, msgHeight + mButtonGrid->getSize().y());

	// center for good measure
	setPosition((Renderer::Instance().DisplayWidthAsFloat() - mSize.x()) / 2.0f, (Renderer::Instance().DisplayHeightAsFloat() - mSize.y()) / 2.0f);

	addChild(&mBackground);
	addChild(&mGrid);
}

bool GuiMsgBox::ProcessInput(const InputCompactEvent& event)
{
	// special case for when GuiMsgBox comes up to report errors before anything has been configured
	if (mAcceleratorFunc && event.AskForConfiguration())
	{
		mAcceleratorFunc();
		return true;
	}

	return Component::ProcessInput(event);
}

void GuiMsgBox::onSizeChanged()
{
	mGrid.setSize(mSize);
	mGrid.setRowHeightPerc(1, mButtonGrid->getSize().y() / mSize.y());
	
	// update messagebox size
	mMsg->setSize(mSize.x() - HORIZONTAL_PADDING_PX*2, mGrid.getRowHeight(0));
	mGrid.onSizeChanged();

	mBackground.fitTo(mSize, Vector3f::Zero(), Vector2f(-32, -32));
}

void GuiMsgBox::CloseAndCall(const std::function<void()>& func)
{
	Close();
	if (func) func();
}

GuiMsgBox* GuiMsgBox::SetDefaultButton(int index)
{
  if ((unsigned int)index < (unsigned int)mButtons.size())
    mButtonGrid->setCursorTo(mButtons[index]);

  return this;
}

