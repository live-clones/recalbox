#pragma once

#include "guis/menus/GuiMenuBase.h"

// Forward declarations
class WindowManager;
class SystemManager;

class GuiMenu : public GuiMenuBase
              , private IGuiMenuBase
{
  public:
    //! Constructor
    GuiMenu(WindowManager& window, SystemManager& systemManager);

  private:
    enum class Components
    {
      Kodi,
      System,
      Update,
      Games,
      Controllers,
      UISettings,
      Sound,
      Network,
      Scrapper,
      Advanced,
      Bios,
      License,
      Quit,
    };

    //! SystemManager instance
    SystemManager& mSystemManager;

    /*
     * IGuiMenuBase implementation
     */

    void SubMenuSelected(int id) override;
};
