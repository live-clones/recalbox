
#pragma once

#include <guis/menus/GuiMenuBase.h>
#include <views/gamelist/IGameListView.h>

// Forward declaration
class SystemManager;
class SystemData;
template<class T> class OptionListComponent;

class GuiMenuGamelistGameDeleteOptions : public GuiMenuBase
  , private IGuiMenuBase


{
  public:
    explicit GuiMenuGamelistGameDeleteOptions(WindowManager& window, IGameListView& view, FileData& game);

  private:
    enum class Components
    {
      Delete,
      Advanced
    };

    //! View reference
    IGameListView& mView;
    //! Game reference
    FileData& mGame;

    HashSet<std::string> mGameFiles;
    HashSet<std::string> mExtraFiles;
    HashSet<std::string> mMediaFiles;
    HashSet<std::string> mSaveFiles;

//    std::shared_ptr<OptionListComponent<Path>> mGameFiles;
//    std::shared_ptr<OptionListComponent<Path>> mMedias;
//    std::shared_ptr<OptionListComponent<Path>> mExtras;
//    std::shared_ptr<OptionListComponent<Path>> mSaves;

    void DeleteAllFiles();

    bool ProcessInput(const InputCompactEvent& event) override;

    void SubMenuSelected(int id) override;

    std::string ComputeMessage();
};



