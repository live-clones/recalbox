#pragma once

#include <utils/os/system/Mutex.h>
#include "components/base/Component.h"
#include "components/ComponentGrid.h"
#include "components/NinePatchComponent.h"

class AnimatedImageComponent;
class TextComponent;

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

private:
	NinePatchComponent mBackground;
	ComponentGrid mGrid;

	std::shared_ptr<AnimatedImageComponent> mAnimation;
	std::shared_ptr<TextComponent> mText;

	Mutex mMutex;
	bool mThreadMessagechanged;
	std::string threadMessage;
};