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

    long long mMaxValue;
    long long mCurrentValue;

    unsigned int mEmptyColor;
    unsigned int mFillColor;
    unsigned int mTextColor;

    void Update(int deltaTime) override;
    void Render(const Transform4x4f& parentTrans) override;

  public:
    ProgressBarComponent(WindowManager&window, long long maxvalue);

	  void setMaxValue(long long maxvalue)
    {
	    if (maxvalue < 1) maxvalue = 1;
	    if (maxvalue < mCurrentValue) maxvalue = mCurrentValue;
	    mMaxValue = maxvalue;
    }

    void setCurrentValue(long long value)
    {
	    if (value < 0) value = 0;
	    if (value > mMaxValue) value = mMaxValue;
	    mCurrentValue = value;
    }

    long long getMaxValue() const { return mMaxValue; }

	  long long getCurrentValue() const { return mCurrentValue; }

	  std::string getText() const { return Strings::ToString((float)mCurrentValue * 100.0f / (float)mMaxValue, 2).append(1, '%'); }
};
