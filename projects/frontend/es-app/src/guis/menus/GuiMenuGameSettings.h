//
// Created by bkg2k on 06/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <guis/menus/GuiMenuBase.h>

// Forward declaration
class SystemManager;
template<class T> class OptionListComponent;
class SwitchComponent;

class GuiMenuGameSettings : public GuiMenuBase
                          , private IOptionListComponent<std::string>
                          , private ISwitchComponent
                          , private IGuiMenuBase
{
  public:
    /*!
     * @brief Default constructor
     * @param window Global window
     */
    explicit GuiMenuGameSettings(WindowManager& window, SystemManager& systemManager);

    ~GuiMenuGameSettings() override;

  private:
    enum class Components
    {
      Ratio,
      RecalboxOverlays,
      Smooth,
      Rewind,
      AutoSave,
      QuitTwice,
      IntegerScale,
      Shaders,
      Softpatching,
      ShaderSet,
      RetroAchivements,
      Netplay,
    };

    //! System manager
    SystemManager& mSystemManager;

    //! Ratio
    std::shared_ptr<OptionListComponent<std::string>> mRatio;
    //! RecalboxOverlays
    std::shared_ptr<SwitchComponent> mRecalboxOverlays;
    //! Smooth
    std::shared_ptr<SwitchComponent> mSmooth;
    //! Rewind
    std::shared_ptr<SwitchComponent> mRewind;
    //! Shaders
    std::shared_ptr<OptionListComponent<std::string>> mSoftpatching;
    //! Auto-save
    std::shared_ptr<SwitchComponent> mAutoSave;
    //! Press twice to quit
    std::shared_ptr<SwitchComponent> mQuitTwice;
    //! Integer scale
    std::shared_ptr<SwitchComponent> mIntegerScale;
    //! Shaders
    std::shared_ptr<OptionListComponent<std::string>> mShaders;
    //! Shader Set
    std::shared_ptr<OptionListComponent<std::string>> mShaderSet;

    //! Get Ratio List
    static std::vector<ListEntry<std::string>> GetRatioEntries();
    //! Get Shaders List
    static std::vector<ListEntry<std::string>> GetShadersEntries();
    //! Get Softpatching List
    static std::vector<ListEntry<std::string>> GetSoftpatchingEntries();
    //! Get shader sets List
    static std::vector<ListEntry<std::string>> GetShaderPresetsEntries();

    /*
     * IGuiMenuBase implementation
     */

    void SubMenuSelected(int id) override;

    /*
     * IOptionListComponent<std::string> implementation
     */

    void OptionListComponentChanged(int id, int index, const std::string& value) override;

    /*
     * ISwitchComponent implementation
     */

    void SwitchComponentChanged(int id, bool status) override;
};



