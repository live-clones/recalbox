//
// Created by bkg2k on 26/03/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include "GuiInfoPopup.h"
#include <themes/MenuThemeData.h>
#include <components/TextComponent.h>

GuiInfoPopup::GuiInfoPopup(WindowManager& window, const std::string& message, int duration, GuiInfoPopupBase::PopupType icon)
  : GuiInfoPopupBase(window, false, duration, icon, 2, 1, 1.f)
  , mMessage(message)
  , mIcon(icon)
{
}

float GuiInfoPopup::AddComponents(WindowManager& window, ComponentGrid& grid, float maxWidth, float maxHeight, int paddingX, int paddingY)
{
  (void)paddingY;

  std::string iconText;
  switch (mIcon)
  {
    case PopupType::Music   : iconText = "\uF1b0"; break;
    case PopupType::Warning : iconText = "\uF1ca"; break;
    case PopupType::Help    : iconText = "\uF1c1"; break;
    case PopupType::Netplay : iconText = "\uF1c4"; break;
    case PopupType::Recalbox: iconText = "\uF200"; break;
    case PopupType::Pads    : iconText = "\uF2ee"; break;
    case PopupType::Reboot  : iconText = "\uF006"; break;
    case PopupType::Scraper : iconText = "\uF1e4"; break;
    case PopupType::None:
    default: break;
  }

  auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
  float hwSize = Math::min(Renderer::Instance().DisplayHeightAsFloat(), Renderer::Instance().DisplayWidthAsFloat());
  unsigned int FONT_SIZE_ICON = (unsigned int)((Renderer::Instance().IsSmallResolution() ? 0.08f : 0.04f) * hwSize);
  unsigned int FONT_SIZE_TEXT = (unsigned int)((Renderer::Instance().IsSmallResolution() ? 0.04f : 0.02f) * hwSize);

  mMsgText = std::make_shared<TextComponent>(window, mMessage, Font::get((int)FONT_SIZE_TEXT), menuTheme->menuText.color, TextAlignment::Left);
  mMsgIcon = std::make_shared<TextComponent>(window, iconText, Font::get((int)FONT_SIZE_ICON), menuTheme->menuText.color, TextAlignment::Left);

  grid.setEntry(mMsgText, Vector2i(1, 0), false, false);
  grid.setEntry(mMsgIcon, Vector2i(0, 0), false, false);

  float msgHeight = 0.0f;
  if (mIcon == PopupType::None)
  {
    mMsgText->setSize(maxWidth, 0);
    msgHeight = Math::min(maxHeight, mMsgText->getSize().y());
    grid.setColWidthPerc(1,1);
  }
  else
  {
    mMsgText->setSize(maxWidth - mMsgIcon->getSize().y(), 0);
    msgHeight = Math::min(maxHeight, Math::max(mMsgText->getSize().y(), mMsgIcon->getSize().y()));
    grid.setColWidthPerc(0, (float)(mMsgIcon->getFont()->getSize() + paddingX) / maxWidth);
  }

  return msgHeight;
}
