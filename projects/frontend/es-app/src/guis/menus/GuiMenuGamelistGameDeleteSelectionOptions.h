
#pragma once

#include <guis/menus/GuiMenuBase.h>
#include <views/gamelist/IGameListView.h>

// Forward declaration
class SystemManager;
class SystemData;
template<class T> class OptionListComponent;

class GuiMenuGamelistGameDeleteSelectionOptions : public GuiMenuBase

{
  public:
    explicit GuiMenuGamelistGameDeleteSelectionOptions(WindowManager& window, IGameListView& view, FileData& game);

  private:
    enum class Components
    {
      Delete,
    };

    //! View reference
    IGameListView& mView;
    //! Game reference
    FileData& mGame;

    std::shared_ptr<OptionListComponent<Path>> mGameFiles;
    std::shared_ptr<OptionListComponent<Path>> mMedias;
    std::shared_ptr<OptionListComponent<Path>> mExtras;
    std::shared_ptr<OptionListComponent<Path>> mSaves;

    std::vector<ListEntry<Path>> GetGameFileEntries();
    std::vector<ListEntry<Path>> GetMediaEntries();
    std::vector<ListEntry<Path>> GetExtraEntries();
    std::vector<ListEntry<Path>> GetSaveEntries();

    void DeleteSelectedFiles();

    std::string ComputeMessage();
};



