#pragma once

#include <guis/menus/GuiMenuBase.h>
#include <scraping/scrapers/IScraperEngineFreezer.h>

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
      RecalboxRGBDual,
      Games,
      Controllers,
      UISettings,
      Tate,
      Sound,
      Network,
      Scraper,
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
