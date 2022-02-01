#include <guis/GuiTextEditPopupKeyboard.h>
#include <components/MenuComponent.h>
#include <themes/MenuThemeData.h>
#include <RecalboxConf.h>
#include <utils/locale/LocaleHelper.h>

GuiTextEditPopupKeyboard::GuiTextEditPopupKeyboard(WindowManager&window, const std::string& title, const std::string& initValue,
                                                   const std::function<void(const std::string&)>& okCallback, bool multiLine, const std::string& acceptBtnText)
	: Gui(window),
	  mBackground(window, Path(":/frame.png")),
	  mGrid(window, Vector2i(1, 4)),
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

	if (!mMultiLine)
		mText->setCursor(initValue.size());

	// Header
	mGrid.setEntry(mTitle, Vector2i(0, 0), false, true);

	// Text edit add
	mGrid.setEntry(mText, Vector2i(0, 1), true, false, Vector2i(1, 1));

	std::vector< std::vector< std::shared_ptr<ButtonComponent> > > buttonList;

	// Keyboard
	// Case for if multiline is enabled, then don't create the keyboard.
	if (!mMultiLine) {

		std::vector<std::array<std::string, 12> > lines;

		std::string keyboard = RecalboxConf::Instance().AsString("system.kblayout");

		if (keyboard == "fr")
		{
			lines.push_back({ "&", "é", "\"", "'", "(", "§", "è", "!", "ç", "à", ")", "-" });
			lines.push_back({ "1", "2", "3",  "4", "5", "6", "7", "8", "9", "0", "°", "_" });

			lines.push_back({ "à", "ä", "ë", "ì", "ï", "ò", "ö", "ü", "\\", "|", "#", "@" });
			lines.push_back({ "á", "â", "ê", "í", "î", "ó", "ô", "ú",  "û", "ñ", "¡", "¿" });

			lines.push_back({ "a", "z", "e", "r", "t", "y", "u", "i", "o", "p", "^", "$" });
			lines.push_back({ "A", "Z", "E", "R", "T", "Y", "U", "I", "O", "P", "¨", "*" });

			lines.push_back({ "q", "s", "d", "f", "g", "h", "j", "k", "l", "m", "ù", "`" });
			lines.push_back({ "Q", "S", "D", "F", "G", "H", "J", "K", "L", "M", "%", "£" });

			// Special case, SHIFT key at position 0
			lines.push_back({ "", "<", "w", "x", "c", "v", "b", "n", ",", ";", ":", "=" });
			lines.push_back({ "", ">", "W", "X", "C", "V", "B", "N", "?", ".", "/", "+" });
		}
		else
		{
			lines.push_back({ "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "_", "+" });
			lines.push_back({ "!", "@", "#", "$", "%", "^", "&", "*", "(", ")", "-", "=" });

			lines.push_back({ "à", "ä", "è", "ë", "ì", "ï", "ò", "ö", "ù", "ü", "¨", "¿" });
			lines.push_back({ "á", "â", "é", "ê", "í", "î", "ó", "ô", "ú", "û", "ñ", "¡" });

			lines.push_back({ "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "{", "}" });
			lines.push_back({ "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "[", "]" });

			lines.push_back({ "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "\"", "|" });
			lines.push_back({ "A", "S", "D", "F", "G", "H", "J", "K", "L", ":", "'", "\\" });

			// Special case, SHIFT key at position 0
			lines.push_back({ "", "~", "z", "x", "c", "v", "b", "n", "m", ",", ".", "?" });
			lines.push_back({ "", "`", "Z", "X", "C", "V", "B", "N", "M", "<", ">", "/" });
		}

	    for (unsigned int i = 0; i < 5; i++)
	    {
	      std::vector< std::shared_ptr<ButtonComponent> > buttons;
        for (unsigned int j = 0; j < 12; j++)
        {
          if (i == 4 && j == 0)
          {
            // Special case for shift key
            mShiftButton = std::make_shared<ButtonComponent>(mWindow, "SHIFT", _("SHIFTS FOR UPPER,LOWER, AND SPECIAL"), [this] {
              switchShift();
            });
            buttons.push_back(mShiftButton);
          }
          else buttons.push_back(makeButton(lines[2 * i][j], lines[2 * i + 1][j]));
        }
			  buttonList.push_back(buttons);
	    }
	}

	const float screenHeightAvailable = Renderer::Instance().DisplayHeightAsFloat() - HelpItemStyle().TextFont()->getHeight(); // Height - Help Height
  const float gridWidth = Renderer::Instance().DisplayWidthAsFloat() * 0.98f;
  const float gridHeight = screenHeightAvailable * ((float) buttonList.size() / (float) (buttonList.size() + 3) ); // 3 => share space with mTitle + mText + buttons

	// Add keyboard keys
	mKeyboardGrid = makeMultiDimButtonGrid(mWindow, buttonList, gridWidth, gridHeight);

	mShiftButton->autoSizeFont();
	if (mShiftButton->isTextOverlapping()) {
		mShiftButton->setText("SH.", _("SHIFTS FOR UPPER,LOWER, AND SPECIAL"), false, false);
	}

	mGrid.setEntry(mKeyboardGrid, Vector2i(0, 2), true, false);

	std::vector< std::shared_ptr<ButtonComponent> > buttons;

	buttons.push_back(std::make_shared<ButtonComponent>(mWindow, acceptBtnText, acceptBtnText, [this, okCallback] { okCallback(mText->getValue()); Close(); }));
	buttons.push_back(std::make_shared<ButtonComponent>(mWindow, _("SPACE"), _("SPACE"), [this] {
		mText->startEditing();
		mText->textInput(" ");
		mText->stopEditing();
	}));
	buttons.push_back(std::make_shared<ButtonComponent>(mWindow, _("DELETE"), _("DELETE A CHAR"), [this] {
		mText->startEditing();
		mText->textInput("\b");
		mText->stopEditing();
	}));
	buttons.push_back(std::make_shared<ButtonComponent>(mWindow, _("CANCEL"), _("DISCARD CHANGES"), [this] { Close(); }));

	// Add buttons
	mButtons = makeButtonGrid(mWindow, buttons);
	mGrid.setEntry(mButtons, Vector2i(0, 3), true, false);

	// Determine size from text size
	float textHeight = mText->getFont()->getHeight();
	if (multiLine)
		textHeight *= 6;
	mText->setSize(gridWidth - 40, textHeight);

	// Determine available free space to get a more airy grid
	float height = mTitle->getFont()->getHeight() + textHeight + mKeyboardGrid->getSize().y() + (mMultiLine ? 0 : mButtons->getSize().y());
	float padding = Math::min(40.0f, (screenHeightAvailable - height) * 0.75f);

	// If multiline, set all size back to default, else draw size for keyboard.
	if (mMultiLine)
		setSize(Renderer::Instance().DisplayWidthAsFloat() * 0.75f, height + padding);
	else
		setSize(gridWidth, height + padding);

	setPosition((Renderer::Instance().DisplayWidthAsFloat() - mSize.x()) / 2, (screenHeightAvailable - mSize.y()) / 2);
}

std::shared_ptr<ButtonComponent> GuiTextEditPopupKeyboard::makeButton(const std::string& key, const std::string& shiftedKey)
{
    std::shared_ptr<ButtonComponent> button = std::make_shared<ButtonComponent>(mWindow, key, key, [this, key, shiftedKey] {
        mText->startEditing();
        if (mShift)
        	mText->textInput(shiftedKey.c_str());
        else
        	mText->textInput(key.c_str());
        mText->stopEditing();
    }, false);
    KeyboardButton kb(button, key, shiftedKey);
    keyboardButtons.push_back(kb);
    return button;
}

void GuiTextEditPopupKeyboard::onSizeChanged()
{
	mBackground.fitTo(mSize, Vector3f::Zero(), Vector2f(-32, -32));

	mText->setSize(mSize.x() - 40, mText->getSize().y());

	float fullHeight = mTitle->getFont()->getHeight() + mText->getSize().y() + mKeyboardGrid->getSize().y() + (mMultiLine ? 0 : mButtons->getSize().y());

	// update grid
	mGrid.setRowHeightPerc(0, mTitle->getFont()->getHeight() / fullHeight);
	mGrid.setRowHeightPerc(1, mText->getSize().y() / fullHeight);
	mGrid.setRowHeightPerc(2, mKeyboardGrid->getSize().y() / fullHeight);
	mGrid.setRowHeightPerc(3, mMultiLine ? 0 : mButtons->getSize().y() / fullHeight);

	mGrid.setSize(mSize);

	mKeyboardGrid->onSizeChanged();
}

bool GuiTextEditPopupKeyboard::ProcessInput(const InputCompactEvent& event)
{
	if (Component::ProcessInput(event))
		return true;

	// pressing back when not text editing closes us
	if (event.CancelPressed())
	{
		Close();
		return true;
	}

	// For deleting a chara (Left Top Button)
	if (event.L1Pressed())
	{
		mText->startEditing();
		mText->textInput("\b");
		mText->stopEditing();
	}

	// For Adding a space (Right Top Button)
	if (event.R1Pressed())
	{
		mText->startEditing();
		mText->textInput(" ");
		mText->stopEditing();
	}

	// For Shifting (Y)
	if (event.YPressed())
		switchShift();

	return false;
}

// Shifts the keys when user hits the shift button.
void GuiTextEditPopupKeyboard::switchShift()
{
	mShift = !mShift;
	if (mShift)	mShiftButton->setColorShift(0xFF0000FF);
	else    		mShiftButton->removeColorShift();

  for (auto & kb: keyboardButtons)
  {
    const std::string& text = mShift ? kb.shiftedKey : kb.key;
    kb.button->setText(text, text, false, false, false);
  }

  updateHelpPrompts();
}

bool GuiTextEditPopupKeyboard::getHelpPrompts(Help& help)
{
	mGrid.getHelpPrompts(help);
	help.Set(HelpType::Y, _("SHIFT"))
	    .Set(Help::Cancel(), _("BACK"))
	    .Set(HelpType::R, _("SPACE"))
	    .Set(HelpType::L, _("DELETE"));
	return true;
}

