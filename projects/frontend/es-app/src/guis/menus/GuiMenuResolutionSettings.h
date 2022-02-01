//
// Created by bkg2k on 31/01/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include "GuiMenuBase.h"

class SystemManager;

class GuiMenuResolutionSettings : public GuiMenuBase
  , private IOptionListComponent<std::string>
  , private IGuiMenuBase
{
  public:
    /*!
     * @brief Default constructor
     * @param window Global window
     */
    explicit GuiMenuResolutionSettings(WindowManager& window, SystemManager& systemManager);

  private:
    enum class Components
    {
      GlobalResolution,
      FrontendResolution,
      Emulators,
    };

    //! System manager reference
    SystemManager& mSystemManager;

    static std::vector<ListEntry<std::string>> GetGlobalResolutionEntries();
    static std::vector<ListEntry<std::string>> GetFrontEndResolutionEntries();

    /*
     * IOptionListComponent<Overclocking> implementation
     */

    void OptionListComponentChanged(int id, int index, const std::string& value) override;

    /*
     * IGuiMenuBase implementation
     */

    void SubMenuSelected(int id) override;
};



