#pragma once

#include "GuiComponent.h"
#include "help/HelpStyle.h"


class ImageComponent;
class TextureResource;
class ComponentGrid;

class HelpComponent : public GuiComponent
{
public:
	explicit HelpComponent(Window&window);

	void render(const Transform4x4f& parent) override;
	void setOpacity(unsigned char opacity) override;

  void UpdateHelps();

  private:
	std::shared_ptr<TextureResource> getIconTexture(const char* name);
	std::map< std::string, std::shared_ptr<TextureResource> > mIconCache;

	std::shared_ptr<ComponentGrid> mGrid;

	HelpStyle mStyle;
};
