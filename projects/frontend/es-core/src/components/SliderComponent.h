#pragma once

#include "components/base/Component.h"
#include "components/ImageComponent.h"
#include "ISliderComponent.h"

class TextCache;
class Font;

// Used to display/edit a value between some min and max values.
class SliderComponent : public Component
{
public:
	//Minimum value (far left of the slider), maximum value (far right of the slider), increment size (how much just pressing L/R moves by), unit to display (optional).
	SliderComponent(WindowManager&window, float min, float max, float increment, const std::string& suffix);

	void setSlider(float val);
	float getSlider() { return mValue; }

  void setColor(unsigned int color) override;
	inline void setOriginColor(unsigned int color){ mOriginColor = color; }
	inline unsigned int getOriginColor() override{return mOriginColor;};

	bool ProcessInput(const InputCompactEvent& event) override;
	void Update(int deltaTime) override;
	void Render(const Transform4x4f& parentTrans) override;
	
	void onSizeChanged() override;
	
	bool getHelpPrompts(Help& help) override;

	inline void setSelectedChangedCallback(const std::function<void(const float&)>& callback)
	{
		mSelectedChangedCallback = callback;
	}

	inline void SetInterface(int id, ISliderComponent* interface)
  {
	  mIdentifier = id;
	  mInterface = interface;
  }

private:
	void onValueChanged();

	float mMin, mMax;
	float mValue;
	float mSingleIncrement;
	float mMoveRate;
	int mMoveAccumulator;
	unsigned int mColor;
	unsigned int mOriginColor;

	ImageComponent mKnob;

	std::string mSuffix;
	std::shared_ptr<Font> mFont;
	std::shared_ptr<TextCache> mValueCache;

	std::function<void(const float&)> mSelectedChangedCallback;
	ISliderComponent* mInterface;
	int mIdentifier;
};
