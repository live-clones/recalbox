#include "components/ProgressBarComponent.h"
#include "Renderer.h"
#include "WindowManager.h"
#include "themes/MenuThemeData.h"

ProgressBarComponent::ProgressBarComponent(WindowManager&window, long long maxvalue)
  : Component(window),
    mFont(Font::get(FONT_SIZE_MEDIUM)),
    mMaxValue(1),
    mCurrentValue(0)
{
	auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
	mFont = menuTheme->menuText.font;
  mEmptyColor = menuTheme->menuText.separatorColor;
  unsigned int averageComponent = (((mEmptyColor >> 24) & 0xFF) + ((mEmptyColor >> 16) & 0xFF) + ((mEmptyColor >> 8) & 0xFF)) / 3;
  mFillColor = averageComponent >= 0x80 ? 0x00000080 : 0xFFFFFF80;
  mTextColor = menuTheme->menuText.color;

	setMaxValue(maxvalue);
	setCurrentValue(0);
}

void ProgressBarComponent::Render(const Transform4x4f& parentTrans)
{
  Transform4x4f trans = (parentTrans * getTransform()).round();
  Renderer::SetMatrix(trans);

  float width = (float)((double)mCurrentValue * (double)mSize.x() / (double)mMaxValue);

  Renderer::DrawRectangle(0.0f, 0.0f, mSize.x(), mSize.y(), mEmptyColor);
  Renderer::DrawRectangle(0.0f, 0.0f, width, mSize.y(), mFillColor);
}

void ProgressBarComponent::Update(int deltaTime)
{
  Component::Update(deltaTime);
}

