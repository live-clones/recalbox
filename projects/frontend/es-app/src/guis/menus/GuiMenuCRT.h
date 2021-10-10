//
// Created by digitalLumberjack on 20/0/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <guis/menus/GuiMenuBase.h>

// Forward declaration
class SystemManager;
class SwitchComponent;
template<class T> class OptionListComponent;

class GuiMenuCRT : public GuiMenuBase
                          , private IOptionListComponent<std::string>
                          , private ISwitchComponent
{
  public:
    /*!
     * @brief Default constructor
     * @param window Global window
     */
    explicit GuiMenuCRT(WindowManager& window);
    ~GuiMenuCRT() override;
  private:
    enum class Components
    {
      CRTDac,
      EsResolution,
      Force50HZ,
    };

    //! Dac selection
    bool mReboot;
    //! Dac selection
    std::shared_ptr<OptionListComponent<std::string>> mDac;
    std::string mOriginalDac;
    //! Es resolution
    std::shared_ptr<OptionListComponent<std::string>> mEsResolution;
    std::string mOriginalEsResolution;
    //! Force 50 HZ
    std::shared_ptr<SwitchComponent> mForce50Hz;
    bool mOriginalForce50Hz;

    //! Get dacs
    std::vector<ListEntry<std::string>> GetDacEntries();
    //! Get resolutions
    std::vector<ListEntry<std::string>> GetEsResolutionEntries();

    /*
     * IOptionListComponent<Overclocking> implementation
     */

    void OptionListComponentChanged(int id, int index, const std::string& value) override;

    /*
     * ISwitchComponent implementation
     */

    void SwitchComponentChanged(int id, bool status) override;
};




