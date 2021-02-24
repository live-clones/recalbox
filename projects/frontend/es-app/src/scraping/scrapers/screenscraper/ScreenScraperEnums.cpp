//
// Created by gugue_u on 18/02/2021.
//

#include <scraping/ScraperTools.h>
#include "ScreenScraperEnums.h"

ScreenScraperEnums::ScreenScraperRegionPriority ScreenScraperEnums::ScreenScraperRegionPriorityFromString(const std::string& regionPriority)
{
  if (regionPriority == "favoriteRegion") return ScreenScraperEnums::ScreenScraperRegionPriority::FavoriteRegion;
  return ScreenScraperEnums::ScreenScraperRegionPriority::DetectedRegion;
}

const std::string& ScreenScraperEnums::ScreenScraperRegionPriorityFromEnum(ScreenScraperRegionPriority regionPriority)
{
  switch (regionPriority)
  {
    case ScreenScraperEnums::ScreenScraperRegionPriority::FavoriteRegion:
    {
      static std::string sScraper = "favoriteRegion";
      return sScraper;
    }
    case ScreenScraperEnums::ScreenScraperRegionPriority::DetectedRegion:
    {
      break;
    }
  }
  static std::string sScraper = "DetectedRegion";
  return sScraper;
}

ScreenScraperEnums::ScreenScraperImageType ScreenScraperEnums::ScreenScraperImageTypeFromString(const std::string& imageType)
{
  if (imageType == "None") return ScreenScraperImageType::None;
  if (imageType == "InGameScreenShot") return ScreenScraperImageType::ScreenshotIngame;
  if (imageType == "TitleScreenshot") return ScreenScraperImageType::ScreenshotTitle;
  if (imageType == "ClearLogo") return ScreenScraperImageType::Wheel;
  if (imageType == "Marquee") return ScreenScraperImageType::Marquee;
  if (imageType == "Box2D") return ScreenScraperImageType::Box2d;
  if (imageType == "Box3D") return ScreenScraperImageType::Box3d;
  if (imageType == "MixV1") return ScreenScraperImageType::MixV1;
  if (imageType == "MixV2") return ScreenScraperImageType::MixV2;
  return ScreenScraperImageType::Unknown;
}

const std::string& ScreenScraperEnums::ScreenScraperImageTypeFromEnum(ScreenScraperImageType imageType)
{
  switch (imageType)
  {
    case ScreenScraperImageType::None:
    {
      static std::string sImageType = "None";
      return sImageType;
    }
    case ScreenScraperImageType::ScreenshotIngame:
    {
      static std::string sImageType = "InGameScreenShot";
      return sImageType;
    }
    case ScreenScraperImageType::ScreenshotTitle:
    {
      static std::string sImageType = "TitleScreenshot";
      return sImageType;
    }
    case ScreenScraperImageType::Wheel:
    {
      static std::string sImageType = "ClearLogo";
      return sImageType;
    }
    case ScreenScraperImageType::Marquee:
    {
      static std::string sImageType = "Marquee";
      return sImageType;
    }
    case ScreenScraperImageType::Box2d:
    {
      static std::string sImageType = "Box2D";
      return sImageType;
    }
    case ScreenScraperImageType::Box3d:
    {
      static std::string sImageType = "Box3D";
      return sImageType;
    }
    case ScreenScraperImageType::MixV1:
    {
      static std::string sImageType = "MixV1";
      return sImageType;
    }
    case ScreenScraperImageType::MixV2:
    {
      static std::string sImageType = "MixV2";
      return sImageType;
    }
    case ScreenScraperImageType::Unknown:
    {
      break;
    }
  }
  static std::string sImageType = "Unknown";
  return sImageType;
}

ScreenScraperEnums::ScreenScraperVideoType ScreenScraperEnums::ScreenScraperVideoTypeFromString(const std::string& videoType)
{
  if (videoType == "NormalizedVideo") return ScreenScraperVideoType::Optimized;
  if (videoType == "OriginalVideo") return ScreenScraperVideoType::Raw;
  return ScreenScraperVideoType::None;
}

const std::string& ScreenScraperEnums::ScreenScraperVideoTypeFromEnum(ScreenScraperVideoType videoType)
{
  switch (videoType)
  {
    case ScreenScraperVideoType::None:
    {
      static std::string sScraper = "None";
      return sScraper;
    }
    case ScreenScraperVideoType::Raw:
    {
      static std::string sFileName = "OriginalVideo";
      return sFileName;
    }
    case ScreenScraperVideoType::Optimized:
    {
      break;
    }
  }
  static std::string sFileName = "NormalizedVideo";
  return sFileName;
}