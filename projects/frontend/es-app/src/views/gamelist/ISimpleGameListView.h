#pragma once

#include "views/gamelist/IGameListView.h"

#include "components/TextComponent.h"
#include "components/ImageComponent.h"
#include "themes/ThemeExtras.h"

class ISimpleGameListView : public IGameListView
{
public:
	ISimpleGameListView(Window* window, FolderData* root);
	~ISimpleGameListView() override = default;

	// Called when a new file is added, a file is removed, a file's metadata changes, or when file sort changed
	void onFileChanged(FileData* file, FileChangeType change) override;

	// Called whenever the theme changes.
	void onThemeChanged(const std::shared_ptr<ThemeData>& theme) override;

	FileData* getCursor() override = 0;
	virtual int getCursorIndex() = 0;
	virtual int getCursorIndexMax() = 0;
	void setCursor(FileData*) override = 0;
	virtual void setCursorIndex(int) = 0;

	bool input(InputConfig* config, Input input) override;

	inline void updateInfoPanel() override {}

	inline void populateList(const FolderData* folder) override { (void)folder; }
	inline void refreshList() override {};

	std::vector<HelpPrompt> getHelpPrompts() override;
	std::vector<std::string> getAvailableLetters() override;
	void jumpToLetter(char letter) override;

	void jumpToNextLetter(int increment);

protected:
	virtual void launch(FileData* game) = 0;

	virtual FileData* getEmptyListItem() = 0;

	TextComponent mHeaderText;
	ImageComponent mHeaderImage;
	ImageComponent mBackground;

	ThemeExtras mThemeExtras;

	std::stack<FolderData*> mCursorStack;
	bool mFavoritesOnly;

private:
  bool mIsFavoriteSystem;
  int mFavoritesCount;
};
