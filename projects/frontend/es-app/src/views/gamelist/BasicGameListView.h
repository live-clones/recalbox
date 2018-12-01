#pragma once

#include "views/gamelist/ISimpleGameListView.h"
#include "components/TextListComponent.h"

class BasicGameListView : public ISimpleGameListView
{
public:
	BasicGameListView(Window* window, FolderData* root);

	// Called when a FileData* is added, has its metadata changed, or is removed
	virtual void onFileChanged(FileData* file, FileChangeType change);

	virtual void onThemeChanged(const std::shared_ptr<ThemeData>& theme);

	virtual FileData* getCursor() override;
	virtual int getCursorIndex() override;
	virtual void setCursor(FileData* file) override;
	virtual void setCursorIndex(int index) override;

	virtual const char* getName() const override { return "basic"; }

	virtual std::vector<HelpPrompt> getHelpPrompts() override;

	virtual void populateList(const FolderData* folder) override;
  virtual void refreshList() override;

	virtual inline void updateInfoPanel() override {}

	virtual FileData::List getFileDataList();

protected:
	virtual void launch(FileData* game) override;

	TextListComponent<FileData*> mList;
private:
  const FolderData *mPopulatedFolder;
  unsigned long listingOffset;

	/*void addFavorites(const FileData::List& files, const FileSorts::SortType& sortType);
	void getFavorites(const FileData::List& files, FileData::List& favorites);
	void addItem(FileData* file, bool toTheBeginning = false);*/

	const char * getItemIcon(FileData* item);
};
