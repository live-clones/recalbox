//
// Created by gugue_u on 29/03/2022.
//
//

#include <systems/SystemManager.h>
#include "guis/menus/GuiMenuBase.h"
#include "components/MenuComponent.h"
#include "games/FileSorts.h"
#include "guis/GuiMetaDataEd.h"

class IGameListView;

class GuiMenuGameFilters : public GuiMenuBase
                         , private ISwitchComponent
{
  public:
    /*!
     * @brief Constructor
     * @param window Main window
     * @param system Target system
     * @param systemManager System manager
     */
    GuiMenuGameFilters(WindowManager&window, SystemData& system);

    ~GuiMenuGameFilters() override;

  private:
    enum class Components
    {
      Adult,
      FavoritesOnly,
      ShowHidden,
      HidePreinstalled,
      NoGames,
      ShowOnlyLatestVersion,
    };

    //! System reference
    SystemData& mSystem;
    //! Gamelist UI reference
    IGameListView& mGamelist;

    std::shared_ptr<TextComponent> mGame;
    std::shared_ptr<SwitchComponent> mFavoritesOnly;

    //! Refresh first menu entry
    void RefreshGameMenuContext();

    //! Get available letter List
    std::vector<ListEntry<unsigned int>> GetLetterEntries();
    //! Get available sort List
    std::vector<ListEntry<FileSorts::Sorts>> GetSortEntries();
    //! Get available region List
    std::vector<ListEntry<Regions::GameRegions>> GetRegionEntries();

    /*
     * ISwitchComponent implementation
     */

    void SwitchComponentChanged(int id, bool status) override;

    void ManageSystems();
};
