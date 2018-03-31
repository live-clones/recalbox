//
// Created by xizor on 30/03/18.
//

#include "GuiInfoPopup.h"
#include "components/ComponentGrid.h"
#include "components/NinePatchComponent.h"
#include "components/TextComponent.h"
#include "Renderer.h"
#include <SDL_timer.h>
#include <MenuThemeData.h>
#include <components/ScrollableContainer.h>

GuiInfoPopup::GuiInfoPopup(Window* window, std::string message, int duration, std::string icon) :
		GuiComponent(window), mMessage(message), mDuration(duration * 1000), running(true)
{
	mFrame = new NinePatchComponent(window);

	float maxWidth = Renderer::getScreenWidth() * 0.2f;
	float maxHeight = Renderer::getScreenHeight() * 0.2f;

	auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();

	maxAlpha = menuTheme->menuBackground.color & 0xFF;
	mFrameColor = menuTheme->menuBackground.color;

	unsigned int FONT_SIZE_ICON = 0.05f * std::min(Renderer::getScreenHeight(), Renderer::getScreenWidth());
	unsigned int FONT_SIZE_TEXT = 0.02f * std::min(Renderer::getScreenHeight(), Renderer::getScreenWidth());

	auto iconText = std::make_shared<TextComponent>(mWindow, icon, Font::get(FONT_SIZE_ICON), menuTheme->menuText.color, ALIGN_CENTER);
	auto text = std::make_shared<TextComponent>(mWindow, message, Font::get(FONT_SIZE_TEXT), menuTheme->menuText.color, ALIGN_LEFT);

	mGrid = new ComponentGrid(window, Eigen::Vector2i(2, 1));

	mGrid->setEntry(iconText, Eigen::Vector2i(0, 0), false, false);

	mGrid->setEntry(text, Eigen::Vector2i(1, 0), false, false);



	text->setSize(maxWidth - iconText->getSize().y(), 0);

	const float msgHeight = std::min(maxHeight, std::max(text->getSize().y(), iconText->getSize().y()));


	// add a padding to the box
	int paddingX = (int) (Renderer::getScreenWidth() * 0.02f);
	int paddingY = (int) (Renderer::getScreenHeight() * 0.02f);

	mGrid->setColWidthPerc(0, (iconText->getFont()->getSize() + paddingX) / maxWidth);

	mGrid->setSize(maxWidth + paddingX, msgHeight + paddingY);

	float posX = Renderer::getScreenWidth()*0.98f - mGrid->getSize().x()*0.98f;
	float posY = Renderer::getScreenHeight() * 0.02f;

	setPosition(posX, posY, 0);

	mFrame->setImagePath(menuTheme->menuBackground.path);
	mFrame->setCenterColor(mFrameColor);
	mFrame->setEdgeColor(mFrameColor);
	mFrame->fitTo(mGrid->getSize(), Eigen::Vector3f::Zero(), Eigen::Vector2f(-32, -32));
	addChild(mFrame);

	// we only init the actual time when we first start to render
	mStartTime = 0;

	addChild(mGrid);
}

GuiInfoPopup::~GuiInfoPopup()
{
	delete mGrid;
	delete mFrame;
}

GuiInfoPopup::GuiInfoPopup(Window* window, std::string message, int duration) :
		GuiComponent(window), mMessage(message), mDuration(duration * 1000), running(true)
{
	mFrame = new NinePatchComponent(window);

	float maxWidth = Renderer::getScreenWidth() * 0.2f;
	float maxHeight = Renderer::getScreenHeight() * 0.2f;

	auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();

	maxAlpha = menuTheme->menuBackground.color & 0xFF;

	mFrameColor = menuTheme->menuBackground.color;

	unsigned int FONT_SIZE_TEXT = 0.02f * std::min(Renderer::getScreenHeight(), Renderer::getScreenWidth());

	auto text = std::make_shared<TextComponent>(mWindow, message, Font::get(FONT_SIZE_TEXT), menuTheme->menuText.color, ALIGN_LEFT);

	mGrid = new ComponentGrid(window, Eigen::Vector2i(1, 1));

	mGrid->setEntry(text, Eigen::Vector2i(0, 0), false, false);



	text->setSize(maxWidth, 0);

	const float msgHeight = std::min(maxHeight, text->getSize().y());


	// add a padding to the box
	int paddingX = (int) (Renderer::getScreenWidth() * 0.02f);
	int paddingY = (int) (Renderer::getScreenHeight() * 0.02f);

	mGrid->setSize(maxWidth + paddingX, msgHeight + paddingY);

	float posX = Renderer::getScreenWidth()*0.98f - mGrid->getSize().x()*0.98f;
	float posY = Renderer::getScreenHeight() * 0.02f;

	setPosition(posX, posY, 0);

	mFrame->setImagePath(menuTheme->menuBackground.path);
	mFrame->setCenterColor(mFrameColor);
	mFrame->setEdgeColor(mFrameColor);
	mFrame->fitTo(mGrid->getSize(), Eigen::Vector3f::Zero(), Eigen::Vector2f(-32, -32));
	addChild(mFrame);

	// we only init the actual time when we first start to render
	mStartTime = 0;

	addChild(mGrid);
}

void GuiInfoPopup::render(const Eigen::Affine3f& parentTrans)
{
	// we use identity as we want to render on a specific window position, not on the view
	Eigen::Affine3f trans = getTransform() * Eigen::Affine3f::Identity();
	if(running && updateState())
	{
		// if we're still supposed to be rendering it
		Renderer::setMatrix(trans);
		renderChildren(trans);
	}
}

bool GuiInfoPopup::updateState()
{
	int curTime = SDL_GetTicks();

	// we only init the actual time when we first start to render
	if(mStartTime == 0)
	{
		mStartTime = curTime;
	}

	// compute fade in effect
	if (curTime - mStartTime > mDuration)
	{
		// we're past the popup duration, no need to render
		running = false;
		return false;
	}
	else if (curTime < mStartTime) {
		// if SDL reset
		running = false;
		return false;
	}
	else if (curTime - mStartTime <= 500) {
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

	mGrid->setOpacity(alpha);

	// apply fade in effect to popup frame
	mFrame->setEdgeColor((mFrameColor & 0xffffff00) | alpha);
	mFrame->setCenterColor((mFrameColor & 0xffffff00) | alpha);
	return true;
}