#include <RecalboxConf.h>
#include "views/gamelist/GridGameListView.h"
#include "Window.h"
#include "Settings.h"
#include "Locale.h"

GridGameListView::GridGameListView(Window&window, FolderData* root)
  : ISimpleGameListView(window, mSystemManager, root),
    mGrid(window)
{
	mGrid.setPosition(0, mSize.y() * 0.2f);
	mGrid.setSize(mSize.x(), mSize.y() * 0.8f);
	addChild(&mGrid);

	populateList(root);
}

void GridGameListView::setCursor(FileData* file)
{
	if(!mGrid.setCursor(file))
	{
		populateList(file->getParent());
		mGrid.setCursor(file);
	}
}

bool GridGameListView::ProcessInput(const InputCompactEvent& event)
{
	if (event.AnyLeftPressed() || event.AnyRightPressed())
		return Component::ProcessInput(event); // TODO: Check call!

	return ISimpleGameListView::ProcessInput(event);
}

void GridGameListView::populateList(const FolderData* folder)
{
	mGrid.clear();
	bool favoritesOnly = Settings::Instance().FavoritesOnly();
	FileData::List files = favoritesOnly ? folder->getAllFavorites(true) : folder->getAllDisplayableItems(true);
	for (FileData* fd : files)
	{
		mGrid.add(fd->getName(), fd->getThumbnailOrImagePath(), fd);
	}
}

bool GridGameListView::getHelpPrompts(Help& help)
{
	bool hideSystemView = RecalboxConf::Instance().AsBool("emulationstation.hidesystemview");

	help.Set(HelpType::AllDirections, _("SCROLL"))
	    .Set(HelpType::B, _("LAUNCH"));
	if(!hideSystemView)
	  help.Set(HelpType::A, _("BACK"));
	return true;
}
