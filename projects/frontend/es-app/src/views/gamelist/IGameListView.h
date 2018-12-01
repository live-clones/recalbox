#pragma once

#include "FileData.h"
#include "Renderer.h"
#include "SystemData.h"

class Window;

class GuiComponent;

class FileData;

class ThemeData;

enum class FileChangeType
{
    Added,
    Run,
    MetadataChanged,
    Removed,
    Sorted,
    DisplayUpdated,
};

// This is an interface that defines the minimum for a GameListView.
class IGameListView : public GuiComponent
{
  protected:
    FolderData* mRoot;
    SystemData* mSystem;
    std::shared_ptr<ThemeData> mTheme;

  public:
    IGameListView(Window* window, FolderData* root)
      : GuiComponent(window),
        mRoot(root),
        mSystem(root->getSystem())
    {
      setSize((float) Renderer::getScreenWidth(), (float) Renderer::getScreenHeight());
    }

    virtual ~IGameListView() {}

    // Called when a new file is added, a file is removed, a file's metadata changes, or when file sort changed
    virtual void onFileChanged(FileData* file, FileChangeType change) = 0;

    // Called whenever the theme changes.
    virtual void onThemeChanged(const std::shared_ptr<ThemeData>& theme) = 0;

    void setTheme(const std::shared_ptr<ThemeData>& theme);

    inline const std::shared_ptr<ThemeData>& getTheme() const { return mTheme; }

    inline FolderData* getRoot() const { return mRoot; }

    virtual FileData* getCursor() = 0;
    virtual void setCursor(FileData*) = 0;

    virtual bool input(InputConfig* config, Input input) override;

    virtual const char* getName() const = 0;

    virtual HelpStyle getHelpStyle() override;

    virtual void updateInfoPanel() = 0;

    virtual void populateList(const FolderData* folder) = 0;
    virtual void refreshList() = 0;

    virtual FileData::List getFileDataList() = 0;
};
