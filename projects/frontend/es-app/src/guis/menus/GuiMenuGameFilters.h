//
// Created by gugue_u on 29/03/2022.
//
//

#include "guis/menus/GuiMenuBase.h"
#include "components/MenuComponent.h"
#include "guis/GuiMetaDataEd.h"

class IGameListView;

class GuiMenuGameFilters : public GuiMenuBase
                         , private ISwitchComponent
{
  public:
    /*!
     * @brief Constructor
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

    /*
     * ISwitchComponent implementation
     */

    void SwitchComponentChanged(int id, bool status) override;

    /*!
    * @brief Manage all systems
    */
    void ManageSystems();
};