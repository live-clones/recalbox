#pragma once

#include "components/base/Component.h"
#include "components/ImageComponent.h"
#include "ISwitchComponent.h"

// A very simple "on/off" switch.
// Should hopefully be switched to use images instead of text in the future.
class SwitchComponent : public Component
{
public:
	explicit SwitchComponent(WindowManager&window);
  SwitchComponent(WindowManager&window, bool state)
    : SwitchComponent(window)
  {
    mInitialState = state;
    mState = state;
    setImageState();
  }

  inline void SetInterface(int id, ISwitchComponent* interface)
  {
    mId = id;
    mInterface = interface;
  }

    bool ProcessInput(const InputCompactEvent& event) override;
	void Render(const Transform4x4f& parentTrans) override;
	void onSizeChanged() override;
	void setColor(unsigned int color) override;
	inline void setOriginColor(unsigned int color){mOriginColor = color;};
	inline unsigned int getOriginColor() override{return mOriginColor;};

	bool getState() const  { return mState; }

  void setState(bool state);
	std::string getValue() const override;
	bool changed() const;

	bool getHelpPrompts(Help& help) override;

  inline void setChangedCallback(const std::function<void(bool state)>& callback) { mChangedCallback = callback; };

private:
  void setImageState();
	void onStateChanged();

	ImageComponent mImage;
	std::function<void(bool)> mChangedCallback;
	ISwitchComponent* mInterface;
  unsigned int mOriginColor;
  int mId;
	bool mState;
	bool mInitialState;
};
