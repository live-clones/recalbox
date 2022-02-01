#include <guis/GuiTextEditPopup.h>
#include <components/MenuComponent.h>
#include <components/ButtonComponent.h>
#include <utils/locale/LocaleHelper.h>
#include <themes/MenuThemeData.h>

GuiTextEditPopup::GuiTextEditPopup(WindowManager& window, const std::string& title, const std::string& initValue,
                                   const std::function<void(const std::string&)>& okCallback, bool multiLine,
                                   const std::string& acceptBtnText)
	: Gui(window),
	  mBackground(window, Path(":/frame.png")),
	  mGrid(window, Vector2i(1, 3)),
	  mMultiLine(multiLine)
{
	
	auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
	
	mBackground.setImagePath(menuTheme->menuBackground.path);
	mBackground.setCenterColor(menuTheme->menuBackground.color);
	mBackground.setEdgeColor(menuTheme->menuBackground.color);
	
	addChild(&mBackground);
	addChild(&mGrid);

	mTitle = std::make_shared<TextComponent>(mWindow, Strings::ToUpperUTF8(title), menuTheme->menuTitle.font, menuTheme->menuTitle.color, TextAlignment::Center);

	mText = std::make_shared<TextEditComponent>(mWindow);
	mText->setValue(initValue);

	if(!multiLine)
		mText->setCursor(initValue.size());

	std::vector< std::shared_ptr<ButtonComponent> > buttons;
	buttons.push_back(std::make_shared<ButtonComponent>(mWindow, acceptBtnText, acceptBtnText, [this, okCallback] { okCallback(mText->getValue()); Close(); }));
	buttons.push_back(std::make_shared<ButtonComponent>(mWindow, _("CANCEL"), _("DISCARD CHANGES"), [this] { Close(); }));

	mButtonGrid = makeButtonGrid(mWindow, buttons);

	mGrid.setEntry(mTitle, Vector2i(0, 0), false, true);
	mGrid.setEntry(mText, Vector2i(0, 1), true, false, Vector2i(1, 1), Borders::Top | Borders::Bottom);
	mGrid.setEntry(mButtonGrid, Vector2i(0, 2), true, false);

	float textHeight = mText->getFont()->getHeight();
	if(multiLine)
		textHeight *= 6;
	mText->setSize(0, textHeight);

	setSize(Renderer::Instance().DisplayWidthAsFloat() * 0.5f, mTitle->getFont()->getHeight() + textHeight + mButtonGrid->getSize().y() + 40);
	setPosition((Renderer::Instance().DisplayWidthAsFloat() - mSize.x()) / 2, (Renderer::Instance().DisplayHeightAsFloat() - mSize.y()) / 2);
}

void GuiTextEditPopup::onSizeChanged()
{
	mBackground.fitTo(mSize, Vector3f::Zero(), Vector2f(-32, -32));

	mText->setSize(mSize.x() - 40, mText->getSize().y());

	// update grid
	mGrid.setRowHeightPerc(0, mTitle->getFont()->getHeight() / mSize.y());
	mGrid.setRowHeightPerc(2, mButtonGrid->getSize().y() / mSize.y());
	mGrid.setSize(mSize);
}

bool GuiTextEditPopup::ProcessInput(const InputCompactEvent& event)
{
	if (Component::ProcessInput(event))
		return true;

	// pressing back when not text editing closes us
	if (event.CancelPressed())
	{
		Close();
		return true;
	}

	return false;
}

bool GuiTextEditPopup::getHelpPrompts(Help& help)
{
  mGrid.getHelpPrompts(help);
	help.Set(Help::Cancel(), _("BACK"));
	return true;
}
