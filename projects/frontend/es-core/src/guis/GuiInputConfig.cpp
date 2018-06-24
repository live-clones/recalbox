#include "Locale.h"
#include "guis/GuiInputConfig.h"
#include "Window.h"
#include "Log.h"
#include "components/TextComponent.h"
#include "components/ImageComponent.h"
#include "components/MenuComponent.h"
#include "components/ButtonComponent.h"
#include "Util.h"
#include "Locale.h"
#include "InputManager.h"
#include "InputStack.h"
#include "MenuThemeData.h"
#include <boost/format.hpp>
using namespace boost::locale;

static const int inputCount = 21;
static const char* inputName[inputCount] = {      "Up", "Down", "Left", "Right", "Joystick1Up" , "Joystick1Left", "Joystick2Up" , "Joystick2Left", "A",    "B",   "X",   "Y", "Start", "Select", "PageUp", "PageDown", "L2", "R2", "L3", "R3", "HotKey" };
static const bool inputSkippable[inputCount] = { false, false,   false,   false,     true,              true,         true,             true,      false,  false,  true,   true, false,    false,     true,      true, true, true, true, true,  false};
static const char* inputIcon[inputCount] = { ":/help/dpad_up.svg", ":/help/dpad_down.svg", ":/help/dpad_left.svg", ":/help/dpad_right.svg", ":/help/joystick_up.svg", ":/help/joystick_left.svg", ":/help/joystick_up.svg", ":/help/joystick_left.svg",
											 ":/help/button_a.svg", ":/help/button_b.svg", ":/help/button_x.svg", ":/help/button_y.svg", ":/help/button_start.svg", ":/help/button_select.svg",
											":/help/button_l.svg", ":/help/button_r.svg", ":/help/button_l2.svg", ":/help/button_r2.svg",":/help/button_l3.svg", ":/help/button_r3.svg", ":/help/button_hotkey.svg" };

using namespace Eigen;

GuiInputConfig::GuiInputConfig(Window* window, InputConfig* target, const std::function<void()>& doneCallback) : GuiComponent(window), 
	mBackground(window, ":/frame.png"), mGrid(window, Vector2i(1, 5)), 
	mTargetConfig(target), mCursorOnList(true)
{
	LOG(LogInfo) << "Configuring device " << mTargetConfig->getDeviceId() << " (" << mTargetConfig->getDeviceName() << ").";

	std::string inputDispName[inputCount] = { _("UP"), _("DOWN"), _("LEFT"), _("RIGHT"),
						  _("JOYSTICK 1 UP"), _("JOYSTICK 1 LEFT"), _("JOYSTICK 2 UP"), _("JOYSTICK 2 LEFT"),
						  "A", "B", "X", "Y", "START", "SELECT ", _("PAGE UP"), _("PAGE DOWN"),  "L2", "R2", "L3", "R3", _("HOTKEY") };


	InputConfig previousConfig =  new InputConfig(mTargetConfig);
	mTargetConfig->clear();
	
	auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
	
	mBackground.setImagePath(menuTheme->menuBackground.path);
	mBackground.setCenterColor(menuTheme->menuBackground.color);
	mBackground.setEdgeColor(menuTheme->menuBackground.color);
	
	mMainColor = menuTheme->menuText.color;

	addChild(&mBackground);
	addChild(&mGrid);

	mTitle = std::make_shared<TextComponent>(mWindow, _("CONFIGURING"), menuTheme->menuTitle.font, menuTheme->menuTitle.color, ALIGN_CENTER);
	mGrid.setEntry(mTitle, Vector2i(0, 0), false, true);

	char strbuf[256];
	if(mTargetConfig->getDeviceId() == DEVICE_KEYBOARD)
	  strncpy(strbuf, _("KEYBOARD").c_str(), 256);
	else
	  snprintf(strbuf, 256, _("GAMEPAD %i").c_str(), mTargetConfig->getDeviceId() + 1);
	  
	mSubtitle1 = std::make_shared<TextComponent>(mWindow, strToUpper(strbuf), menuTheme->menuText.font, menuTheme->menuFooter.color, ALIGN_CENTER);
	mGrid.setEntry(mSubtitle1, Vector2i(0, 1), false, true);

	mSubtitle2 = std::make_shared<TextComponent>(mWindow, "", menuTheme->menuTextSmall.font, menuTheme->menuTextSmall.color, ALIGN_CENTER);
	mGrid.setEntry(mSubtitle2, Vector2i(0, 2), false, true);

	mList = std::make_shared<ComponentList>(mWindow);
	mGrid.setEntry(mList, Vector2i(0, 3), true, true);
	
	for(int i = 0; i < inputCount; i++) {
		ComponentListRow row;
		// icon
		auto icon = std::make_shared<ImageComponent>(mWindow);
		icon->setImage(inputIcon[i]);
		icon->setColorShift(menuTheme->menuText.color);
		icon->setResize(0, menuTheme->menuText.font->getLetterHeight() * 1.25f);
		row.addElement(icon, false);

		// spacer between icon and text
		auto spacer = std::make_shared<GuiComponent>(mWindow);
		spacer->setSize(16, 0);
		row.addElement(spacer, false);

		auto text = std::make_shared<TextComponent>(mWindow, inputDispName[i], menuTheme->menuText.font, menuTheme->menuText.color);
		row.addElement(text, true);

		auto mapping = std::make_shared<TextComponent>(mWindow, "", menuTheme->menuText.font, menuTheme->menuText.color, ALIGN_RIGHT);
		row.addElement(mapping, true);
		mMappings.push_back(mapping);

		row.input_handler = [this, i, mapping](InputConfig* config, Input input) -> bool {
			// ignore input not from our target device
			if(config != mTargetConfig)
				return false;

			if(input.value != 0) { // pressed (an event is fired when releasing, with value == 0)
				if(!mInputStack.hasInput(input)) {
					mInputStack.push(input, [this, i] (const std::list<Input> inputs) {
						for(auto it = inputs.begin(); it != inputs.end(); it++) {
							// Key Up
							if (mTargetConfig->isMappedTo("UP", *it)) {
								if (i > 0 && mTargetConfig->isMapped("DOWN")) {
									restaurePreviousAssignment();
									if (!isAssigned() && inputSkippable[i]) {
										setSkipped();
									}
									mList->moveCursor(-1);
									setPress();
								}
								return;
							}

							// Key Down
							if (mTargetConfig->isMappedTo("DOWN", *it)) {
								if (!isAssigned() && !inputSkippable[i])
									return ;
								restaurePreviousAssignment();
								if (!isAssigned()) {
									setSkipped();
								}
								rowDone();
								return;
							}

							if (isAssigned()) {
								if (mTargetConfig->isMappedTo("A", *it)) {
									unAssign();
								}

								setHelpMessage();
								setPress();
								return;
							}
						}

						for(auto it = inputs.begin(); it != inputs.end(); it++) {
							if(assign(*it)) {
								rowDone(); // if successful, move cursor/stop configuring - if not, we'll just try again
								return ;
							}
						}
					});
				}
			}
			return true;
		};

		mList->addRow(row);
	}

	mList->setCursorChangedCallback([this](CursorState state) {
		setHelpMessage();
	});

	// when lefting the "OK" button, back to the mList items by clicking the UP on the gamepad, the SDL may send more than one event 
	// (SDL_JOYBUTTONDOWN and SDL_JOYHATMOTION).
	// The first one is used by the window manager to get back the focus on the mList items, but the 
	// other one(s) will be sent to the mList last item handler, so, we have to swallow this kind of "echo"
	// during a small empiric delay
	mList->setFocusGainedCallback([this]() {
		mInputStack.debounce();
		mCursorOnList = true;
		auto cursorChange = mList->getCursorChangedCallback();
		cursorChange(CURSOR_STOPPED);
		setPress();
	});

	mList->setFocusLostCallback([this]() {
		mCursorOnList = false;
		auto cursorChange = mList->getCursorChangedCallback();
		cursorChange(CURSOR_STOPPED);
		mSubtitle2->setText("");
	});

	setHelpMessage();
	setPress();

	// buttons
	std::vector< std::shared_ptr<ButtonComponent> > buttons;

	buttons.push_back(std::make_shared<ButtonComponent>(mWindow, _("OK"), _("OK"), [this, doneCallback] {
		InputManager::getInstance()->writeDeviceConfig(mTargetConfig); // save
		if(doneCallback)
			doneCallback();
		delete this; 
	}));

	buttons.push_back(std::make_shared<ButtonComponent>(mWindow, _("CANCEL"), _("CANCEL"), [this, previousConfig, doneCallback] {
		mTargetConfig->loadFrom(&previousConfig); // restaure previous target configuration
		if(doneCallback)
			doneCallback();
		delete this; 
	}));

	mButtonGrid = makeButtonGrid(mWindow, buttons);
	mGrid.setEntry(mButtonGrid, Vector2i(0, 4), true, false);

	setSize(Renderer::getScreenWidth() * 0.6f, Renderer::getScreenHeight() * 0.85f);
	setPosition((Renderer::getScreenWidth() - mSize.x()) / 2, (Renderer::getScreenHeight() - mSize.y()) / 2);
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
void GuiInputConfig::rowDone() {
	if(!mList->moveCursor(1)) { // try to move to the next one
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
	std::shared_ptr<TextComponent>& text = mMappings.at(inputId);
	text->setText(strToUpper(msg));
	text->setColor(color);
}

void GuiInputConfig::setHelpMessage() {
	std::string msg;
	Input input;
	int inputId = mList->getCursorId();
	bool skippable = inputSkippable[inputId];
	bool assigned = mTargetConfig->getInputByName(inputName[inputId], &input);
	std::shared_ptr<TextComponent>& text = mMappings.at(inputId);
	auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();

	if (assigned) {
		if (mTargetConfig->isMapped("A")) {
			msg = msg + _("A TO UNSET");
		}
		if (mTargetConfig->isMapped("DOWN")) {
			if (inputId == 0)
				msg = (msg.length() ? msg + " - " : "") +
					  str(boost::format(_("DOWN TO KEEP [%1%]")) % strToUpper(input.string()));
			else
				msg = (msg.length() ? msg + " - " : "") +
					  str(boost::format(_("UP/DOWN TO KEEP [%1%]")) % strToUpper(input.string()));
		}

	} else if (skippable)
		msg = _("UP/DOWN TO SKIP");
	else 
		msg = _("INPUT REQUIRED");

	mSubtitle2->setText(msg);
	mSubtitle2->setColor(skippable || assigned ? menuTheme->menuTextSmall.color : 0xff4141ff);
}

void GuiInputConfig::setPress() {
	Input input;
	int inputId = mList->getCursorId();
	if (mTargetConfig->getInputByName(inputName[inputId], &input)) {
		setAssignedTo(input);
	} else {
		setText(_("PRESS ANYTHING"), mMainColor); 
	}
}

bool GuiInputConfig::assign(Input input) {
	int inputId = mList->getCursorId();

	// input is from InputConfig* mTargetConfig
	// if this input is mapped to something other than "nothing" or the current row, error
	// (if it's the same as what it was before, allow it)
	if(std::string("HotKey").compare(inputName[inputId]) != 0 && mTargetConfig->getMappedTo(input).size() > 0 && !mTargetConfig->isMappedTo(inputName[inputId], input)) {
		setMapped();
		return false;
	}

	setAssignedTo(input);
	
	input.configured = true;
	mTargetConfig->mapInput(inputName[inputId], input);

	LOG(LogInfo) << "  Mapping [" << input.string() << "] -> " << inputName[inputId];

	return true;
}

void GuiInputConfig::unAssign() {
	Input input;
	int inputId = mList->getCursorId();
	mTargetConfig->getInputByName(inputName[inputId], &input);

	setNotDefined();
	
	input.configured = false;
	mTargetConfig->unmapInput(inputName[inputId]);

	LOG(LogInfo) << "  Unmapping [" << input.string() << "] -> " << inputName[inputId];
}

void GuiInputConfig::restaurePreviousAssignment() {
	Input input;
	int inputId = mList->getCursorId();
	if(mTargetConfig->getInputByName(inputName[inputId], &input))
		setAssignedTo(input);
	else
		setNotDefined();
}

bool GuiInputConfig::isAssigned() {
	int inputId = mList->getCursorId();
	return mTargetConfig->isMapped(inputName[inputId]);
}