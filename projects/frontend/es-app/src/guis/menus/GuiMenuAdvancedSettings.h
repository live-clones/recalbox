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

class GuiMenuAdvancedSettings : public GuiMenuBase
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
    static constexpr const char* sOverclockBaseFolder = "/recalbox/system/configs/overclocking";
    static constexpr const char* sOverclockFile = "/boot/recalbox-oc-config.txt";

    //! O/C file & description
    struct Overclocking
    {
      std::string File;
      std::string Description;
      bool Hazardous;

      bool operator == (const Overclocking& r) const { return (File == r.File) && (Description == r.Description) && (Hazardous == r.Hazardous); }
    };

    //! Overclocking list
    typedef std::vector<Overclocking> OverclockList;

    //! System Manager
    SystemManager& mSystemManager;

    //! Default overclock value
    Overclocking mDefaultOverclock;
    //! Original overclock value
    std::string mOriginalOverclock;
    //! Last overclock hazardous?
    static bool sLastHazardous;

    //! Overclock
    std::shared_ptr<OptionListComponent<Overclocking>> mOverclock;
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

    //! Set overclock
    static void SetOverclock(const Overclocking& oc);
    //! Reset overclock
    void ResetOverclock();
    //! Set Adult flag
    static void SetAdult(bool on);
    //! Set Overscan
    static void SetOverscan(bool on);
    //! Set Show FPS
    static void SetShowFPS(bool on);
    //! Set Webmanager
    static void SetWebmanager(bool on);
};



