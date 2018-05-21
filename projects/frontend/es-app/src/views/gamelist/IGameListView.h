#pragma once

#include "FileData.h"
#include "Renderer.h"
#include "SystemData.h"

class Window;
class GuiComponent;
class FileData;
class ThemeData;

// This is an interface that defines the minimum for a GameListView.
class IGameListView : public GuiComponent
{
public:
	IGameListView(Window* window, FileData* root) : GuiComponent(window), mRoot(root), mSystem(root->getSystem())
		{ setSize((float)Renderer::getScreenWidth(), (float)Renderer::getScreenHeight()); }

	virtual ~IGameListView() {}

	// Called when a new file is added, a file is removed, a file's metadata changes, or when file sort changed
	virtual void onFileChanged(FileData* file, FileChangeType change) = 0;
	
	// Called whenever the theme changes.
	virtual void onThemeChanged(const std::shared_ptr<ThemeData>& theme) = 0;

	void setTheme(const std::shared_ptr<ThemeData>& theme);
	inline const std::shared_ptr<ThemeData>& getTheme() const { return mTheme; }
	inline FileData* getRoot() const { return mRoot; }

	virtual FileData* getCursor() = 0;
	virtual void setCursor(FileData*) = 0;

	virtual bool input(InputConfig* config, Input input) override;

	virtual const char* getName() const = 0;

	virtual HelpStyle getHelpStyle() override;

	virtual void updateInfoPanel() = 0;

	virtual void populateList(const FileData* folder) = 0;
	virtual void refreshList() = 0;

	virtual std::vector<FileData*> getFileDataList() = 0;

protected:
	FileData* mRoot;
	SystemData* mSystem;
	std::shared_ptr<ThemeData> mTheme;
};
