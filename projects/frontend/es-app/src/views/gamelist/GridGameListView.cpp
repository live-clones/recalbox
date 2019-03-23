#include <RecalboxConf.h>
#include "views/gamelist/GridGameListView.h"
#include "ThemeData.h"
#include "Window.h"
#include "views/ViewController.h"
#include "Settings.h"
#include "Locale.h"

GridGameListView::GridGameListView(Window* window, FolderData* root)
  : ISimpleGameListView(window, root),
    mGrid(window)
{
	mGrid.setPosition(0, mSize.y() * 0.2f);
	mGrid.setSize(mSize.x(), mSize.y() * 0.8f);
	addChild(&mGrid);

	populateList(root);
}

FileData* GridGameListView::getCursor()
{
	return mGrid.getSelected();
}

void GridGameListView::setCursor(FileData* file)
{
	if(!mGrid.setCursor(file))
	{
		populateList(file->getParent());
		mGrid.setCursor(file);
	}
}

bool GridGameListView::input(InputConfig* config, Input input)
{
	if(config->isMappedTo("left", input) || config->isMappedTo("right", input))
		return GuiComponent::input(config, input);

	return ISimpleGameListView::input(config, input);
}

void GridGameListView::populateList(const FolderData* folder)
{
	mGrid.clear();
	bool favoritesOnly = Settings::getInstance()->getBool("FavoritesOnly");
	FileData::List files = favoritesOnly ? folder->getAllFavorites(true) : folder->getAllDisplayableItems(true);
	for (FileData* fd : files)
	{
		mGrid.add(fd->getName(), fd->getThumbnailOrImagePath(), fd);
	}
}

void GridGameListView::launch(FileData* game)
{
	ViewController::get()->launch(game);
}

std::vector<HelpPrompt> GridGameListView::getHelpPrompts()
{
	bool hideSystemView = RecalboxConf::getInstance()->get("emulationstation.hidesystemview") == "1";

	std::vector<HelpPrompt> prompts;
	prompts.push_back(HelpPrompt("up/down/left/right", _("SCROLL")));
	prompts.push_back(HelpPrompt("b", _("LAUNCH")));
	if(!hideSystemView)
	  prompts.push_back(HelpPrompt("a", _("BACK")));
	return prompts;
}
