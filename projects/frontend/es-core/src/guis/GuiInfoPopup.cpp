//
// Created by xizor on 30/03/18.
//

#include "GuiInfoPopup.h"
#include "components/TextComponent.h"
#include "Renderer.h"
#include <SDL_timer.h>
#include <themes/MenuThemeData.h>
#include <components/ScrollableContainer.h>

GuiInfoPopup::GuiInfoPopup(WindowManager&window, const std::string& message, int duration, PopupType icon)
  : Gui(window),
    mGrid(window, Vector2i(2, 1)),
    mFrame(window, Path(":/frame.png")),
    mType(icon),
    mTargetOffset(0),
    mDuration(duration * 1000),
    mRunning(true)
{
	float maxWidth = Renderer::Instance().DisplayWidthAsFloat() * (Renderer::Instance().IsSmallResolution() ? 0.4f : 0.2f);
	float maxHeight = Renderer::Instance().DisplayHeightAsFloat() * (Renderer::Instance().IsSmallResolution() ? 0.5f : 0.4f);

	auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();

	maxAlpha = (int)menuTheme->menuBackground.color & 0xFF;
	mFrameColor = menuTheme->menuBackground.color;

	float hwSize = Math::min(Renderer::Instance().DisplayHeightAsFloat(), Renderer::Instance().DisplayWidthAsFloat());
	unsigned int FONT_SIZE_ICON = (unsigned int)((Renderer::Instance().IsSmallResolution() ? 0.08f : 0.04f) * hwSize);
	unsigned int FONT_SIZE_TEXT = (unsigned int)((Renderer::Instance().IsSmallResolution() ? 0.04f : 0.02f) * hwSize);

	// add a padding to the box
	int paddingX = (int) (Renderer::Instance().DisplayWidthAsFloat() * 0.02f);
	int paddingY = (int) (Renderer::Instance().DisplayHeightAsFloat() * 0.02f);

	std::string iconText;
  switch (icon)
  {
    case PopupType::Music   : iconText = "\uF1b0"; break;
    case PopupType::Help    : iconText = "\uF1c1"; break;
    case PopupType::Netplay : iconText = "\uF1c4"; break;
    case PopupType::Recalbox: iconText = "\uF200"; break;
    case PopupType::Pads    : iconText = "\uF2ee"; break;
    case PopupType::Reboot  : iconText = "\uF006"; break;
    case PopupType::None:
    default: break;
  }

  mMsgText = std::make_shared<TextComponent>(window, message, Font::get((int)FONT_SIZE_TEXT), menuTheme->menuText.color, TextAlignment::Left);
	mMsgIcon = std::make_shared<TextComponent>(window, iconText, Font::get((int)FONT_SIZE_ICON), menuTheme->menuText.color, TextAlignment::Left);

	mGrid.setEntry(mMsgText, Vector2i(1, 0), false, false);
	mGrid.setEntry(mMsgIcon, Vector2i(0, 0), false, false);

  float msgHeight = 0.0f;
	if (icon == PopupType::None)
	{
		mMsgText->setSize(maxWidth, 0);
		msgHeight = Math::min(maxHeight, mMsgText->getSize().y());
		mGrid.setColWidthPerc(1,1);
	}
	else
	{
		mMsgText->setSize(maxWidth - mMsgIcon->getSize().y(), 0);
		msgHeight = Math::min(maxHeight, Math::max(mMsgText->getSize().y(), mMsgIcon->getSize().y()));
		mGrid.setColWidthPerc(0, (float)(mMsgIcon->getFont()->getSize() + paddingX) / maxWidth);
	}
	mGrid.setSize(maxWidth + (float)paddingX, msgHeight + (float)paddingY);

	float posX = 0.0f, posY = 0.0f;

  mCorner = Corner::TopRight;
  posX = Renderer::Instance().DisplayWidthAsFloat() * 0.98f - mGrid.getSize().x() * 0.98f;
  posY = Renderer::Instance().DisplayHeightAsFloat();

	setPosition(posX, posY, 0);
  setSize(mGrid.getSize());

	mFrame.setImagePath(menuTheme->menuBackground.path);
	mFrame.setCenterColor(mFrameColor);
	mFrame.setEdgeColor(mFrameColor);
	mFrame.fitTo(mGrid.getSize(), Vector3f::Zero(), Vector2f(-32, -32));
	addChild(&mFrame);

	// we only init the actual time when we first start to render
	mStartTime = 0;

	addChild(&mGrid);
}

void GuiInfoPopup::Render(const Transform4x4f& parentTrans)
{
  (void)parentTrans;

	// we use identity as we want to render on a specific window position, not on the view
	Transform4x4f trans = getTransform() * Transform4x4f::Identity();

  Renderer::SetMatrix(trans);
  renderChildren(trans);
}

void GuiInfoPopup::Update(int delta)
{
  updateChildren(delta);

  // Update position
  switch(mCorner)
  {
    case Corner::TopRight:
    case Corner::TopLeft:
    {
      float targetY = Renderer::Instance().DisplayHeightAsFloat() * 0.02f + (float)mTargetOffset;
      float diff = (mPosition.y() - targetY) * .90f;
      if (diff >= -2.0f && diff <= 2.0f) diff = 0;
      mPosition.y() = targetY + diff;
      break;
    }
    case Corner::BottomRight:
    case Corner::BottomLeft:
    {
      float targetY = Renderer::Instance().DisplayHeightAsFloat() * 0.98f - mSize.y() - (float)mTargetOffset;
      float diff = (mPosition.y() - targetY) * .90f;
      if (diff >= -2.0f && diff <= 2.0f) diff = 0;
      mPosition.y() = targetY + diff;
      break;
    }
  }

  // Update fade and timer
	int curTime = SDL_GetTicks();

	// we only init the actual time when we first start to render
	if(mStartTime == 0)
	{
		mStartTime = curTime;
	}

	int alpha = 0;

	// compute fade in effect
	if ((curTime - mStartTime > mDuration) || // we're past the popup duration, no need to render
	    (curTime < mStartTime))               // if SDL reset
	{
		mRunning = false;
		return;
	}
	else if (curTime - mStartTime <= 500)
	{
		alpha = ((curTime - mStartTime)*255/500);
	}
	else if (curTime - mStartTime < mDuration - 500)
	{
		alpha = 255;
	}
	else
	{
		alpha = ((-(curTime - mStartTime - mDuration)*255)/500);
	}

	if (alpha > maxAlpha)
		alpha = maxAlpha;

	mGrid.setOpacity(alpha);

	// apply fade in effect to popup frame
	mFrame.setEdgeColor((mFrameColor & 0xffffff00) | alpha);
	mFrame.setCenterColor((mFrameColor & 0xffffff00) | alpha);
}

void GuiInfoPopup::SlideOffset(int size)
{
  // Update position
  switch(mCorner)
  {
    case Corner::TopRight:
    case Corner::TopLeft:
    {
      mTargetOffset -= size;
      break;
    }
    case Corner::BottomRight:
    case Corner::BottomLeft:
    {
      mTargetOffset += size;
      break;
    }
  }
}


