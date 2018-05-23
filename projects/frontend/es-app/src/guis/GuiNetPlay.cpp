//
// Created by xizor on 20/05/18.
//

#include "GuiNetPlay.h"
#include "components/TextComponent.h"
#include "components/ButtonComponent.h"
#include "components/MenuComponent.h"


#define BUTTON_GRID_VERT_PADDING 32
#define BUTTON_GRID_HORIZ_PADDING 10

#define TITLE_HEIGHT (mTitle->getFont()->getLetterHeight() + TITLE_VERT_PADDING)

using namespace Eigen;

GuiNetPlay::GuiNetPlay(Window* window) : GuiComponent(window),
                                        mBackground(window, ":/frame.png"), mGrid(window, Eigen::Vector2i(1, 3)),
                                         mList(NULL), mGridMeta(new ComponentGrid(window, Eigen::Vector2i(2, 1)))
{
	addChild(&mBackground);
	addChild(&mGrid);

	auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();

	mBackground.setImagePath(menuTheme->menuBackground.path);
	mBackground.setCenterColor(menuTheme->menuBackground.color);
	mBackground.setEdgeColor(menuTheme->menuBackground.color);

	mTitle = std::make_shared<TextComponent>(mWindow, _("NETPLAY"), menuTheme->menuText.font, menuTheme->menuText.color, ALIGN_CENTER);
	mGrid.setEntry(mTitle, Eigen::Vector2i(0, 0), false);

	if (parseLobby()) {
		mList = std::make_shared<ComponentList>(mWindow);
		mGridMeta->setEntry(mList, Vector2i(0, 0), true);
		mGrid.setEntry(mGridMeta, Vector2i(0, 1), true);

		mMetaText = std::make_shared<TextComponent>(mWindow, "", menuTheme->menuText.font, menuTheme->menuText.color, ALIGN_LEFT);
		mGridMeta->setEntry(mMetaText, Vector2i(1, 0), true);

		ComponentListRow row;
		std::shared_ptr<GuiComponent> ed;
		for (auto v : mRooms) {
			row.elements.clear();
			ed = std::make_shared<TextComponent>(mWindow, v.second.get<std::string>("fields.game_name"), menuTheme->menuText.font, menuTheme->menuText.color, ALIGN_LEFT);
			row.addElement(ed, true);
			row.makeAcceptInputHandler([this] { launch();});
			addRow(row);
		}
		populateGridMeta(0);
		mList->setCursorChangedCallback([this](CursorState state){populateGridMeta(mList->getCursor());});
	} else {
		auto text = std::make_shared<TextComponent>(mWindow, _("NO GAMES OR NO CONNECTION"), menuTheme->menuText.font, menuTheme->menuText.color, ALIGN_CENTER);
		mGrid.setEntry(text, Vector2i(0, 1), true);
	}

	mGrid.resetCursor();

	mButtons.push_back(std::make_shared<ButtonComponent>(mWindow, _("BACK"), _("BACK"), [this] { delete this; }));

	mButtonGrid = makeButtonGrid(mWindow, mButtons);
	mGrid.setEntry(mButtonGrid, Eigen::Vector2i(0, 2), true, false, Eigen::Vector2i(1, 1), GridFlags::BORDER_TOP);

	updateSize();
	setPosition((Renderer::getScreenWidth() - mSize.x()) / 2, (Renderer::getScreenHeight() - mSize.y()) / 2);

}

void GuiNetPlay::populateGridMeta(int i)
{
	std::string text = "    " + _("Core : ") + mRooms[i].second.get<std::string>("fields.core_name");
	text += "\n    " + _("Country : ") + mRooms[i].second.get<std::string>("fields.country");
	mMetaText->setText(text);
}

void GuiNetPlay::launch()
{
	mWindow->pushGui(new GuiMsgBoxScroll(
			mWindow, _("NETPLAY"),
			mRooms[mList->getCursor()].second.get<std::string>("fields.game_name"),
			_("OK"),
			[] {}, "", nullptr, "", nullptr, ALIGN_CENTER));
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
	float height;
	if (mList) {
		height = TITLE_HEIGHT + mList->getTotalRowHeight() + getButtonGridHeight() + 2;
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
	} else {
		height = maxHeight;
	}


	const float width = Renderer::getScreenWidth() * 0.90f;
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

bool GuiNetPlay::parseLobby()
{
	mRooms.clear();
	auto json_req = RecalboxSystem::getInstance()->execute("curl -s http://lobby.libretro.com/list/");
	if (json_req.second == 0) {
		json::ptree root;
		std::stringstream ss;
		ss << json_req.first;
		json::read_json(ss, root);

		for (json::ptree::value_type &array_element : root) {
			//std::cout << array_element.second.get<std::string>("fields.username") << std::endl;
			mRooms.push_back(array_element);
		}
		return true;
	} else {
		return false;
	}
}