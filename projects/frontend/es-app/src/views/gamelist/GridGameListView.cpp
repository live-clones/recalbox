#include <RecalboxConf.h>
#include "views/gamelist/GridGameListView.h"
#include "WindowManager.h"
#include "utils/locale/LocaleHelper.h"

GridGameListView::GridGameListView(WindowManager&window, SystemData& system)
  : ISimpleGameListView(window, mSystemManager, system),
    mGrid(window)
{
	mGrid.setPosition(0, mSize.y() * 0.2f);
	mGrid.setSize(mSize.x(), mSize.y() * 0.8f);
	addChild(&mGrid);

	populateList(system.MasterRoot());
}

void GridGameListView::setCursor(FileData* file)
{
	if(!mGrid.setCursor(file))
	{
		populateList(*file->Parent());
		mGrid.setCursor(file);
	}
}

bool GridGameListView::ProcessInput(const InputCompactEvent& event)
{
	if (event.AnyLeftPressed() || event.AnyRightPressed())
		return Component::ProcessInput(event); // TODO: Check call!

	return ISimpleGameListView::ProcessInput(event);
}

void GridGameListView::populateList(const FolderData& folder)
{
	mGrid.clear();
	bool favoritesOnly = RecalboxConf::Instance().GetFavoritesOnly();
	FileData::List files = favoritesOnly ? folder.GetAllFavorites(true, folder.System().IncludeAdultGames()) : folder.GetAllDisplayableItems(true,
                                                                                                                           folder.System().IncludeAdultGames());
	for (FileData* fd : files)
	{
		mGrid.add(fd->Name(), fd->ThumbnailOrImagePath(), fd);
	}
}

bool GridGameListView::getHelpPrompts(Help& help)
{
	bool hideSystemView = RecalboxConf::Instance().GetStartupHideSystemView();

	help.Set(HelpType::AllDirections, _("SCROLL"))
	    .Set(Help::Valid(), _("LAUNCH"));
	if(!hideSystemView)
	  help.Set(Help::Cancel(), _("BACK"));
	return true;
}