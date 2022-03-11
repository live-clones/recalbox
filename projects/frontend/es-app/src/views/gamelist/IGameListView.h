#pragma once

#include "games/FileData.h"
#include "Renderer.h"
#include "systems/SystemData.h"

class WindowManager;
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
    enum class Change
    {
      Resort, //!< New sorting required
      Update, //!< Update lists
    };

    IGameListView(WindowManager&window, SystemData& system)
      : Gui(window),
        mSystem(system),
        mTheme(nullptr)
    {
      setSize(Renderer::Instance().DisplayWidthAsFloat(), Renderer::Instance().DisplayHeightAsFloat());
    }

    ~IGameListView() override = default;

    const SystemData& System() const { return mSystem; }

    // Called when a new file is added, a file is removed, a file's metadata changes, or when file sort changed
    virtual void onFileChanged(FileData* file, FileChangeType change) = 0;

    /*!
     * @brief Called when a major change occurs on the system
     * @param change Change type
     */
    virtual void onChanged(Change change) = 0;

    // Called whenever the theme changes.
    virtual void onThemeChanged(const ThemeData& theme) = 0;

    void setTheme(const ThemeData& theme);

    inline const ThemeData& getTheme() const { return *mTheme; }

    virtual int Count() const = 0;
    virtual bool IsEmpty() const  = 0;
    virtual FileData* getCursor() = 0;
    virtual void setCursorStack(FileData*) = 0;
    virtual void setCursor(FileData*) = 0;

    virtual int getCursorIndex() = 0;
    virtual int getCursorIndexMax() = 0;
    virtual void setCursorIndex(int) = 0;

    virtual void removeEntry(FileData* fileData) = 0;

    virtual const char* getName() const = 0;

    void ApplyHelpStyle() override;

    void updateInfoPanel();

    virtual void DoUpdateGameInformation() = 0;

    virtual void populateList(const FolderData& folder) = 0;
    virtual void refreshList() = 0;

    virtual FileData::List getFileDataList() = 0;
    virtual std::vector<unsigned int> getAvailableLetters() = 0;
    virtual void jumpToLetter(unsigned int unicode) = 0;

    /*!
     * @brief Get available regions from the current game list
     * @return Region list (may be empty)
     */
    virtual Regions::List AvailableRegionsInGames() = 0;
};
