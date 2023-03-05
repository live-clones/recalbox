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
                 , private IGuiMenuBase
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
      GameRegion,
      GameResolution,
      DemoIn240pOn31kHz,
      ScanlinesOn31kHz,
      Extended15kHzRange,
      UseV2,
      SuperRez,
      ZeroLag,
      ForceJack,
      Adjustment,
      HorizontalPalOffset,
      VerticalPalOffset,
      ForceHDMI,
    };

    //! Dac selection
    std::shared_ptr<OptionListComponent<CrtAdapterType>> mDac;
    CrtAdapterType mOriginalDac;
    //! Es resolution
    std::shared_ptr<OptionListComponent<std::string>> mEsResolution;
    std::string mOriginalEsResolution;
    //! Force jack audio
    bool mForceJack;
    bool mOriginalForceJack;
    //! Force HDMI video
    bool mForceHDMI;
    bool mOriginalForceHDMI;
    bool mOriginalFrontendIn240pOn31kHz;

    //! Get dacs
    static std::vector<ListEntry<CrtAdapterType>> GetDacEntries(bool onlyRgbDual);
    //! Get resolutions
    static std::vector<ListEntry<std::string>> GetEsResolutionEntries(bool only31kHz);
    //! Get super resolutions
    static std::vector<ListEntry<std::string>> GetSuperRezEntries();


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

    /*
     * IGuiMenuBase implementation
     */

    void SubMenuSelected(int id) override;
};
