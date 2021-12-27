#pragma once

#include "views/gamelist/ISimpleGameListView.h"
#include "components/ImageGridComponent.h"
#include "components/ImageComponent.h"
#include <views/ViewController.h>

class GridGameListView : public ISimpleGameListView
{
public:
	GridGameListView(WindowManager& window, SystemData& system);

	FileData* getCursor() override { return mGrid.getSelected(); }
	void setCursor(FileData*) override;

	bool ProcessInput(const InputCompactEvent& event) override;

	const char* getName() const override { return "grid"; }

	bool getHelpPrompts(Help& help) override;

protected:
	void populateList(const FolderData& folder) final;
	void launch(FileData* game) override { ViewController::Instance().Launch(game, GameLinkedData(), Vector3f()); }

	ImageGridComponent<FileData*> mGrid;
};
