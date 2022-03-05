#include <systems/SystemManager.h>
#include "guis/menus/GuiMenuBase.h"
#include "components/MenuComponent.h"
#include "components/OptionListComponent.h"
#include "games/FileSorts.h"
#include "guis/GuiMetaDataEd.h"

class IGameListView;

class GuiMenuGamelistOptions : public GuiMenuBase
                         , private GuiMetaDataEd::IMetaDataAction
                         , private IOptionListComponent<unsigned int>
                         , private IOptionListComponent<Regions::GameRegions>
                         , private IOptionListComponent<FileSorts::Sorts>
                         , private IGuiMenuBase
                         , private ISwitchComponent
{
  public:
    /*!
     * @brief Constructor
     * @param window Main window
     * @param system Target system
     * @param systemManager System manager
     */
    GuiMenuGamelistOptions(WindowManager&window, SystemData& system, SystemManager& systemManager);

    //! Destructor
    ~GuiMenuGamelistOptions() override;

  private:
    enum class Components
    {
      JumpToLetter,
      Sorts,
      Adult,
      Regions,
      FavoritesOnly,
      ShowHidden,
      LatestRevOnly,
      FlatFolders,
      MetaData,
      UpdateGamelist,
      Delete,
      DeleteScreeshot,
      DisplayByFileName,
      MainMenu,
      Quit,
    };

    //! System reference
    SystemData& mSystem;
    //! System manager reference
    SystemManager& mSystemManager;
    //! Gamelist UI reference
    IGameListView& mGamelist;

    std::shared_ptr<OptionListComponent<unsigned int>> mJumpToLetterList;
    std::shared_ptr<OptionListComponent<FileSorts::Sorts>> mListSort;
    std::shared_ptr<OptionListComponent<Regions::GameRegions>> mListRegion;
    std::shared_ptr<SwitchComponent> mAdult;
    std::shared_ptr<SwitchComponent> mFlatFolders;
    std::shared_ptr<SwitchComponent> mFavoritesOnly;
    std::shared_ptr<SwitchComponent> mShowHidden;
    std::shared_ptr<TextComponent> mGame;

    //! Refresh first menu entry
    void RefreshGameMenuContext();

    //! Get available letter List
    std::vector<ListEntry<unsigned int>> GetLetterEntries();
    //! Get available sort List
    std::vector<ListEntry<FileSorts::Sorts>> GetSortEntries();
    //! Get available region List
    std::vector<ListEntry<Regions::GameRegions>> GetRegionEntries();

    /*
     * GuiMetaDataEd::IMetaDataAction implementation
     */

    void Delete(IGameListView* gamelistview, FileData& game) override;
    void Modified(IGameListView* gamelistview, FileData& game) override;

    /*
     * IOptionListComponent<unsigned int> implementation
     */

    void OptionListComponentChanged(int id, int index, const unsigned int& value) override;

    /*
     * IOptionListComponent<FileSorts::Sorts> implementation
     */
    void OptionListComponentChanged(int id, int index, const Regions::GameRegions& value) override;

    /*
     * IOptionListComponent<FileSorts::Sorts> implementation
     */
    void OptionListComponentChanged(int id, int index, const FileSorts::Sorts& value) override;

    /*
     * IGuiMenuBase implementation
     */

    void SubMenuSelected(int id) override;

    /*
     * ISwitchComponent implementation
     */

    void SwitchComponentChanged(int id, bool status) override;
};
