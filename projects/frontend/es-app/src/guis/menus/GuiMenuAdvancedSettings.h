//
// Created by bkg2k on 11/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <guis/menus/GuiMenuBase.h>

// Forward declaration
class SystemManager;
class SystemData;
template<class T> class OptionListComponent;
class SwitchComponent;

//! O/C file & description
struct Overclocking
{
  std::string File;
  std::string Description;
  bool Hazardous;
  int Frequency;

  bool operator == (const Overclocking& r) const { return (File == r.File) && (Description == r.Description) && (Hazardous == r.Hazardous) && (Frequency == r.Frequency); }
};

class GuiMenuAdvancedSettings : public GuiMenuBase
                              , private IOptionListComponent<Overclocking>
                              , private IOptionListComponent<SystemSorting>
                              , private ISwitchComponent
                              , private IGuiMenuBase
{
  public:
    /*!
     * @brief Default constructor
     * @param window Global window
     */
    explicit GuiMenuAdvancedSettings(WindowManager& window, SystemManager& systemManager);

    //! Destructor
    ~GuiMenuAdvancedSettings() override;

  private:
    enum class Components
    {
      DebugLogs,
      OverclockList,
      SystemSort,
      BootSubMenu,
      VirtualSubMenu,
      AdultGames,
      ResolutionSubMenu,
      AdvancedSubMenu,
      KodiSubMenu,
      SecuritySubMenu,
      Overscan,
      ShowFPS,
      CrtSubMenu,
      Manager,
      FactoryReset,
    };

    static constexpr const char* sOverclockBaseFolder = "/recalbox/system/configs/overclocking";
    static constexpr const char* sOverclockFile = "/boot/recalbox-oc-config.txt";

    //! Overclocking list
    typedef std::vector<Overclocking> OverclockList;

    //! System Manager
    SystemManager& mSystemManager;

    //! Default overclock value
    Overclocking mDefaultOverclock;
    //! Original overclock value
    std::string mOriginalOverclock;
    //! Original Sorting type
    SystemSorting mOriginalSort;
    //! Last overclock hazardous?
    bool mLastHazardous;
    //! Is there at least a valid O/C?
    bool mValidOverclock;

    //! Overclock
    std::shared_ptr<OptionListComponent<Overclocking>> mOverclock;
    //! System Sort
    std::shared_ptr<OptionListComponent<SystemSorting>> mSort;
    //! Adult
    std::shared_ptr<SwitchComponent> mAdult;
    //! Overscan
    std::shared_ptr<SwitchComponent> mOverscan;
    //! Show FPS
    std::shared_ptr<SwitchComponent> mShowFPS;
    //! Enable Webmanager
    std::shared_ptr<SwitchComponent> mWebManager;

    //! Get O/C list for the current board
    static OverclockList AvailableOverclocks();

    //! Get O/C List
    std::vector<ListEntry<Overclocking>> GetOverclockEntries();

    //! Reset overclock
    void ResetOverclock();

    //! Reset Factory requested Level 1
    void ResetFactory();

    //! Reset Factory requested Level 2
    static void ResetFactoryReally(WindowManager* This);

    //! Do Reset Factory
    static void DoResetFactory();

    //! Get Sorting List
    std::vector<ListEntry<SystemSorting>> GetSortingEntries();

    /*
     * IOptionListComponent<Overclocking> implementation
     */

    void OptionListComponentChanged(int id, int index, const Overclocking& value) override;

    /*
     * ISwitchComponent implementation
     */

    void SwitchComponentChanged(int id, bool status) override;

    /*
     * IGuiMenuBase implementation
     */

    void SubMenuSelected(int id) override;

    /*
     * IOptionListComponent<SystemSorting> implementation
     */

    void OptionListComponentChanged(int id, int index, const SystemSorting& value) override;
};



