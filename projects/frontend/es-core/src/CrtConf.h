//
// Created by bkg2k on 11/01/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <RecalboxConf.h>
#include <hardware/crt/CrtAdapterType.h>
#include <views/crt/CrtResolutions.h>

class CrtConf: public IniFile, public StaticLifeCycleControler<CrtConf>
{
  public:
    //! Constructor
    CrtConf();

    #define DefineCrtModeOffsetDeclaration(name, type, type2, key) \
      type GetCrtModeOffset##name(CrtResolution reso) const; \
      CrtConf& SetCrtModeOffset##name(CrtResolution reso, const type& value);

    #define DefineCrtModeOffsetImplementation(name, type, type2, key, defaultValue) \
      type CrtConf::GetCrtModeOffset##name(CrtResolution reso) const { return As##type2(std::string(sModeOffsetPrefix).append(1, '.').append(CrtResolutionFromEnum(reso)).append(1, '.').append(key), defaultValue); } \
      CrtConf& CrtConf::SetCrtModeOffset##name(CrtResolution reso, const type& value) { Set##type2(std::string(sModeOffsetPrefix).append(1, '.').append(CrtResolutionFromEnum(reso)).append(1, '.').append(key), value); return *this; }

    #define DefineCrtViewportDeclaration(name, type, type2, key) \
      type GetCrtViewport##name(CrtResolution reso) const; \
      CrtConf& SetCrtViewport##name(CrtResolution reso, const type& value);

    #define DefineCrtViewportImplementation(name, type, type2, key, defaultValue) \
      type CrtConf::GetCrtViewport##name(CrtResolution reso) const { return As##type2(std::string(sViewportPrefix).append(1, '.').append(CrtResolutionFromEnum(reso)).append(1, '.').append(key), defaultValue); } \
      CrtConf& CrtConf::SetCrtViewport##name(CrtResolution reso, const type& value) { Set##type2(std::string(sViewportPrefix).append(1, '.').append(CrtResolutionFromEnum(reso)).append(1, '.').append(key), value); return *this; }

    /*!
     * @brief Called when file has been saved
     */
    void OnSave() override;

    DefineGetterSetterEnumGeneric(CrtConf, SystemCRT, CrtAdapterType, sSystemCRT, CrtAdapter)
    DefineGetterSetterGeneric(CrtConf, SystemCRTResolution, std::string, String, sSystemCRTResolution, "240")
    DefineGetterSetterGeneric(CrtConf, SystemCRT31kHzResolution, std::string, String, sSystemCRT31kHzResolution, "480")
    DefineGetterSetterGeneric(CrtConf, SystemCRTGameRegionSelect, bool, Bool, sSystemCRTGameRegionSelect, false)
    DefineGetterSetterGeneric(CrtConf, SystemCRTGameResolutionSelect, bool, Bool, sSystemCRTGameResolutionSelect, true)
    DefineGetterSetterGeneric(CrtConf, SystemCRTRunDemoIn240pOn31kHz, bool, Bool, sSystemCRTRunDemoIn240pOn31kHz, false)
    DefineGetterSetterGeneric(CrtConf, SystemCRTScanlines31kHz, bool, Bool, sSystemCRTScanlines31kHz, false)
    DefineGetterSetterGeneric(CrtConf, SystemCRTExtended15KhzRange, bool, Bool, sSystemCRTExtended15KhzRange, false)
    DefineGetterSetterGeneric(CrtConf, SystemCRTSuperrez, std::string, String, sSystemCRTSuperrez, "x6")
    DefineGetterSetterGeneric(CrtConf, SystemCRTUseV2, bool, Bool, sSystemCRTUseV2, false)
    DefineGetterSetterGeneric(CrtConf, SystemCRTForceJack, bool, Bool, sSystemCRTForceJack, false)
    DefineGetterSetterGeneric(CrtConf, SystemCRTForceHDMI, bool, Bool, sSystemCRTForceHDMI, false)

    DefineCrtModeOffsetDeclaration(VerticalOffset, int, Int, sVerticalOffset)
    DefineCrtModeOffsetDeclaration(HorizontalOffset, int, Int, sHorizontalOffset)
    DefineCrtViewportDeclaration(Width, int, Int, sWidth)

    static CrtResolution CrtResolutionFromString(const std::string& menu);
    static const std::string& CrtResolutionFromEnum(CrtResolution type);

  private:
    static constexpr const char* sSystemCRT                       = "adapter.type";
    static constexpr const char* sSystemCRTResolution             = "options.es.resolution";
    static constexpr const char* sSystemCRTGameRegionSelect       = "options.regionselect";
    static constexpr const char* sSystemCRTGameResolutionSelect   = "options.resolutionselect";
    static constexpr const char* sSystemCRTUseV2                  = "options.usev2";
    static constexpr const char* sSystemCRTSuperrez               = "options.superrez";
    static constexpr const char* sSystemCRT31kHzResolution        = "options.31khz.es.resolution";
    static constexpr const char* sSystemCRTRunDemoIn240pOn31kHz   = "options.31khz.demo240pOn31khz";
    static constexpr const char* sSystemCRTScanlines31kHz         = "options.31khz.scanlines";
    static constexpr const char* sSystemCRTExtended15KhzRange     = "options.15khz.extendedrange";
    static constexpr const char* sSystemCRTForceJack              = "audio.forcejack";
    static constexpr const char* sSystemCRTForceHDMI              = "video.forcehdmi";

    static constexpr const char* sViewportPrefix                  = "viewport";
    static constexpr const char* sModeOffsetPrefix                = "mode.offset";
    static constexpr const char* sVerticalOffset                  = "verticaloffset";
    static constexpr const char* sHorizontalOffset                = "horizontaloffset";
    static constexpr const char* sWidth                           = "width";

    static CrtAdapterType CrtAdapterFromString(const std::string& adapter);
    static const std::string& CrtAdapterFromEnum(CrtAdapterType adapter);
};



