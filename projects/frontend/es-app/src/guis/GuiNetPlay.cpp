//
// Created by xizor on 20/05/18.
//

#include "GuiNetPlay.h"
#include "components/TextComponent.h"
#include "components/ButtonComponent.h"
#include "components/MenuComponent.h" // for makeButtonGrid

#define BUTTON_GRID_VERT_PADDING 32
#define BUTTON_GRID_HORIZ_PADDING 10

#define TITLE_HEIGHT (mTitle->getFont()->getLetterHeight() + TITLE_VERT_PADDING)

using namespace Eigen;

GuiNetPlay::GuiNetPlay(Window* window) : GuiComponent(window),
                                        mBackground(window, ":/frame.png"), mGrid(window, Eigen::Vector2i(1, 3))
{
	addChild(&mBackground);
	addChild(&mGrid);

	auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();

	mBackground.setImagePath(menuTheme->menuBackground.path);
	mBackground.setCenterColor(menuTheme->menuBackground.color);
	mBackground.setEdgeColor(menuTheme->menuBackground.color);

	mTitle = std::make_shared<TextComponent>(mWindow, _("NETPLAY"), menuTheme->menuText.font, menuTheme->menuText.color, ALIGN_CENTER);
	mGrid.setEntry(mTitle, Eigen::Vector2i(0, 0), false);

	mList = std::make_shared<ComponentList>(mWindow);
	mGrid.setEntry(mList, Vector2i(0, 1), true);

	ComponentListRow row;
	std::shared_ptr<GuiComponent> ed;
	for (int i=0; i<15; i++) {
		row.elements.clear();
		ed = std::make_shared<TextComponent>(mWindow, "netplay", menuTheme->menuText.font, menuTheme->menuText.color, ALIGN_LEFT);
		row.addElement(ed, true);
		row.makeAcceptInputHandler([this] {
			mWindow->pushGui(new GuiMsgBoxScroll(
					mWindow, _("NETPLAY"),
					"fuck",
					_("OK"),
					[] {}, "", nullptr, "", nullptr, ALIGN_LEFT));
			return true;
		});
		addRow(row);
	}





	mGrid.resetCursor();

	mButtons.push_back(std::make_shared<ButtonComponent>(mWindow, _("BACK"), _("BACK"), [this] { delete this; }));

	mButtonGrid = makeButtonGrid(mWindow, mButtons);
	mGrid.setEntry(mButtonGrid, Eigen::Vector2i(0, 2), true, false, Eigen::Vector2i(1, 1), GridFlags::BORDER_TOP);

	updateSize();
	setPosition((Renderer::getScreenWidth() - mSize.x()) / 2, (Renderer::getScreenHeight() - mSize.y()) / 2);
}

void populateList(Window* window)
{


}

float GuiNetPlay::getButtonGridHeight() const
{
	auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
	return (mButtonGrid ? mButtonGrid->getSize().y() : menuTheme->menuText.font->getHeight() + BUTTON_GRID_VERT_PADDING);
}

bool GuiNetPlay::input(InputConfig* config, Input input)
{
	if (config->isMappedTo("a", input) && input.value != 0)
	{
		delete this;
	}
	return GuiComponent::input(config, input);
}

void GuiNetPlay::updateSize()
{
	const float maxHeight = Renderer::getScreenHeight() * 0.7f;
	float height = TITLE_HEIGHT + mList->getTotalRowHeight() + getButtonGridHeight() + 2;
	if(height > maxHeight)
	{
		height = TITLE_HEIGHT + getButtonGridHeight();
		int i = 0;
		while(i < mList->size())
		{
			float rowHeight = mList->getRowHeight(i);
			if(height + rowHeight < maxHeight)
				height += rowHeight;
			else
				break;
			i++;
		}
	}

	float width = std::min(Renderer::getScreenHeight(), (unsigned int) (Renderer::getScreenWidth() * 0.90f));
	setSize(width, height);
}

void GuiNetPlay::onSizeChanged()
{
	mBackground.fitTo(mSize, Eigen::Vector3f::Zero(), Eigen::Vector2f(-32, -32));

	// update grid row/col sizes
	mGrid.setRowHeightPerc(0, TITLE_HEIGHT / mSize.y());
	mGrid.setRowHeightPerc(2, getButtonGridHeight() / mSize.y());

	mGrid.setSize(mSize);
}

std::vector<HelpPrompt> GuiNetPlay::getHelpPrompts()
{
	return mGrid.getHelpPrompts();
}
