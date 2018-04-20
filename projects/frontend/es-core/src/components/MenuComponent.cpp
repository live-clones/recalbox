#include <Settings.h>
#include <recalbox/RecalboxSystem.h>
#include "components/MenuComponent.h"
#include "components/ButtonComponent.h"
#include "Locale.h"
#include "MenuThemeData.h"

#define BUTTON_GRID_VERT_PADDING 32
#define BUTTON_GRID_HORIZ_PADDING 10

#define TITLE_HEIGHT (mTitle->getFont()->getLetterHeight() + TITLE_VERT_PADDING)

using namespace Eigen;

MenuComponent::MenuComponent(Window* window, const char* title, const std::shared_ptr<Font>& titleFont) : GuiComponent(window), 
	mBackground(window), mGrid(window, Vector2i(1, 3))
{	
	addChild(&mBackground);
	addChild(&mGrid);
		
	auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
	
	mBackground.setImagePath(menuTheme->menuBackground.path);
	mBackground.setCenterColor(menuTheme->menuBackground.color);
	mBackground.setEdgeColor(menuTheme->menuBackground.color);

	// set up title
	mTitle = std::make_shared<TextComponent>(mWindow);
	mTitle->setHorizontalAlignment(ALIGN_CENTER);
		
	setTitle(title, menuTheme->menuTitle.font);
	mTitle->setColor(menuTheme->menuTitle.color);

	mGrid.setEntry(mTitle, Vector2i(0, 0), false);
	


	if (title == _("MAIN MENU") ) {

		auto headerGrid = std::make_shared<ComponentGrid>(mWindow, Vector2i(7, 1));

		std::string arch = Settings::getInstance()->getString("Arch");
		if (arch == "x86" || arch == "x86_64") {

			auto batt = RecalboxSystem::getInstance()->getSysBatteryInfo();
			if (batt.second != -1) {
				auto batDisplay = std::make_shared<TextComponent>(mWindow);
				batDisplay->setFont(menuTheme->menuText.font);
				if (batt.second <= 15)
					batDisplay->setColor(0xFF0000FF);
				else
					batDisplay->setColor(menuTheme->menuText.color);
				batDisplay->setText(batt.first + " " + std::to_string(batt.second) + "%");
				batDisplay->setHorizontalAlignment(ALIGN_CENTER);
				headerGrid->setEntry(batDisplay, Vector2i(0, 0), false);

			}
		}

		if (Settings::getInstance()->getBool("ShowClock")) {

			mDateTime = std::make_shared<DateTimeComponent>(mWindow);
			mDateTime->setDisplayMode(DateTimeComponent::DISP_TIME);
			mDateTime->setFont(menuTheme->menuText.font);
			mDateTime->setColor(menuTheme->menuText.color);
			headerGrid->setEntry(mDateTime, Vector2i(6, 0), false);
		}

	    mGrid.setEntry(headerGrid, Vector2i(0, 0), false);

	}


	// set up list which will never change (externally, anyway)
	mList = std::make_shared<ComponentList>(mWindow);
	mGrid.setEntry(mList, Vector2i(0, 1), true);

	updateGrid();
	updateSize();

	mGrid.resetCursor();
}

void MenuComponent::setTitle(const char* title, const std::shared_ptr<Font>& font)
{
	mTitle->setText(strToUpper(title));
	mTitle->setFont(font);
}

float MenuComponent::getButtonGridHeight() const
{
	auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
	return (mButtonGrid ? mButtonGrid->getSize().y() : menuTheme->menuText.font->getHeight() + BUTTON_GRID_VERT_PADDING);
}

void MenuComponent::updateSize()
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

void MenuComponent::onSizeChanged()
{
	mBackground.fitTo(mSize, Eigen::Vector3f::Zero(), Eigen::Vector2f(-32, -32));

	// update grid row/col sizes
	mGrid.setRowHeightPerc(0, TITLE_HEIGHT / mSize.y());
	mGrid.setRowHeightPerc(2, getButtonGridHeight() / mSize.y());
	
	mGrid.setSize(mSize);
}

void MenuComponent::addButton(const std::string& name, const std::string& helpText, const std::function<void()>& callback)
{
	mButtons.push_back(std::make_shared<ButtonComponent>(mWindow, strToUpper(name), helpText, callback));
	updateGrid();
	updateSize();
}

void MenuComponent::updateGrid()
{
	if(mButtonGrid)
		mGrid.removeEntry(mButtonGrid);

	mButtonGrid.reset();

	if(mButtons.size())
	{
		mButtonGrid = makeButtonGrid(mWindow, mButtons);
		mGrid.setEntry(mButtonGrid, Vector2i(0, 2), true, false);
	}
}

std::vector<HelpPrompt> MenuComponent::getHelpPrompts()
{
	return mGrid.getHelpPrompts();
}

std::shared_ptr<ComponentGrid> makeButtonGrid(Window* window, const std::vector< std::shared_ptr<ButtonComponent> >& buttons)
{
	std::shared_ptr<ComponentGrid> buttonGrid = std::make_shared<ComponentGrid>(window, Vector2i(buttons.size(), 2));

	float buttonGridWidth = (float)BUTTON_GRID_HORIZ_PADDING * buttons.size(); // initialize to padding
	for(int i = 0; i < (int)buttons.size(); i++)
	{
		buttonGrid->setEntry(buttons.at(i), Vector2i(i, 0), true, false);
		buttonGridWidth += buttons.at(i)->getSize().x();
	}
	for(unsigned int i = 0; i < buttons.size(); i++)
	{
		buttonGrid->setColWidthPerc(i, (buttons.at(i)->getSize().x() + BUTTON_GRID_HORIZ_PADDING) / buttonGridWidth);
	}
	
	buttonGrid->setSize(buttonGridWidth, buttons.at(0)->getSize().y() + BUTTON_GRID_VERT_PADDING + 2);
	buttonGrid->setRowHeightPerc(1, 2 / buttonGrid->getSize().y()); // spacer row to deal with dropshadow to make buttons look centered

	return buttonGrid;
}

std::shared_ptr<ImageComponent> makeArrow(Window* window)
{
	auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
	auto bracket = std::make_shared<ImageComponent>(window);
	bracket->setImage(menuTheme->iconSet.arrow);
	bracket->setColorShift(menuTheme->menuText.color);
	bracket->setResize(0, round(menuTheme->menuText.font->getLetterHeight()));
	
	return bracket;
}
