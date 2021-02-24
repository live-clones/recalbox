//
// Created by gugue_U on 18/02/2021.
//

#pragma once

#include <string>

class ScreenScraperEnums
{
  public:
    enum class ScreenScraperRegionPriority
    {
        DetectedRegion,
        FavoriteRegion,
    };

    enum class ScreenScraperImageType
    {
        None,             //!< No image
        ScreenshotIngame, //!< In-game screenshot
        ScreenshotTitle,  //!< Title screenshot
        Wheel,            //!< Wheel
        Marquee,          //!< Marquee
        Box2d,            //!< Box 2d (front)
        Box3d,            //!< Box 3d (front)
        MixV1,            //!< Server-side mix v1
        MixV2,            //!< Server-side mix v2
        Unknown,          //!< Unknown
    };

    enum class ScreenScraperVideoType
    {
        None,      //!< No video
        Raw,       //!< Unprocessed video
        Optimized, //!< Optimized video
    };

    static ScreenScraperRegionPriority ScreenScraperRegionPriorityFromString(const std::string& scraperRegionOptions);
    static const std::string& ScreenScraperRegionPriorityFromEnum(ScreenScraperRegionPriority scraperRegionOptions);

    static ScreenScraperImageType ScreenScraperImageTypeFromString(const std::string& imageType);
    static const std::string& ScreenScraperImageTypeFromEnum(ScreenScraperImageType imageType);

    static ScreenScraperVideoType ScreenScraperVideoTypeFromString(const std::string& videoType);
    static const std::string& ScreenScraperVideoTypeFromEnum(ScreenScraperVideoType videoType);
};