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
    GuiMenuGameFilters(WindowManager&window);

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

    std::shared_ptr<TextComponent> mGame;
    std::shared_ptr<SwitchComponent> mFavoritesOnly;

    /*
     * ISwitchComponent implementation
     */

    void SwitchComponentChanged(int id, bool status) override;

    void ManageSystems();
};
