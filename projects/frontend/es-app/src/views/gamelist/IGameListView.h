#pragma once

#include "games/FileData.h"
#include "Renderer.h"
#include "systems/SystemData.h"

class Window;

class Component;

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
    SystemData& mSystem;
    const ThemeData* mTheme;

  public:
    IGameListView(Window&window, SystemData& system)
      : Gui(window),
        mSystem(system),
        mTheme(nullptr)
    {
      setSize(Renderer::getDisplayWidthAsFloat(), Renderer::getDisplayHeightAsFloat());
    }

    ~IGameListView() override = default;

    const SystemData& System() const { return mSystem; }

    // Called when a new file is added, a file is removed, a file's metadata changes, or when file sort changed
    virtual void onFileChanged(FileData* file, FileChangeType change) = 0;

    // Called whenever the theme changes.
    virtual void onThemeChanged(const ThemeData& theme) = 0;

    void setTheme(const ThemeData& theme);

    inline const ThemeData& getTheme() const { return *mTheme; }

    virtual FileData* getCursor() = 0;
    virtual void setCursor(FileData*) = 0;

    virtual const char* getName() const = 0;

    void ApplyHelpStyle() override;

    virtual void updateInfoPanel() = 0;

    virtual void populateList(const FolderData& folder) = 0;
    virtual void refreshList() = 0;

    virtual FileData::List getFileDataList() = 0;
    virtual std::string getAvailableLetters() = 0;
    virtual void jumpToLetter(unsigned int unicode) = 0;

    /*!
     * @brief Get available regions from the current game list
     * @return Region list (may be empty)
     */
    virtual Regions::List AvailableRegionsInGames() = 0;
};
