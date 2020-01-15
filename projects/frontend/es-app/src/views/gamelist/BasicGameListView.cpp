#include <RecalboxConf.h>
#include "views/gamelist/BasicGameListView.h"
#include "views/ViewController.h"
#include "Renderer.h"
#include "themes/ThemeData.h"
#include "systems/SystemData.h"
#include "FileSorts.h"
#include "Settings.h"
#include "LocaleHelper.h"
#include "SystemIcons.h"
#include <recalbox/RecalboxSystem.h>

BasicGameListView::BasicGameListView(Window& window, SystemManager& systemManager, FolderData* root)
	: ISimpleGameListView(window, systemManager, root),
	  mList(window),
    mEmptyListItem(root->getSystem()),
    mPopulatedFolder(nullptr),
	  listingOffset(0)
{
	mList.setSize(mSize.x(), mSize.y() * 0.8f);
	mList.setPosition(0, mSize.y() * 0.2f);
	mList.setDefaultZIndex(20);
	addChild(&mList);

	mEmptyListItem.Metadata().SetName(_("EMPTY LIST"));

	populateList(root);
}

void BasicGameListView::onThemeChanged(const ThemeData& theme)
{
	ISimpleGameListView::onThemeChanged(theme);
	mList.applyTheme(theme, getName(), "gamelist", ThemeProperties::All);
	sortChildren();
}

void BasicGameListView::onFileChanged(FileData* file, FileChangeType change)
{
	ISimpleGameListView::onFileChanged(file, change);

	if(change == FileChangeType::MetadataChanged)
	{
		// might switch to a detailed view
    ViewController::Instance().reloadGameListView(this);
		return;
	}
}

const char * BasicGameListView::getItemIcon(FileData* item)
{
	if (item->Metadata().Hidden()) return "\uF070 ";
	if ((item->isGame()) && (item->Metadata().Favorite()))
		return SystemIcons::GetIcon(item->getSystem()->getName());

	return nullptr;
}

void BasicGameListView::populateList(const FolderData* folder)
{
	mPopulatedFolder = folder;
	mList.clear();
	mHeaderText.setText(mSystem->getFullName());

  // Default filter
  FileData::Filter filter = FileData::Filter::Normal | FileData::Filter::Favorite;
  // Add hidden?
  if (Settings::Instance().ShowHidden())
    filter |= FileData::Filter::Hidden;
  // Favorites only?
  if (mFavoritesOnly)
    filter = FileData::Filter::Favorite;

  // Get items
  bool flatfolders = (RecalboxConf::Instance().AsBool(getRoot()->getSystem()->getName() + ".flatfolder"));
  int count = flatfolders ? folder->countFilteredItemsRecursively(filter, false) : folder->countFilteredItems(filter, true);
  if (count == 0) filter |= FileData::Filter::Normal;
	FileData::List items = flatfolders ? folder->getFilteredItemsRecursively(filter, false) : folder->getFilteredItems(filter, true);
	// Check emptyness
	if (items.empty())
  {
	  // Insert "EMPTY SYSTEM" item
    items.push_back(&mEmptyListItem);
  }

  // Sort
  const FileSorts::SortType& sortType = mSystem->getSortType(mSystem->isFavorite());
  FolderData::Sort(items, sortType.comparisonFunction, sortType.ascending);

  // Add to list
  //mList.reserve(items.size()); // TODO: Reserve memory once
  for (FileData* fd : items)
	{
    // Select fron icon
    const char* icon = getItemIcon(fd);
  	// Get name
  	std::string name = icon != nullptr ? icon + fd->getName() : fd->getName();
    // Store
		mList.add(name, fd, fd->isFolder() ? 1 : 0, false);
	}
}

FileData::List BasicGameListView::getFileDataList()
{
	FileData::List objects = mList.getObjects();
	FileData::List slice;
  for (auto it = objects.begin() + listingOffset; it != objects.end(); it++)
  {
    slice.push_back(*it);
  }
  return slice;
}

void BasicGameListView::setCursorIndex(int index)
{
  if (index >= mList.size()) index = mList.size() - 1;
  if (index < 0) index = 0;

  RecalboxSystem::NotifyGame(*getCursor(), false, false);
	mList.setCursorIndex(index);
}

void BasicGameListView::setCursor(FileData* cursor)
{
	if(!mList.setCursor(cursor, listingOffset))
	{
		populateList(mRoot);
		mList.setCursor(cursor);

		// update our cursor stack in case our cursor just got set to some folder we weren't in before
		if(mCursorStack.empty() || mCursorStack.top() != cursor->getParent())
		{
			std::stack<FolderData*> tmp;
			FolderData* ptr = cursor->getParent();
			while((ptr != nullptr) && ptr != mRoot)
			{
				tmp.push(ptr);
				ptr = ptr->getParent();
			}
			
			// flip the stack and put it in mCursorStack
			mCursorStack = std::stack<FolderData*>();
			while(!tmp.empty())
			{
				mCursorStack.push(tmp.top());
				tmp.pop();
			}
		}
	}
  RecalboxSystem::NotifyGame(*getCursor(), false, false);
}
