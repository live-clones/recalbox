#include <guis/GuiArcadeVirtualKeyboard.h>
#include "components/TextEditComponent.h"
#include "utils/Log.h"
#include "resources/Font.h"
#include "WindowManager.h"
#include "Renderer.h"

#include "utils/locale/LocaleHelper.h"
#include "themes/MenuThemeData.h"

TextEditComponent::TextEditComponent(WindowManager&window)
	: Component(window),
    mFocused(false),
    mEditing(false),
    mCursor(0),
    mCursorRepeatTimer(0),
    mCursorRepeatDir(0),
    mScrollOffset(0.0f, 0.0f),
    mBox(window, Path(":/textinput_ninepatch.png")),
    mFont(MenuThemeData::getInstance()->getCurrentTheme()->menuText.font)
{
	addChild(&mBox);
	
	onFocusLost();

	setSize(256, mFont->getHeight() + TEXT_PADDING_VERT);
}

void TextEditComponent::onFocusGained()
{
	mFocused = true;
	mBox.setImagePath(Path(":/textinput_ninepatch_active.png"));
	startEditing();
}

void TextEditComponent::onFocusLost()
{
	mFocused = false;
	mBox.setImagePath(Path(":/textinput_ninepatch.png"));
	stopEditing();
}

void TextEditComponent::onSizeChanged()
{
	mBox.fitTo(mSize, Vector3f::Zero(), Vector2f(-34, -32 - TEXT_PADDING_VERT));
	mScrollOffset[0] = 0;
	onTextChanged(); // wrap point probably changed
}

void TextEditComponent::setValue(const std::string& val)
{
  mText = val;
	onTextChanged();
}

void TextEditComponent::textInput(const char* text)
{
  //mText = text;
  //mCursor = cursor > 0 ? cursor : mText.length();
  mCursorRepeatDir = 0;
  if(text[0] == '\b')
  {
    if(mCursor > 0)
    {
      size_t newCursor = Font::getPrevCursor(mText, mCursor);
      mText.erase(mText.begin() + newCursor, mText.begin() + mCursor);
      mCursor = newCursor;
    }
  }
  else
  {
    mText.insert(mCursor, text);
    mCursor += strlen(text);
  }

	onTextChanged();
	onCursorChanged();
}

void TextEditComponent::startEditing()
{
	SDL_StartTextInput();
	mEditing = true;
	updateHelpPrompts();
}

void TextEditComponent::stopEditing()
{
	SDL_StopTextInput();
	mEditing = false;
	updateHelpPrompts();
}

void TextEditComponent::ArcadeVirtualKeyboardCanceled(GuiArcadeVirtualKeyboard& vk)
{
  (void)vk;
}

void TextEditComponent::ArcadeVirtualKeyboardValidated(GuiArcadeVirtualKeyboard& vk, const std::string& text)
{
  (void)vk;
  (void)text;
}

void TextEditComponent::ArcadeVirtualKeyboardTextChange(GuiArcadeVirtualKeyboard& vk, const std::string& text)
{
  (void)vk;
  setValue(text);
  setCursor(text.length());
}

bool TextEditComponent::ProcessInput(const InputCompactEvent& event)
{
	if (event.AnythingReleased())
	{
		if (event.LeftReleased() || event.RightReleased())
			mCursorRepeatDir = 0;
		return false;
	}

	if (!event.IsKeyboard() && event.R1Pressed() && mFocused)
	{
    auto* vk = new GuiArcadeVirtualKeyboard(mWindow, "", getValue(), this);
    mWindow.pushGui(vk);
		//startEditing();
		return true;
	}

  if (event.IsKeyboard())
  {
    //{ LOG(LogDebug) << "[TextInput] Type: " << (int)event.RawEvent().Type() << " - Id: " << event.RawEvent().Id() << " - Device: " << event.RawEvent().Device() << " - Value: " << event.RawEvent().Value(); }
    bool pressed = event.RawEvent().Value() != 0;
    switch(event.RawEvent().Id())
    {
      case SDLK_BACKSPACE: if (pressed) textInput("\b"); break;
      case SDLK_DELETE:
      {
        if (pressed)
          if (mCursor < (int)mText.length())
          {
            size_t newCursor = Font::getNextCursor(mText, mCursor);
            mText.erase(mText.begin() + mCursor, mText.begin() + newCursor);
            onTextChanged();
          }
        break;
      }
      case SDLK_RETURN:
      case SDLK_RETURN2: if (isMultiline()) textInput("\n"); else stopEditing(); break;
      case SDLK_ESCAPE: stopEditing(); break;
      case SDLK_LEFT:
      case SDLK_RIGHT:
      {
        mCursorRepeatDir = event.RawEvent().Id() == SDLK_LEFT ? -1 : 1;
        mCursorRepeatTimer = -(CURSOR_REPEAT_START_DELAY - CURSOR_REPEAT_SPEED);
        moveCursor(mCursorRepeatDir);
        break;
      }
    }
    return true;
  }

	return false;
}

void TextEditComponent::Update(int deltaTime)
{
	updateCursorRepeat(deltaTime);
  Component::Update(deltaTime);
}

void TextEditComponent::updateCursorRepeat(int deltaTime)
{
	if(mCursorRepeatDir == 0)
		return;

	mCursorRepeatTimer += deltaTime;
	while(mCursorRepeatTimer >= CURSOR_REPEAT_SPEED)
	{
		moveCursor(mCursorRepeatDir);
		mCursorRepeatTimer -= CURSOR_REPEAT_SPEED;
	}
}

void TextEditComponent::moveCursor(int amt)
{
	mCursor = Font::moveCursor(mText, mCursor, amt);
	onCursorChanged();
}

void TextEditComponent::setCursor(size_t pos)
{
	if(pos == std::string::npos)
		mCursor = mText.length();
	else
		mCursor = (int)pos;

	moveCursor(0);
}

void TextEditComponent::onTextChanged()
{
	std::string wrappedText = (isMultiline() ? mFont->wrapText(mText, getTextAreaSize().x()) : mText);
	mTextCache = std::unique_ptr<TextCache>(mFont->buildTextCache(wrappedText, 0, 0, 0x77777700 | getOpacity()));

	if(mCursor > (int)mText.length())
		mCursor = mText.length();

	if(mTextChangedCallback)
		mTextChangedCallback();
}

void TextEditComponent::onCursorChanged()
{
	if(isMultiline())
	{
		Vector2f textSize = mFont->getWrappedTextCursorOffset(mText, getTextAreaSize().x(), mCursor); 

		if(mScrollOffset.y() + getTextAreaSize().y() < textSize.y() + mFont->getHeight()) //need to scroll down?
		{
			mScrollOffset[1] = textSize.y() - getTextAreaSize().y() + mFont->getHeight();
		}else if(mScrollOffset.y() > textSize.y()) //need to scroll up?
		{
			mScrollOffset[1] = textSize.y();
		}
	}else{
		Vector2f cursorPos = mFont->sizeText(mText.substr(0, mCursor));

		if(mScrollOffset.x() + getTextAreaSize().x() < cursorPos.x())
		{
			mScrollOffset[0] = cursorPos.x() - getTextAreaSize().x();
		}else if(mScrollOffset.x() > cursorPos.x())
		{
			mScrollOffset[0] = cursorPos.x();
		}
	}
}

void TextEditComponent::Render(const Transform4x4f& parentTrans)
{
	Transform4x4f trans = getTransform() * parentTrans;
	renderChildren(trans);

	// text + cursor rendering
	// offset into our "text area" (padding)
	trans.translation() += Vector3f(getTextAreaPos().x(), getTextAreaPos().y(), 0);

	Vector2i clipPos((int)trans.translation().x(), (int)trans.translation().y());
	Vector3f dimScaled = trans * Vector3f(getTextAreaSize().x(), getTextAreaSize().y(), 0); // use "text area" size for clipping
	Vector2i clipDim((int)(dimScaled.x() - trans.translation().x()), (int)(dimScaled.y() - trans.translation().y()));
	Renderer::Instance().PushClippingRect(clipPos, clipDim);

	trans.translate(Vector3f(-mScrollOffset.x(), -mScrollOffset.y(), 0));
	trans.round();

	Renderer::SetMatrix(trans);

	if(mTextCache)
	{
		mFont->renderTextCache(mTextCache.get());
	}

	// pop the clip early to allow the cursor to be drawn outside of the "text area"
	Renderer::Instance().PopClippingRect();

	// draw cursor
	if(mEditing)
	{
		Vector2f cursorPos(0);
		if(isMultiline())
		{
			cursorPos = mFont->getWrappedTextCursorOffset(mText, getTextAreaSize().x(), mCursor);
		}else{
			cursorPos = mFont->sizeText(mText.substr(0, mCursor));
			cursorPos[1] = 0;
		}

		float cursorHeight = mFont->getHeight() * 0.8f;
		Renderer::DrawRectangle(cursorPos.x(), cursorPos.y() + (mFont->getHeight() - cursorHeight) / 2, 2.0f, cursorHeight, 0x000000FF);
	}
}

bool TextEditComponent::isMultiline()
{
	return (getSize().y() > mFont->getHeight() * 1.25f);
}

Vector2f TextEditComponent::getTextAreaSize() const
{
	return { mSize.x() - TEXT_PADDING_HORIZ, mSize.y() - TEXT_PADDING_VERT };
}

bool TextEditComponent::getHelpPrompts(Help& help)
{
	if(mEditing)
		help.Set(HelpType::AllDirections, _("MOVE CURSOR"))
		    .Set(Help::Cancel(), _("STOP EDITING"));
	else
		help.Set(Help::Valid(), _("EDIT"));
	return true;
}
