//
// Created by xizor on 20/05/18.
//

#include "GuiNetPlay.h"
#include "Renderer.h"
#include "components/TextComponent.h"
#include "components/ButtonComponent.h"
#include "components/MenuComponent.h" // for makeButtonGrid
#include "Util.h"
#include "Log.h"
#include "MenuThemeData.h"

#define HORIZONTAL_PADDING_PX 20

GuiNetPlay::GuiNetPlay(Window* window, FileData* file) : GuiComponent(window),
                                        mBackground(window, ":/frame.png"), mGrid(window, Eigen::Vector2i(1, 4))
{
	float width = Renderer::getScreenWidth() * 0.6f; // max width
	float minWidth = Renderer::getScreenWidth() * 0.3f; // minimum width

	auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();

	mBackground.setImagePath(menuTheme->menuBackground.path);
	mBackground.setCenterColor(menuTheme->menuBackground.color);
	mBackground.setEdgeColor(menuTheme->menuBackground.color);
	mTitle = std::make_shared<TextComponent>(mWindow, _("NETPLAY"), menuTheme->menuText.font, menuTheme->menuText.color, ALIGN_CENTER);
	mSubTitle = std::make_shared<TextComponent>(mWindow, file->getName(), menuTheme->menuText.font, menuTheme->menuText.color, ALIGN_CENTER);
	mGrid.setEntry(mTitle, Eigen::Vector2i(0, 0), false, false);
	mGrid.setEntry(mSubTitle, Eigen::Vector2i(0, 1), false, false);

	mButtons.push_back(std::make_shared<ButtonComponent>(mWindow, _("BACK"), _("BACK"), [this] { delete this; }));

	mButtonGrid = makeButtonGrid(mWindow, mButtons);
	mGrid.setEntry(mButtonGrid, Eigen::Vector2i(0, 3), true, false, Eigen::Vector2i(1, 1), GridFlags::BORDER_TOP);

	// decide final width
	if(mSubTitle->getSize().x() < width && mButtonGrid->getSize().x() < width)
	{
		// mMsg and buttons are narrower than width
		width = std::max(mButtonGrid->getSize().x(), mSubTitle->getSize().x());
		width = std::max(width, minWidth);
	}
	mSubTitle->setSize(width, 0);

	setSize(width + HORIZONTAL_PADDING_PX*2, Renderer::getScreenHeight() * 0.6f);

	// center for good measure
	setPosition((Renderer::getScreenWidth() - mSize.x()) / 2.0f, (Renderer::getScreenHeight() - mSize.y()) / 2.0f);

	addChild(&mBackground);
	addChild(&mGrid);
}

bool GuiNetPlay::input(InputConfig* config, Input input)
{
	if (config->isMappedTo("a", input) && input.value != 0)
	{
		delete this;
	}
	return GuiComponent::input(config, input);
}

void GuiNetPlay::onSizeChanged()
{
	mGrid.setSize(mSize);
	mGrid.setRowHeightPerc(1, mButtonGrid->getSize().y() / mSize.y());

	// update messagebox size
	mSubTitle->setSize(mSize.x() - HORIZONTAL_PADDING_PX*2, mGrid.getRowHeight(0));
	mGrid.onSizeChanged();

	mBackground.fitTo(mSize, Eigen::Vector3f::Zero(), Eigen::Vector2f(-32, -32));
}

std::vector<HelpPrompt> GuiNetPlay::getHelpPrompts()
{
	return mGrid.getHelpPrompts();
}