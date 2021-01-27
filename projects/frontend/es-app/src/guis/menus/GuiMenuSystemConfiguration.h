//
// Created by bkg2k on 04/01/2021.
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

class GuiMenuSystemConfiguration : public GuiMenuBase
                                 , private IOptionListComponent<std::string>
                                 , private ISwitchComponent
{
  public:
    /*!
     * @brief Default constructor
     * @param window Global window
     */
    explicit GuiMenuSystemConfiguration(WindowManager& window, SystemData& system, SystemManager& systemManager);

  private:
    enum class Components
    {
      Emulator,
      Ratio,
      Smooth,
      Rewind,
      AutoSave,
      Shaders,
      ShaderSet,
    };

    //! System manager
    SystemManager& mSystemManager;
    //! Target system
    SystemData& mSystem;
    //! Default emulator
    std::string mDefaultEmulator;
    //! Default core
    std::string mDefaultCore;

    //! Emulator
    std::shared_ptr<OptionListComponent<std::string>> mEmulator;
    //! Ratio
    std::shared_ptr<OptionListComponent<std::string>> mRatio;
    //! Smooth
    std::shared_ptr<SwitchComponent> mSmooth;
    //! Rewind
    std::shared_ptr<SwitchComponent> mRewind;
    //! Auto-save
    std::shared_ptr<SwitchComponent> mAutoSave;
    //! Shaders
    std::shared_ptr<OptionListComponent<std::string>> mShaders;
    //! Shader Set
    std::shared_ptr<OptionListComponent<std::string>> mShaderSet;

    //! Get Emulator List
    std::vector<ListEntry<std::string>> GetEmulatorEntries();
    //! Get Ratio List
    std::vector<ListEntry<std::string>> GetRatioEntries();
    //! Get Shaders List
    std::vector<ListEntry<std::string>> GetShadersEntries();
    //! Get ShaderSet List
    std::vector<ListEntry<std::string>> GetShaderSetEntries();

    /*
     * IOptionListComponent<std::string> implementation
     */

    void OptionListComponentChanged(int id, int index, const std::string& value) override;

    /*
     * ISwitchComponent implementation
     */

    void SwitchComponentChanged(int id, bool status) override;
};



