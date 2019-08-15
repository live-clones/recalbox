#pragma once

#include "views/gamelist/ISimpleGameListView.h"
#include "components/ImageGridComponent.h"
#include "components/ImageComponent.h"
#include <views/ViewController.h>

class GridGameListView : public ISimpleGameListView
{
public:
	GridGameListView(Window* window, FolderData* root);

	//virtual void onThemeChanged(const std::shared_ptr<ThemeData>& theme) override;

	FileData* getCursor() override { return mGrid.getSelected(); }
	void setCursor(FileData*) override;

	bool input(InputConfig* config, Input input) override;

	const char* getName() const override { return "grid"; }

	std::vector<HelpPrompt> getHelpPrompts() override;

protected:
	void populateList(const FolderData* folder) override;
	void launch(FileData* game) override { ViewController::get()->launch(game); }

	ImageGridComponent<FileData*> mGrid;
};
