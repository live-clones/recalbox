#include <utils/Strings.h>
#include <utils/storage/HashMap.h>
#include "components/HelpComponent.h"
#include "Renderer.h"
#include "Settings.h"
#include "utils/Log.h"
#include "components/ImageComponent.h"
#include "components/TextComponent.h"
#include "components/ComponentGrid.h"

#define ICON_TEXT_SPACING Math::max(Renderer::getDisplayWidthAsFloat() * 0.004f, 2.0f) // space between [icon] and [text] (px)
#define ENTRY_SPACING Math::max(Renderer::getDisplayWidthAsFloat() * 0.008f, 2.0f) // space between [text] and next [icon] (px)

static const HashMap<HelpType, const char*>& IconPathMap()
{
  static const HashMap<HelpType, const char*> _IconPathMap =
  {
    { HelpType::UpDown,            ":/help/dpad_updown.svg" },
    { HelpType::LeftRight,         ":/help/dpad_leftright.svg" },
    { HelpType::AllDirections,     ":/help/dpad_all.svg" },
    { HelpType::A,                 ":/help/button_a.svg" },
    { HelpType::B,                 ":/help/button_b.svg" },
    { HelpType::X,                 ":/help/button_x.svg" },
    { HelpType::Y,                 ":/help/button_y.svg" },
    { HelpType::L,                 ":/help/button_l.svg" },
    { HelpType::R,                 ":/help/button_r.svg" },
    { HelpType::LR,                ":/help/button_lr.svg" },
    { HelpType::L2R2,              ":/help/button_l2r2.svg" },
    { HelpType::Start,             ":/help/button_start.svg" },
    { HelpType::Select,            ":/help/button_select.svg" },
    { HelpType::Joy1UpDown,        ":/help/joystick_left_updown.svg" },
    { HelpType::Joy1LeftRight,     ":/help/joystick_left_leftright.svg" },
    { HelpType::Joy2UpDown,        ":/help/joystick_right_updown.svg" },
    { HelpType::Joy2LeftRight,     ":/help/joystick_right_leftright.svg" },
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
      {
        const char** found = IconPathMap().try_get(Help::TypeFromIndex(i));
        if (found != nullptr)
          icon->setImage(Path(*found));
      }

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
		mGrid->setColWidthPerc(col, icons[i]->getSize().x() / width);
		mGrid->setColWidthPerc(col + 1, ICON_TEXT_SPACING / width);
		mGrid->setColWidthPerc(col + 2, labels[i]->getSize().x() / width);
    mGrid->setColWidthPerc(col + 3, ENTRY_SPACING / width);

		mGrid->setEntry(icons[i], Vector2i(col, 0), false, false);
		mGrid->setEntry(labels[i], Vector2i(col + 2, 0), false, false);
	}

	mGrid->setPosition(Vector3f(HelpItemStyle().Position().x(), HelpItemStyle().Position().y(), 0.0f));
}

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
