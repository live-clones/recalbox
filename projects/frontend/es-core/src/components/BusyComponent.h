#pragma once
#include "GuiComponent.h"
#include "components/ComponentGrid.h"
#include "components/NinePatchComponent.h"

class AnimatedImageComponent;
class TextComponent;

class BusyComponent : public GuiComponent
{
public:
	explicit BusyComponent(Window* window);
	~BusyComponent() override;

	void onSizeChanged() override;
	void setText(std::string txt);

	void render(const Transform4x4f& parentTrans) override;

private:
	NinePatchComponent mBackground;
	ComponentGrid mGrid;

	std::shared_ptr<AnimatedImageComponent> mAnimation;
	std::shared_ptr<TextComponent> mText;

	SDL_mutex *mutex;
	bool mThreadMessagechanged;
	std::string threadMessage;
};