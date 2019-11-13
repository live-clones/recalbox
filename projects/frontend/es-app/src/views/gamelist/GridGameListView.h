#pragma once

#include "views/gamelist/ISimpleGameListView.h"
#include "components/ImageGridComponent.h"
#include "components/ImageComponent.h"
#include <views/ViewController.h>

class GridGameListView : public ISimpleGameListView
{
public:
	GridGameListView(Window* window, FolderData* root);

	FileData* getCursor() override { return mGrid.getSelected(); }
	void setCursor(FileData*) override;

	bool ProcessInput(const InputCompactEvent& event) override;

	const char* getName() const override { return "grid"; }

	std::vector<HelpPrompt> getHelpPrompts() override;

protected:
	void populateList(const FolderData* folder) override;
	void launch(FileData* game) override { ViewController::Instance().launch(game); }

	ImageGridComponent<FileData*> mGrid;
};
