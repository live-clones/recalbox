#include <utils/Strings.h>
#include "components/ProgressBarComponent.h"
#include "Renderer.h"
#include "Window.h"
#include "MenuThemeData.h"

ProgressBarComponent::ProgressBarComponent(Window&window, int maxvalue)
  : Component(window),
    mFont(Font::get(FONT_SIZE_MEDIUM)),
    mMaxValue(1),
    mCurrentValue(0)
{
	auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
	mFont = menuTheme->menuText.font;
  mEmptyColor = menuTheme->menuText.separatorColor;
  mFillColor = menuTheme->menuText.selectorColor;
  mTextColor = menuTheme->menuText.color;

	setMaxValue(maxvalue);
	setCurrentValue(0);
}

void ProgressBarComponent::Render(const Transform4x4f& parentTrans)
{
  Transform4x4f trans = (parentTrans * getTransform()).round();
  Renderer::setMatrix(trans);

  float width = (float)mCurrentValue * mSize.y() / (float)mMaxValue;

  Renderer::drawRect(0.0f, 0.0f, mSize.x(), mSize.y(), mEmptyColor);
  Renderer::drawRect(0.0f, 0.0f, width, mSize.y(), mFillColor);
}

void ProgressBarComponent::Update(int deltaTime)
{
  Component::Update(deltaTime);
}

