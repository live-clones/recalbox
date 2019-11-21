#pragma once

#include "games/FileData.h"
#include "Renderer.h"
#include "systems/SystemData.h"

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
class IGameListView : public Gui
{
  protected:
    FolderData* mRoot; // TODO: check if a & is more appropriate
    SystemData* mSystem;
    const ThemeData* mTheme;

  public:
    IGameListView(Window&window, FolderData* root)
      : Gui(window),
        mRoot(root),
        mSystem(root->getSystem()),
        mTheme(nullptr)
    {
      setSize(Renderer::getDisplayWidthAsFloat(), Renderer::getDisplayHeightAsFloat());
    }

    ~IGameListView() override = default;

    // Called when a new file is added, a file is removed, a file's metadata changes, or when file sort changed
    virtual void onFileChanged(FileData* file, FileChangeType change) = 0;

    // Called whenever the theme changes.
    virtual void onThemeChanged(const ThemeData& theme) = 0;

    void setTheme(const ThemeData& theme);

    inline const ThemeData& getTheme() const { return *mTheme; }

    inline FolderData* getRoot() const { return mRoot; }

    virtual FileData* getCursor() = 0;
    virtual void setCursor(FileData*) = 0;

    bool ProcessInput(const InputCompactEvent& event) override;

    virtual const char* getName() const = 0;

    void ApplyHelpStyle() override;

    virtual void updateInfoPanel() = 0;

    virtual void populateList(const FolderData* folder) = 0;
    virtual void refreshList() = 0;

    virtual FileData::List getFileDataList() = 0;
    virtual std::vector<std::string> getAvailableLetters() = 0;
    virtual void jumpToLetter(char letter) = 0;
};
