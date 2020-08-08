#pragma once

#include "components/base/Component.h"
#include <functional>
#include "resources/Font.h"
#include "components/NinePatchComponent.h"
#include <string>

class ProgressBarComponent : public Component
{
  private:
    std::shared_ptr<Font> mFont;

    int mMaxValue;
    int mCurrentValue;

    unsigned int mEmptyColor;
    unsigned int mFillColor;
    unsigned int mTextColor;

    void Update(int deltaTime) override;
    void Render(const Transform4x4f& parentTrans) override;

  public:
    ProgressBarComponent(Window&window, int maxvalue);

	  void setMaxValue(int maxvalue)
    {
	    if (maxvalue < 1) maxvalue = 1;
	    if (maxvalue < mCurrentValue) maxvalue = mCurrentValue;
	    mMaxValue = maxvalue;
    }

    void setCurrentValue(int value)
    {
	    if (value < 0) value = 0;
	    if (value > mMaxValue) value = mMaxValue;
	    mCurrentValue = value;
    }

    int getMaxValue() const { return mMaxValue; }

	  int getCurrentValue() const { return mCurrentValue; }

	  std::string getText() const { return Strings::ToString((float)mCurrentValue * 100.0f / (float)mMaxValue, 2).append(1, '%'); }
};
