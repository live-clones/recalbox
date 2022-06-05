#pragma once

#include <utils/os/system/Mutex.h>
#include <components/base/Component.h>
#include <components/ComponentGrid.h>
#include <components/NinePatchComponent.h>
#include <components/TextComponent.h>

class AnimatedImageComponent;

class BusyComponent : public Component
{
public:
	explicit BusyComponent(WindowManager&window);
	~BusyComponent() override = default;

	void onSizeChanged() override;
	void setText(std::string txt);

	void Render(const Transform4x4f& parentTrans) override;

	float RealWidth() const { return mBackground.getSize().x(); }
  float RealHeight() const { return mBackground.getSize().y(); }

  void Enable(bool enable) { mEnabled = enable; }

  void SetFont(std::shared_ptr<Font> font)
  {
    mText->setFont(font);
    onSizeChanged();
  }

private:
	NinePatchComponent mBackground;
	ComponentGrid mGrid;

	std::shared_ptr<AnimatedImageComponent> mAnimation;
	std::shared_ptr<TextComponent> mText;

	Mutex mMutex;
	bool mThreadMessagechanged;
	std::string threadMessage;
  bool mEnabled;
};