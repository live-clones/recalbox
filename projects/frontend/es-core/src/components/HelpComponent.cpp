#include <utils/Strings.h>
#include "components/HelpComponent.h"
#include "Renderer.h"
#include "Settings.h"
#include "utils/Log.h"
#include "components/ImageComponent.h"
#include "components/TextComponent.h"
#include "components/ComponentGrid.h"

#define ICON_TEXT_SPACING Math::max(Renderer::getDisplayWidthAsFloat() * 0.004f, 2.0f) // space between [icon] and [text] (px)
#define ENTRY_SPACING Math::max(Renderer::getDisplayWidthAsFloat() * 0.008f, 2.0f) // space between [text] and next [icon] (px)

static const std::map<HelpType, Path>& IconPathMap()
{
  static const std::map<HelpType, Path> _IconPathMap =
  {
    { HelpType::UpDown,            Path(":/help/dpad_updown.svg") },
    { HelpType::LeftRight,         Path(":/help/dpad_leftright.svg") },
    { HelpType::AllDirections,     Path(":/help/dpad_all.svg") },
    { HelpType::A,                 Path(":/help/button_a.svg") },
    { HelpType::B,                 Path(":/help/button_b.svg") },
    { HelpType::X,                 Path(":/help/button_x.svg") },
    { HelpType::Y,                 Path(":/help/button_y.svg") },
    { HelpType::L,                 Path(":/help/button_l.svg") },
    { HelpType::R,                 Path(":/help/button_r.svg") },
    { HelpType::Start,             Path(":/help/button_start.svg") },
    { HelpType::Select,            Path(":/help/button_select.svg") },
  };
  return _IconPathMap;
}

HelpComponent::HelpComponent(Window&window)
  : Component(window)
{
}

void HelpComponent::UpdateHelps()
{
	if(!Settings::Instance().ShowHelpPrompts() || HelpItems().Empty())
	{
		mGrid.reset();
		return;
	}

	const std::shared_ptr<Font>& font = HelpItemStyle().TextFont();

	mGrid = std::make_shared<ComponentGrid>(mWindow, Vector2i((int)Help::TypeCount() * 4, 1));
	// [icon] [spacer1] [text] [spacer2]

	std::vector< std::shared_ptr<ImageComponent> > icons;
	std::vector< std::shared_ptr<TextComponent> > labels;

	float width = 0;
	const float height = Math::round(font->getLetterHeight() * 1.25f);
	for (int i = 0; i < Help::TypeCount(); ++i)
    if (HelpItems().IsSet(Help::TypeFromIndex(i)))
    {
      auto icon = std::make_shared<ImageComponent>(mWindow);
      const Path imagePath = HelpItemStyle().ImagePath(Help::TypeFromIndex(i));
      if (!imagePath.IsEmpty())
        icon->setImage(imagePath);
      else
        icon->setImage(IconPathMap().at(Help::TypeFromIndex(i)));

      icon->setColorShift(HelpItemStyle().IconColor());
      icon->setResize(0, height);
      icons.push_back(icon);

      auto lbl = std::make_shared<TextComponent>(mWindow, Strings::ToUpperUTF8(HelpItems().Text(Help::TypeFromIndex(i))), font, HelpItemStyle().TextColor());
      labels.push_back(lbl);

      width += icon->getSize().x() + lbl->getSize().x() + ICON_TEXT_SPACING + ENTRY_SPACING;
    }

	mGrid->setSize(width, height);
	for (int i = 0; i < (int)icons.size(); i++)
	{
		const int col = i*4;
		mGrid->setColWidthPerc(col, icons.at(i)->getSize().x() / width);
		mGrid->setColWidthPerc(col + 1, ICON_TEXT_SPACING / width);
		mGrid->setColWidthPerc(col + 2, labels.at(i)->getSize().x() / width);

		mGrid->setEntry(icons.at(i), Vector2i(col, 0), false, false);
		mGrid->setEntry(labels.at(i), Vector2i(col + 2, 0), false, false);
	}

	mGrid->setPosition(Vector3f(HelpItemStyle().Position().x(), HelpItemStyle().Position().y(), 0.0f));
	//mGrid->setPosition(OFFSET_X, Renderer::getScreenHeight() - mGrid->getSize().y() - OFFSET_Y);
}

/*std::shared_ptr<TextureResource> HelpComponent::getIconTexture(const char* name)
{
	auto it = mIconCache.find(name);
	if(it != mIconCache.end())
		return it->second;

	auto pathLookup = IconPathMap().find(name);
	if(pathLookup == IconPathMap().end())
	{
		LOG(LogError) << "Unknown help icon \"" << name << "\"!";
		return nullptr;
	}
	if(!ResourceManager::getInstance()->fileExists(pathLookup->second))
	{
		LOG(LogError) << "Help icon \"" << name << "\" - corresponding image file \"" << pathLookup->second.ToString() << "\" missing!";
		return nullptr;
	}

	std::shared_ptr<TextureResource> tex = TextureResource::get(pathLookup->second);
	mIconCache[std::string(name)] = tex;
	return tex;
}*/

void HelpComponent::setOpacity(unsigned char opacity)
{
	Component::setOpacity(opacity);

	for (unsigned int i = 0; i < mGrid->getChildCount(); i++)
	{
		mGrid->getChild(i)->setOpacity(opacity);
	}
}

void HelpComponent::Render(const Transform4x4f& parentTrans)
{
	Transform4x4f trans = parentTrans * getTransform();

	if(mGrid)
    mGrid->Render(trans);
}
