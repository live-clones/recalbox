#pragma once

#include "views/gamelist/ISimpleGameListView.h"
#include "components/TextListComponent.h"
#include "EmptyData.h"

class BasicGameListView : public ISimpleGameListView
{
public:
	BasicGameListView(Window* window, FolderData* root);

	// Called when a FileData* is added, has its metadata changed, or is removed
	void onFileChanged(FileData* file, FileChangeType change) override;

	void onThemeChanged(const std::shared_ptr<ThemeData>& theme) override;

	FileData* getCursor() override;
	int getCursorIndex() override;
	int getCursorIndexMax() override;
	void setCursor(FileData* file) override;
	void setCursorIndex(int index) override;

	const char* getName() const override { return "basic"; }

	void populateList(const FolderData* folder) override;
	void refreshList() override;

	inline void updateInfoPanel() override {}

	FileData::List getFileDataList() override;

protected:
	void launch(FileData* game) override;

  FileData* getEmptyListItem() override { return &mEmptyListItem; }

  TextListComponent<FileData*> mList;
private:
  EmptyData mEmptyListItem;
  const FolderData *mPopulatedFolder;
  unsigned long listingOffset;

	/*void addFavorites(const FileData::List& files, const FileSorts::SortType& sortType);
	void getFavorites(const FileData::List& files, FileData::List& favorites);
	void addItem(FileData* file, bool toTheBeginning = false);*/

	const char * getItemIcon(FileData* item);
};
