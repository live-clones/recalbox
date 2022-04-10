#include <components/SliderComponent.h>
#include <cassert>
#include <Renderer.h>
#include <themes/MenuThemeData.h>
#include <help/Help.h>
#include <input/InputCompactEvent.h>
#include <utils/Log.h>
#include <utils/locale/LocaleHelper.h>

#define MOVE_REPEAT_DELAY 500
#define MOVE_REPEAT_RATE 40

SliderComponent::SliderComponent(WindowManager&window, float min, float max, float increment, const std::string& suffix)
  : Component(window)
  , mMin(min)
  , mMax(max)
  , mSingleIncrement(increment)
  , mMoveRate(0)
  , mMoveAccumulator(0)
  , mOriginColor(0)
  , mKnob(window)
  , mSuffix(suffix)
  , mInterface(nullptr)
  , mIdentifier(0)
{
	assert((min - max) != 0);

	// some sane default value
	mValue = (max + min) / 2;
	
	auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
	mColor = mOriginColor = menuTheme->menuText.color;

	mKnob.setOrigin(0.5f, 0.5f);
	mKnob.setImage(menuTheme->iconSet.knob);
	
	mKnob.setColorShift(mColor);
	setSize(Renderer::Instance().DisplayWidthAsFloat() * 0.15f, menuTheme->menuText.font->getLetterHeight());
}

bool SliderComponent::ProcessInput(const InputCompactEvent& event)
{
	if (event.AnyLeft())
	{
		if (event.AnyLeftPressed()) setSlider(mValue - mSingleIncrement);

		mMoveRate = event.AnyLeftPressed() ? -mSingleIncrement : 0;
		mMoveAccumulator = -MOVE_REPEAT_DELAY;
	}
	if (event.AnyRight() && !event.AnyLeftPressed())
	{
		if (event.AnyRightPressed()) setSlider(mValue + mSingleIncrement);

		mMoveRate = event.AnyRightPressed() ? mSingleIncrement : 0;
		mMoveAccumulator = -MOVE_REPEAT_DELAY;
	}

	return Component::ProcessInput(event);
}

void SliderComponent::Update(int deltaTime)
{
	if(mMoveRate != 0)
	{
		mMoveAccumulator += deltaTime;
		while(mMoveAccumulator >= MOVE_REPEAT_RATE)
		{
			setSlider(mValue + mMoveRate);
			mMoveAccumulator -= MOVE_REPEAT_RATE;
		}
	}

  Component::Update(deltaTime);
}

void SliderComponent::Render(const Transform4x4f& parentTrans)
{
  Transform4x4f trans = (parentTrans * getTransform()).round();
	Renderer::SetMatrix(trans);

	// render suffix
	if(mValueCache)
		mFont->renderTextCache(mValueCache.get());

	float width = mSize.x() - mKnob.getSize().x() - (mValueCache ? mValueCache->metrics.size.x() + 4 : 0);

	//render line
	const float lineWidth = 2;

	Renderer::DrawRectangle(mKnob.getSize().x() / 2, mSize.y() / 2 - lineWidth / 2, width, lineWidth, mColor);

	//render knob
  mKnob.Render(trans);
	
	Component::renderChildren(trans);
}

void SliderComponent::setSlider(float value)
{
  if (mValue != value)
  {
    mValue = Math::clamp(value, mMin, mMax);
    onValueChanged();
  }
}

void SliderComponent::setColor(unsigned int color)
{
  if (color != mColor)
  {
    mColor = color;
    mKnob.setColorShift(mColor);
    onValueChanged();
  }
}

void SliderComponent::onSizeChanged()
{
	if(!mSuffix.empty())
		mFont = Font::get((int)(mSize.y()), Path(FONT_PATH_LIGHT));
	
	onValueChanged();
}

void SliderComponent::onValueChanged()
{
	// update suffix textcache
	if(mFont)
	{
		const std::string val = Strings::ToString(mValue, 0) + mSuffix;
		const std::string max = Strings::ToString(mMax, 0) + mSuffix;

		Vector2f textSize = mFont->sizeText(max);

		mValueCache = std::shared_ptr<TextCache>(mFont->buildTextCache(val, mSize.x() - textSize.x(), (mSize.y() - textSize.y()) / 2, mColor));
		mValueCache->metrics.size[0] = textSize.x(); // fudge the width
	}

	// update knob position/size
	mKnob.setResize(0, mSize.y() * 0.7f);
	float lineLength = mSize.x() - mKnob.getSize().x() - (mValueCache ? mValueCache->metrics.size.x() + 4 : 0);
	float position = mMin >= 0 ? (mValue + mMin) / mMax : (mValue - mMin) / (mMax - mMin);
	mKnob.setPosition(position * lineLength + mKnob.getSize().x()/2, mSize.y() / 2);

	if (mInterface != nullptr)
	  mInterface->SliderMoved(mIdentifier, mValue);

	//send new knob value to functor if any
	if (mSelectedChangedCallback)
		mSelectedChangedCallback(mValue);
}

bool SliderComponent::getHelpPrompts(Help& help)
{
	help.Set(HelpType::LeftRight, _("CHANGE"));
	return true;
}

