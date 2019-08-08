#include "components/ButtonComponent.h"
#include "Renderer.h"
#include "Window.h"
#include "Util.h"
#include "Log.h"
#include "Locale.h"
#include "MenuThemeData.h"

ButtonComponent::ButtonComponent(Window* window, const std::string& text, const std::string& helpText, const std::function<void()>& func, bool upperCase) : GuiComponent(window),
	mFont(Font::get(FONT_SIZE_MEDIUM)),
	mFocused(false), 
	mEnabled(true), 
	mTextColorFocused(0xFFFFFFFF), mTextColorUnfocused(0x777777FF),
  mModdedColor(0),
  mBox(window, mButton)
{
	auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
	mFont = menuTheme->menuText.font;
	mTextColorUnfocused = menuTheme->menuText.color;
	mTextColorFocused = menuTheme->menuText.selectedColor;
	mColor = menuTheme->menuText.color;
	mButton = menuTheme->iconSet.button;
	mButton_filled = menuTheme->iconSet.button_filled;
	
	setPressedFunc(func);
	setText(text, helpText, upperCase);
	updateImage();
}

void ButtonComponent::onSizeChanged()
{
	mBox.fitTo(mSize, Vector3f::Zero(), Vector2f(-32, -32));
}

void ButtonComponent::setPressedFunc(std::function<void()> f)
{
	mPressedFunc = std::move(f);
}

bool ButtonComponent::input(InputConfig* config, Input input)
{
	if(config->isMappedTo("b", input) && input.value != 0)
	{
		if(mPressedFunc && mEnabled)
			mPressedFunc();
		return true;
	}

	return GuiComponent::input(config, input);
}

void ButtonComponent::setText(const std::string& text, const std::string& helpText, bool upperCase, bool resize, bool doUpdateHelpPrompts)
{
	mText = upperCase ? strToUpper(text) : text;
	mHelpText = helpText;

	mTextCache = std::unique_ptr<TextCache>(mFont->buildTextCache(mText, 0, 0, getCurTextColor()));

	if (resize)
	{
		float minWidth = mFont->sizeText("DELETE").x() + 12;
		setSize(std::max(mTextCache->metrics.size.x() + 12, minWidth), mTextCache->metrics.size.y() + 8);
	}

	if (doUpdateHelpPrompts)
  	updateHelpPrompts();
}

void ButtonComponent::autoSizeFont()
{
	if (mFont->sizeText(mText).x() + 6 > getSize().x()) {
		mFont = Font::get(FONT_SIZE_SMALL);
		onSizeChanged();
	}
	if (mFont->sizeText(mText).x() + 6 > getSize().x()) {
		mFont = Font::get(FONT_SIZE_EXTRASMALL);
		onSizeChanged();
	}
}

bool ButtonComponent::isTextOverlapping()
{
	// + 2 to keep a small space between text and border
	return mFont->sizeText(mText).x() + 2  > getSize().x();
}

void ButtonComponent::onFocusGained()
{
	mFocused = true;
	updateImage();
}

void ButtonComponent::onFocusLost()
{
	mFocused = false;
	updateImage();
}

void ButtonComponent::setEnabled(bool enabled)
{
	mEnabled = enabled;
	updateImage();
}

void ButtonComponent::updateImage()
{
	if(!mEnabled || !mPressedFunc)
	{
		mBox.setImagePath(mButton_filled);
		mBox.setCenterColor(0x770000FF);
		mBox.setEdgeColor(0x770000FF);
		return;
	}

	// If a new color has been set.  
	if (mNewColor) {
		mBox.setImagePath(mButton_filled);
		mBox.setCenterColor(mModdedColor);
		mBox.setEdgeColor(mModdedColor);
		return;
	}

	mBox.setCenterColor(mColor);
	mBox.setEdgeColor(mColor);
	mBox.setImagePath(mFocused ? mButton_filled : mButton);
}

void ButtonComponent::render(const Transform4x4f& parentTrans)
{
  Transform4x4f trans = roundMatrix(parentTrans * getTransform());
	
	mBox.render(trans);

	if(mTextCache)
	{
		Vector3f centerOffset((mSize.x() - mTextCache->metrics.size.x()) / 2, (mSize.y() - mTextCache->metrics.size.y()) / 2, 0);
		centerOffset = roundVector(centerOffset);
		trans = trans.translate(centerOffset);

		Renderer::setMatrix(trans);
		mTextCache->setColor(getCurTextColor());
		mFont->renderTextCache(mTextCache.get());
		trans = trans.translate(-centerOffset);
	}

	renderChildren(trans);
}

unsigned int ButtonComponent::getCurTextColor() const
{
	if(!mFocused)
		return mTextColorUnfocused;
	else
		return mTextColorFocused;
}

std::vector<HelpPrompt> ButtonComponent::getHelpPrompts()
{
	std::vector<HelpPrompt> prompts;
	prompts.push_back(HelpPrompt("b", mHelpText.empty() ? mText.c_str() : mHelpText.c_str()));
	return prompts;
}
