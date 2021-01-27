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

    //! Run Kodi
    static void RunKodi(GuiMenu* thiz);
    //! Launch System menu
    static void System(GuiMenu* thiz);
    //! Launch Update menu
    static void Update(GuiMenu* thiz);
    //! Launch Games menu
    static void Games(GuiMenu* thiz);
    //! Launch Controllers menu
    static void Controllers(GuiMenu* thiz);
    //! Launch UI Settings menu
    static void UISettings(GuiMenu* thiz);
    //! Launch Sound menu
    static void Sound(GuiMenu* thiz);
    //! Launch Network menu
    static void Network(GuiMenu* thiz);
    //! Launch Scrapper menu
    static void Scrapper(GuiMenu* thiz);
    //! Launch Advanced menu
    static void Advanced(GuiMenu* thiz);
    //! Launch Bios menu
    static void Bios(GuiMenu* thiz);
    //! License
    static void License(GuiMenu* thiz);
    //! Quit
    static void Quit(GuiMenu* thiz);

    /*
     * IGuiMenuBase implementation
     */

    void SubMenuSelected(int id) override;
};
