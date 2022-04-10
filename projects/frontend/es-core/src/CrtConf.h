//
// Created by bkg2k on 11/01/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <RecalboxConf.h>
#include <hardware/crt/CrtAdapterType.h>

class CrtConf: public IniFile, public StaticLifeCycleControler<CrtConf>
{
  public:
    //! Constructor
    CrtConf();

    /*!
     * @brief Called when file has been saved
     */
    void OnSave() override;

    DefineGetterSetterEnumGeneric(CrtConf, SystemCRT, CrtAdapterType, sSystemCRT, CrtAdapter)
    DefineGetterSetterGeneric(CrtConf, SystemCRTResolution, std::string, String, sSystemCRTResolution, "240")
    DefineGetterSetterGeneric(CrtConf, SystemCRTGameRegionSelect, bool, Bool, sSystemCRTGameRegionSelect, false)
    DefineGetterSetterGeneric(CrtConf, SystemCRTGameResolutionSelect, bool, Bool, sSystemCRTGameResolutionSelect, true)
    DefineGetterSetterGeneric(CrtConf, SystemCRTRunDemoIn240pOn31kHz, bool, Bool, sSystemCRTRunDemoIn240pOn31kHz, false)
    DefineGetterSetterGeneric(CrtConf, SystemCRTScanlines31kHz, bool, Bool, sSystemCRTScanlines31kHz, false)
    DefineGetterSetterGeneric(CrtConf, SystemCRTHorizontalOffset, int, Int, sSystemCRTHorizontalOffset, -5)
    DefineGetterSetterGeneric(CrtConf, SystemCRTVerticalOffset, int, Int, sSystemCRTVerticalOffset, 0)
    DefineGetterSetterGeneric(CrtConf, SystemCRTHorizontalPALOffset, int, Int, sSystemCRTHorizontalPALOffset, 0)
    DefineGetterSetterGeneric(CrtConf, SystemCRTVerticalPALOffset, int, Int, sSystemCRTVerticalPALOffset, 0)
    DefineGetterSetterGeneric(CrtConf, SystemCRTViewportWidth, int, Int, sSystemCRTViewportWidth, 0)
    DefineGetterSetterGeneric(CrtConf, SystemCRTForceJack, bool, Bool, sSystemCRTForceJack, false)


private:
    static constexpr const char* sSystemCRT                       = "adapter.type";
    static constexpr const char* sSystemCRTResolution             = "options.es.resolution";
    static constexpr const char* sSystemCRTGameRegionSelect       = "options.regionselect";
    static constexpr const char* sSystemCRTGameResolutionSelect   = "options.resolutionselect";
    static constexpr const char* sSystemCRTRunDemoIn240pOn31kHz   = "options.31khz.demo240pOn31khz";
    static constexpr const char* sSystemCRTScanlines31kHz         = "options.31khz.scanlines";
    static constexpr const char* sSystemCRTHorizontalOffset       = "mode.offset.horizontal";
    static constexpr const char* sSystemCRTVerticalOffset         = "mode.offset.vertical";
    static constexpr const char* sSystemCRTHorizontalPALOffset    = "mode.offset.horizontal.pal";
    static constexpr const char* sSystemCRTVerticalPALOffset      = "mode.offset.vertical.pal";
    static constexpr const char* sSystemCRTViewportWidth          = "viewport.width";
    static constexpr const char* sSystemCRTForceJack              = "audio.forcejack";


    static CrtAdapterType CrtAdapterFromString(const std::string& adapter);
    static const std::string& CrtAdapterFromEnum(CrtAdapterType adapter);
};



