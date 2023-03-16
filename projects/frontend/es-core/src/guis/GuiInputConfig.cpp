#include <utils/locale/LocaleHelper.h>
#include <guis/GuiInputConfig.h>
#include <WindowManager.h>
#include <utils/Log.h>
#include <components/TextComponent.h>
#include <components/ImageComponent.h>
#include <components/MenuComponent.h>
#include <components/ButtonComponent.h>
#include <themes/MenuThemeData.h>

GuiInputConfig::~GuiInputConfig()
{
  mTargetDevice->SetConfiguringState(false);
}

GuiInputConfig::GuiInputConfig(WindowManager&window, InputDevice* target, const std::function<void()>& doneCallback)
  : Gui(window)
  , mBackground(window, Path(":/frame.png"))
  , mGrid(window, Vector2i(1, 5))
  , mTargetDevice(target)
  , mActiveButtons(0)
  , mActiveAxis(0)
  , mActiveHats(0)
  , mNeutralPositionSet(false)
  , mCursorOnList(true)
{
  mTargetDevice->SetConfiguringState(true);
  { LOG(LogInfo) << "[GuiInput] Configuring device " << mTargetDevice->Index() << " (" << mTargetDevice->Name() << ")."; }

	initFormInputs();

	InputDevice previousConfig(*mTargetDevice);
  mTargetDevice->ClearAll();
	
	auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
	
	mBackground.setImagePath(menuTheme->menuBackground.path);
	mBackground.setCenterColor(menuTheme->menuBackground.color);
	mBackground.setEdgeColor(menuTheme->menuBackground.color);
	
	mMainColor = menuTheme->menuText.color;

	addChild(&mBackground);
	addChild(&mGrid);

	mTitle = std::make_shared<TextComponent>(mWindow, _("CONFIGURING"), menuTheme->menuTitle.font, menuTheme->menuTitle.color, TextAlignment::Center);
	mGrid.setEntry(mTitle, Vector2i(0, 0), false, true);

	std::string deviceName = _("KEYBOARD");
	if (mTargetDevice->Identifier() != InputEvent::sKeyboardDevice)
  {
	  deviceName = _("GAMEPAD %i");
	  Strings::ReplaceAllIn(deviceName, "%i", Strings::ToString(mTargetDevice->Index() + 1));
  }

	mSubtitle1 = std::make_shared<TextComponent>(mWindow, Strings::ToUpperUTF8(deviceName), menuTheme->menuText.font, menuTheme->menuFooter.color, TextAlignment::Center);
	mGrid.setEntry(mSubtitle1, Vector2i(0, 1), false, true);

	mSubtitle2 = std::make_shared<TextComponent>(mWindow, "", menuTheme->menuTextSmall.font, menuTheme->menuTextSmall.color, TextAlignment::Center);
	mGrid.setEntry(mSubtitle2, Vector2i(0, 2), false, true);

	mList = std::make_shared<ComponentList>(mWindow);
	mGrid.setEntry(mList, Vector2i(0, 3), true, true);

	int index = -1;
	for (auto& formInput: mFormInputs)
	{
		ComponentListRow row;
		// icon
		auto icon = std::make_shared<ImageComponent>(mWindow);
		icon->setImage(formInput.icon);
		icon->setColorShift(menuTheme->menuText.color);
		icon->setResize(0, menuTheme->menuText.font->getLetterHeight() * 1.25f);
		row.addElement(icon, false);

		// spacer between icon and text
		auto spacer = std::make_shared<Component>(mWindow);
		spacer->setSize(16, 0);
		row.addElement(spacer, false);

		auto text = std::make_shared<TextComponent>(mWindow, formInput.label, menuTheme->menuText.font, menuTheme->menuText.color);
		row.addElement(text, true);

		auto mapping = std::make_shared<TextComponent>(mWindow, "", menuTheme->menuText.font, menuTheme->menuText.color, TextAlignment::Right);
		row.addElement(mapping, true);
		mMappings.push_back(mapping);

		row.SetEventInterceptor(-index, this);

		mList->addRow(row);
	}

	mList->setCursorChangedCallback([this](CursorState state)
	{
		(void)state;
		setHelpMessage();
	});

	// when lefting the "OK" button, back to the mList items by clicking the UP on the gamepad, the SDL may send more than one event 
	// (SDL_JOYBUTTONDOWN and SDL_JOYHATMOTION).
	// The first one is used by the window manager to get back the focus on the mList items, but the 
	// other one(s) will be sent to the mList last item handler, so, we have to swallow this kind of "echo"
	// during a small empiric delay
	mList->setFocusGainedCallback([this]() {
		mCursorOnList = true;
		auto cursorChange = mList->getCursorChangedCallback();
		cursorChange(CursorState::Stopped);
		setPress();
	});

	mList->setFocusLostCallback([this]() {
		mCursorOnList = false;
		auto cursorChange = mList->getCursorChangedCallback();
		cursorChange(CursorState::Stopped);
		mSubtitle2->setText("");
	});

	setHelpMessage();
	setPress();

	// buttons
	std::vector< std::shared_ptr<ButtonComponent> > buttons;

	buttons.push_back(std::make_shared<ButtonComponent>(mWindow, _("OK"), _("OK"), [this, doneCallback] {
    InputManager::WriteDeviceXmlConfiguration(*mTargetDevice); // save
		if(doneCallback)
			doneCallback();
		Close();
	}));

	buttons.push_back(std::make_shared<ButtonComponent>(mWindow, _("CANCEL"), _("CANCEL"), [this, previousConfig, doneCallback] {
    mTargetDevice->LoadFrom(previousConfig); // restaure previous target configuration
		if(doneCallback)
			doneCallback();
		Close();
	}));

	mButtonGrid = makeButtonGrid(mWindow, buttons);
	mGrid.setEntry(mButtonGrid, Vector2i(0, 4), true, false);
	float width = Renderer::Instance().Is480pOrLower() ? Renderer::Instance().DisplayWidthAsFloat() * 0.80f : Renderer::Instance().DisplayWidthAsFloat() * 0.60f;
	float height = Renderer::Instance().Is480pOrLower() ? Renderer::Instance().DisplayHeightAsFloat() * 0.70f : Renderer::Instance().DisplayHeightAsFloat() * 0.85f;
	setSize(width, height);
	setPosition((Renderer::Instance().DisplayWidthAsFloat() - mSize.x()) / 2, (Renderer::Instance().DisplayHeightAsFloat() - mSize.y()) / 2);
}

void GuiInputConfig::initFormInputs()
{
	addFormInput("Up", _("UP"), ":/help/dpad_up.svg", false, InputEvent::EventType::Hat);
	addFormInput("Down", _("DOWN"), ":/help/dpad_down.svg", false, InputEvent::EventType::Hat);
	addFormInput("Left", _("LEFT"), ":/help/dpad_left.svg", false, InputEvent::EventType::Hat);
	addFormInput("Right", _("RIGHT"), ":/help/dpad_right.svg", false, InputEvent::EventType::Hat);

	addFormInput("Joystick1Up", _("JOYSTICK 1 UP"), ":/help/joystick_up.svg", true, InputEvent::EventType::Axis);
	addFormInput("Joystick1Left", _("JOYSTICK 1 LEFT"), ":/help/joystick_left.svg", true, InputEvent::EventType::Axis);
	addFormInput("Joystick2Up", _("JOYSTICK 2 UP"), ":/help/joystick_up.svg", true, InputEvent::EventType::Axis);
	addFormInput("Joystick2Left", _("JOYSTICK 2 LEFT"), ":/help/joystick_left.svg", true, InputEvent::EventType::Axis);
	
	addFormInput("A", "A", ":/help/button_a.svg", false, InputEvent::EventType::Button);
	addFormInput("B", "B", ":/help/button_b.svg", false, InputEvent::EventType::Button);
	
	addFormInput("X", "X", ":/help/button_x.svg", true, InputEvent::EventType::Button);
	addFormInput("Y", "Y", ":/help/button_y.svg", true, InputEvent::EventType::Button);
	
	addFormInput("Start", "START", ":/help/button_start.svg", false, InputEvent::EventType::Button);
	addFormInput("Select", "SELECT", ":/help/button_select.svg", false, InputEvent::EventType::Button);
	
	addFormInput("L1", "L1", ":/help/button_l.svg", true, InputEvent::EventType::Button);
	addFormInput("R1", "R1", ":/help/button_r.svg", true, InputEvent::EventType::Button);
	
	addFormInput("L2", "L2", ":/help/button_l2.svg", true, InputEvent::EventType::Button);
	addFormInput("R2", "R2", ":/help/button_r2.svg", true, InputEvent::EventType::Button);
	
	addFormInput("L3", "L3", ":/help/button_l3.svg", true, InputEvent::EventType::Button);
	addFormInput("R3", "R3", ":/help/button_r3.svg", true, InputEvent::EventType::Button);

	if (Board::Instance().HasPhysicalVolumeButtons())
  {
    addFormInput("VOL-", _("VOLUME -"), ":/help/volume_down.svg", true, InputEvent::EventType::Button);
    addFormInput("VOL+", _("VOLUME +"), ":/help/volume_up.svg", true, InputEvent::EventType::Button);
  }

	if (Board::Instance().HasPhysicalBrightnessButtons())
  {
    addFormInput("LUM-", _("BRIGHTNESS -"), ":/help/brightness_down.svg", true, InputEvent::EventType::Button);
    addFormInput("LUM+", _("BRIGHTNESS +"), ":/help/brightness_up.svg", true, InputEvent::EventType::Button);
  }

  addFormInput("HotKey", _("HOTKEY"), ":/help/button_hotkey.svg", false, InputEvent::EventType::Button);
}

void GuiInputConfig::addFormInput(const char* name, const std::string& label, const char* icon, bool skippable, InputEvent::EventType preferredType)
{
	FormInput formInput(name, label, Path(icon), skippable, preferredType);
	mFormInputs.push_back(formInput);
}

void GuiInputConfig::onSizeChanged()
{
	mBackground.fitTo(mSize, Vector3f::Zero(), Vector2f(-32, -32));

	// update grid
	mGrid.setSize(mSize);

	mGrid.setRowHeightPerc(0, mTitle->getFont()->getHeight() / mSize.y());
	mGrid.setRowHeightPerc(1, mSubtitle1->getFont()->getHeight() / mSize.y());
	mGrid.setRowHeightPerc(2, mSubtitle2->getFont()->getHeight() / mSize.y());
	mGrid.setRowHeightPerc(4, mButtonGrid->getSize().y() / mSize.y());
}

// move cursor to the next thing if we're configuring all, 
// or come out of "configure mode" if we were only configuring one row
void GuiInputConfig::rowDone()
{
	if(!mList->moveCursor(1))
  { // try to move to the next one
		mCursorOnList = false;
		mGrid.moveCursor(Vector2i(0, 1));
	}
	else
		setPress(); // on another one
}

void GuiInputConfig::setText(const std::string& msg, unsigned int color) {
	setText(msg, color, mList->getCursorId());
}

void GuiInputConfig::setText(const std::string& msg, unsigned int color, const int inputId) {
	std::shared_ptr<TextComponent>& text = mMappings[inputId];
	text->setText(Strings::ToUpperUTF8(msg));
	text->setColor(color);
}

void GuiInputConfig::setHelpMessage()
{
	std::string msg;
	InputEvent input;
	int inputId = mList->getCursorId();
	FormInput formInput = mFormInputs[inputId];
	bool assigned = mTargetDevice->GetEntryConfiguration(InputDevice::StringToEntry(formInput.name), input);
	//std::shared_ptr<TextComponent>& text = mMappings[inputId];
	auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();

	if (assigned)
  {
    if (RecalboxConf::Instance().GetSwapValidateAndCancel())
    {
      if (mTargetDevice->IsSet(InputDevice::Entry::B)) msg = msg + _("B TO UNSET");
    }
    else
    {
      if (mTargetDevice->IsSet(InputDevice::Entry::A)) msg = msg + _("A TO UNSET");
    }
		if (mTargetDevice->IsSet(InputDevice::Entry::Down))
    {
			if (inputId == 0) msg = (msg.length() != 0u ? msg + " - " : "") + _("DOWN TO SKIP");
			else              msg = (msg.length() != 0u ? msg + " - " : "") + _("UP/DOWN TO SKIP");
		}
	}
  else if (formInput.skippable)	msg = _("UP/DOWN TO SKIP");
	else                          msg = _("INPUT REQUIRED");

	mSubtitle2->setText(msg);
	mSubtitle2->setColor(formInput.skippable || assigned ? menuTheme->menuTextSmall.color : 0xff4141ff);
}

void GuiInputConfig::setPress() {
	InputEvent input;
	FormInput formInput = mFormInputs[mList->getCursorId()];
	if (mTargetDevice->GetEntryConfiguration(InputDevice::StringToEntry(formInput.name), input)) {
		setAssignedTo(input);
	} else {
		setText(_("PRESS ANYTHING"), mMainColor); 
	}
}

bool GuiInputConfig::assign(InputEvent input)
{
	FormInput formInput = mFormInputs[mList->getCursorId()];

	// input is from InputConfig* mTargetDevice
	// if this input is mapped to something other than "nothing" or the current row, error
	// (if it's the same as what it was before, allow it)
	if (std::string("HotKey") != formInput.name &&
      mTargetDevice->GetMatchedEntry(input) != InputDevice::Entry::None &&
      !mTargetDevice->IsMatching(InputDevice::StringToEntry(formInput.name), input))
	{
		setMapped();
		return false;
	}

  // Trigger axis are required to be positive
  if ((formInput.name == "L2") || (formInput.name == "R2"))
    if ((input.Type() == InputEvent::EventType::Axis) && (input.Value() < 0))
      input = InputEvent(input.Device(), input.Type(), input.Id(), -input.Value(), input.Code());

	setAssignedTo(input);
	
	// set a code while the device is still connected
  input.StoreSDLCode(mTargetDevice->Index());
  mTargetDevice->Set(InputDevice::StringToEntry(formInput.name), input);

  { LOG(LogInfo) << "[GuiInput]   Mapping [" << input.ToString() << "] -> " << formInput.name; }

	return true;
}

void GuiInputConfig::unAssign() {
	FormInput formInput = mFormInputs[mList->getCursorId()];

	InputEvent input;
  mTargetDevice->GetEntryConfiguration(InputDevice::StringToEntry(formInput.name), input);

	setNotDefined();
	
  mTargetDevice->Unset(InputDevice::StringToEntry(formInput.name));

  { LOG(LogInfo) << "[GuiInput]   Unmapping [" << input.ToString() << "] -> " << formInput.name; }
}

void GuiInputConfig::restaurePreviousAssignment()
{
	InputEvent input;
	FormInput formInput = mFormInputs[mList->getCursorId()];
	if(mTargetDevice->GetEntryConfiguration(InputDevice::StringToEntry(formInput.name), input))
		setAssignedTo(input);
	else
		setNotDefined();
}

bool GuiInputConfig::isAssigned()
{
	FormInput formInput = mFormInputs[mList->getCursorId()];
	return mTargetDevice->IsSet(InputDevice::StringToEntry(formInput.name));
}

bool GuiInputConfig::EventReceived(int id, const InputCompactEvent& event)
{
  (void)id;

  // ignore input not from our target device
  if(&event.Device() != mTargetDevice)
    return false;

  // Neutral position has been reached?
  if (!mNeutralPositionSet)
  {
    if (mTargetDevice->CheckNeutralPosition()) mNeutralPositionSet = true;
    return true;
  }

  // Record
  RecordRawInput(event.RawEvent());
  if (!mEventList.empty() && NeutralPosition())
    ProcessEvents();

  /*if (event.RawEvent().AnythingPressed()) // Something was pressed/moved
    if (!mInputStack.hasInput(event.RawEvent()))
      mInputStack.push(event.RawEvent());*/

  return true;
}

bool GuiInputConfig::getHelpPrompts(Help &help)
{
    help.Set(Help::Cancel(), _("CANCEL"));

    return true;
}

void GuiInputConfig::ProcessEvents()
{
  const FormInput& formInput = mFormInputs[mList->getCursorId()];

  std::vector<InputEvent> events = std::move(mEventList);
  mEventList.clear();

  for (auto input : events)
  {
    // Key Up
    if (mTargetDevice->IsMatching(InputDevice::Entry::Up, input))
    {
      if (mList->getCursorId() > 0 && mTargetDevice->IsSet(InputDevice::Entry::Down))
      {
        restaurePreviousAssignment();
        if (!isAssigned() && formInput.skippable) setSkipped();
        mList->moveCursor(-1);
        setPress();
      }
      return;
    }

    // Key Down
    if (mTargetDevice->IsMatching(InputDevice::Entry::Down, input))
    {
      bool assigned = isAssigned();
      bool skippable = formInput.skippable;
      if (!assigned && !skippable) return;
      restaurePreviousAssignment();
      if (!isAssigned()) setSkipped();
      rowDone();
      return;
    }

    if (isAssigned() && mTargetDevice->IsMatching((RecalboxConf::Instance().GetSwapValidateAndCancel() ? InputDevice::Entry::B : InputDevice::Entry::A), input))
    {
      unAssign();
      setHelpMessage();
      setPress();
      return;
    }
  }

  if (isAssigned())
  {
    setHelpMessage();
    setPress();
    return;
  }

  // At first, try to find the preferred type, on the second pass, we ignore the preferred type
  for (auto input: events)
    if ((input.Type() == formInput.preferredType) && assign(input)) { rowDone(); return; }
  for (auto input: events)
    if (assign(input)) { rowDone(); break; }
}

void GuiInputConfig::RecordRawInput(const InputEvent& raw)
{
  switch(raw.Type())
  {
    case InputEvent::EventType::Axis:
    {
      // Calculate
      int value = 0;
      if (int neutral = mTargetDevice->NeutralAxisValue(raw.Id()); neutral != 0)
        value = raw.Value() < 0 && neutral > 0 ? 1 : (raw.Value() > 0 && neutral < 0 ? 3 : 0);
      else
        value = raw.Value() < 0 ? 1 : (raw.Value() > 0 ? 3 : 0); // 1 = -1 & 3 = 1 => value - 2 = +/-1

      // Affect?
      int mask = ~(0x3 << (2 * raw.Id()));
      if ((mActiveAxis & ~mask) != 0) // This axis has been activated?
        if (value == 0)
          mEventList.push_back(InputEvent(raw.Device(), raw.Type(), raw.Id(), ((mActiveAxis >> (2 * raw.Id())) & 0x03) - 2));

      // Assign value
      mActiveAxis &= mask;
      mActiveAxis |= value << (2 * raw.Id());
      break;
    }
    case InputEvent::EventType::Button:
    {
      int id = raw.Id() & 0x1F;

      // Affect?
      if (raw.Value() == 0) mEventList.push_back(InputEvent(raw.Device(), raw.Type(), raw.Id(), 1));

      // Assign
      mActiveButtons &= ~(1 << id);
      mActiveButtons |= (raw.Value() << id);
      /*if (raw.Value() != 0) mButtonReferenceTime[id] = (int)SDL_GetTicks();
      else
      {
        if (mButtonReferenceTime[id] !=0) SetActiveElement(0);
        mButtonReferenceTime[id] = 0;
      }*/
      break;
    }
    case InputEvent::EventType::Hat:
    {
      int mask = ~(0xF << (4 * raw.Id()));
      int value = raw.Value() << (4 * raw.Id());

      // Affect?
      if ((mActiveHats & ~mask) != 0) // This axis has been activated?
        if (raw.Value() == 0)
          mEventList.push_back(InputEvent(raw.Device(), raw.Type(), raw.Id(), (mActiveHats >> (4 * raw.Id())) & 0x0F));

      // Assign value
      mActiveHats &= mask;
      mActiveHats |= value;
      break;
    }
    case InputEvent::EventType::Unknown:
    case InputEvent::EventType::Key:
    default: break;
  }
}
