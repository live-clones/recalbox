//
// Created by bkg2k on 04/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <guis/menus/GuiMenuBase.h>
#include "ResolutionAdapter.h"

// Forward declaration
class SystemManager;
class SwitchComponent;
template<class T> class OptionListComponent;

class GuiMenuKodiSettings : public GuiMenuBase
                          , private IOptionListComponent<std::string>
                          , private ISwitchComponent
{
  public:
    /*!
     * @brief Default constructor
     * @param window Global window
     */
    explicit GuiMenuKodiSettings(WindowManager& window);

  private:
    enum class Components
    {
      Enabled,
      RunAtStartup,
      ButtonX,
      KodiResolution
    };

    //! Enable kodi
    std::shared_ptr<SwitchComponent> mKodiEnabled;
    //! Run kodi at startup
    std::shared_ptr<SwitchComponent> mKodiAtStartup;
    //! Run kodi with X key
    std::shared_ptr<SwitchComponent> mKodiX;
    //! Resolution Adapter
    ResolutionAdapter mResolutionAdapter;

    /*
     * ISwitchComponent implementation
     */
    void SwitchComponentChanged(int id, bool status) override;
    void OptionListComponentChanged(int id, int index, const std::string& value) override;
    std::vector<ListEntry<std::string>> GetKodiResolutionsEntries();

};



