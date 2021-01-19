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
{
  public:
    /*!
     * @brief Default constructor
     * @param window Global window
     */
    explicit GuiMenuSystemConfiguration(WindowManager& window, SystemData& system, SystemManager& systemManager);

  private:
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

    //! Set emulator & core
    void SetEmulatorCore(const std::string& emulatorCore);
    //! Set ratio
    void SetRatio(const std::string& ratio);
    //! Set smoothing on/off
    void SetSmooth(bool on);
    //! Set rewind on/off
    void SetRewind(bool on);
    //! Set auto save on/off
    void SetAutoSave(bool on);
    //! Set raw shaders path
    void SetShaders(const std::string& shaders);
    //! Set shader preset name
    void SetShaderSet(const std::string& shaderset);
};



