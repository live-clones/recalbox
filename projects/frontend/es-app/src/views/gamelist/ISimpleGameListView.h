#pragma once

#include "views/gamelist/IGameListView.h"

#include "components/TextComponent.h"
#include "components/ImageComponent.h"
#include "themes/ThemeExtras.h"

class SystemManager;

class ISimpleGameListView : public IGameListView
{
public:
	ISimpleGameListView(Window& window, SystemManager& systemManager, SystemData& system);
	~ISimpleGameListView() override = default;

	// Called when a new file is added, a file is removed, a file's metadata changes, or when file sort changed
	void onFileChanged(FileData* file, FileChangeType change) override;

	// Called whenever the theme changes.
	void onThemeChanged(const ThemeData& theme) override;

	FileData* getCursor() override = 0;
	virtual int getCursorIndex() = 0;
	virtual int getCursorIndexMax() = 0;
	void setCursor(FileData*) override = 0;
	virtual void setCursorIndex(int) = 0;

	bool ProcessInput(const InputCompactEvent& event) override;

	//inline void DoUpdateGameInformation() override {}

	//inline void populateList(const FolderData& folder) override { (void)folder; }
	//inline void refreshList() override {};

	bool getHelpPrompts(Help& help) override;
	std::vector<unsigned int> getAvailableLetters() override;
	void jumpToLetter(unsigned int unicode) override;

	void jumpToNextLetter(bool forward);

protected:
	virtual void launch(FileData* game) = 0;

	virtual FileData* getEmptyListItem() = 0;

  //! SystemManager instance
	SystemManager& mSystemManager;

	TextComponent mHeaderText;
	ImageComponent mHeaderImage;
	ImageComponent mBackground;

	ThemeExtras mThemeExtras;

	std::stack<FolderData*> mCursorStack;
	bool mFavoritesOnly;

private:
  bool IsFavoriteSystem() { return mSystem.IsFavorite(); }
};
