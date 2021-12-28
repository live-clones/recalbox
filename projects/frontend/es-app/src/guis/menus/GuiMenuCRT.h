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
                 , private IOptionListComponent<CrtAdapterType>
                 , private IOptionListComponent<std::string>
                 , private ISwitchComponent
{
  public:
    /*!
     * @brief Default constructor
     * @param window Global window
     */
    explicit GuiMenuCRT(WindowManager& window);

    //! Default destructor
    ~GuiMenuCRT() override;

  private:
    enum class Components
    {
      CRTDac,
      EsResolution,
      Force50HZ,
      GameOptions,
    };

    //! Dac selection
    std::shared_ptr<OptionListComponent<CrtAdapterType>> mDac;
    CrtAdapterType mOriginalDac;
    //! Es resolution
    std::shared_ptr<OptionListComponent<std::string>> mEsResolution;
    std::string mOriginalEsResolution;
    //! Force 50 HZ
    std::shared_ptr<SwitchComponent> mForce50Hz;
    bool mOriginalForce50Hz;

    //! Get dacs
    static std::vector<ListEntry<CrtAdapterType>> GetDacEntries();
    //! Get resolutions
    static std::vector<ListEntry<std::string>> GetEsResolutionEntries();

    /*!
     * @brief Get Horizontal frequency display test
     * @return Text
     */
    static std::string GetHorizontalFrequency();

    /*!
     * @brief Get 50hz display test
     * @return Texg
     */
    static std::string Get50hz();

    /*
     * IOptionListComponent<Overclocking> implementation
     */

    void OptionListComponentChanged(int id, int index, const CrtAdapterType& value) override;

    /*
     * IOptionListComponent<Overclocking> implementation
     */

    void OptionListComponentChanged(int id, int index, const std::string & value) override;

    /*
     * ISwitchComponent implementation
     */

    void SwitchComponentChanged(int id, bool status) override;
};




