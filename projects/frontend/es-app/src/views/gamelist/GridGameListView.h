#pragma once

#include "views/gamelist/ISimpleGameListView.h"
#include "components/ImageGridComponent.h"
#include "components/ImageComponent.h"
#include <stack>

class GridGameListView : public ISimpleGameListView
{
public:
	GridGameListView(Window* window, FolderData* root);

	//virtual void onThemeChanged(const std::shared_ptr<ThemeData>& theme) override;

	FileData* getCursor() override;
	void setCursor(FileData*) override;

	bool input(InputConfig* config, Input input) override;

	const char* getName() const override { return "grid"; }

	std::vector<HelpPrompt> getHelpPrompts() override;

protected:
	void populateList(const FolderData* folder) override;
	void launch(FileData* game) override;

	ImageGridComponent<FileData*> mGrid;
};
