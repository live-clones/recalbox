//
// Created by gugue_u on 03/01/2021.
//

#include <scraping/ScraperFactory.h>
#include <guis/GuiScraperRun.h>
#include <guis/GuiMsgBox.h>
#include "GuiMenuScreenScraperOptions.h"

GuiMenuScreenScraperOptions::GuiMenuScreenScraperOptions(WindowManager& window, SystemManager& systemManager)
: GuiMenuBase(window, _("SCREENSCRAPER OPTIONS"), nullptr),
mSystemManager(systemManager)
{
  mImage = AddList<ScreenScraperEnums::ScreenScraperImageType>(_("SELECT IMAGE TYPE"), (int)Components::Image, this, GetImagesEntries(), "");
  mThumbnail = AddList<ScreenScraperEnums::ScreenScraperImageType>(_("SELECT THUMBNAIL TYPE"), (int)Components::Thumbnail, this, GetThumbnailsEntries(), "");
  mVideo = AddList<ScreenScraperEnums::ScreenScraperVideoType>(_("SELECT VIDEO TYPE"), (int)Components::Video, this, GetVideosEntries(), "");

  mExtractRegionMethod = AddList<ScreenScraperEnums::ScreenScraperRegionPriority>(_("SELECT REGION PRIORITY"), (int)Components::RegionPriority, this, GetRegionOptionsEntries(), _(""));
  mRegion = AddList<Regions::GameRegions>(_("SELECT FAVORITE REGION"), (int)Components::Region, this, GetRegionsEntries(), "");
  mLanguage = AddList<Languages>(_("SELECT FAVORITE LANGUAGE"), (int)Components::Language, this, GetLanguagesEntries(), "");

  mManual = AddSwitch(_("DOWNLOAD GAME MANUALS"), RecalboxConf::Instance().GetScreenScraperWantManual(), (int)Components::Manuals, this, "");
  mMaps = AddSwitch(_("DOWNLOAD GAME MAPS"), RecalboxConf::Instance().GetScreenScraperWantMaps(), (int)Components::Maps, this, "");
  mP2k = AddSwitch(_("INSTALL PAD-2-KEYBOARD CONFIGURATIONS"), RecalboxConf::Instance().GetScreenScraperWantP2K(), (int)Components::PK2, this, "");

  mLogin = AddEditable(_("USERNAME"), RecalboxConf::Instance().GetScreenScraperLogin(), (int)Components::Login, this, false);
  mPassword = AddEditable(_("PASSWORD"), RecalboxConf::Instance().GetScreenScraperPassword(), (int)Components::Password, this, true);
}

std::vector<GuiMenuBase::ListEntry<ScreenScraperEnums::ScreenScraperImageType>> GuiMenuScreenScraperOptions::GetImagesEntries()
{
  ScreenScraperEnums::ScreenScraperImageType imageCode = RecalboxConf::Instance().GetScreenScraperMainMedia();
  if(imageCode == ScreenScraperEnums::ScreenScraperImageType::Unknown) imageCode = ScreenScraperEnums::ScreenScraperImageType::MixV1;
  std::vector<ListEntry<ScreenScraperEnums::ScreenScraperImageType>> list;
  list.push_back({ _("In-game screenshot"), ScreenScraperEnums::ScreenScraperImageType::ScreenshotIngame, ScreenScraperEnums::ScreenScraperImageType::ScreenshotIngame == imageCode });
  list.push_back({ _("Title screenshot"),   ScreenScraperEnums::ScreenScraperImageType::ScreenshotTitle,  ScreenScraperEnums::ScreenScraperImageType::ScreenshotTitle == imageCode });
  list.push_back({ _("Clear logo"),         ScreenScraperEnums::ScreenScraperImageType::Wheel,            ScreenScraperEnums::ScreenScraperImageType::Wheel == imageCode });
  list.push_back({ _("Marquee"),            ScreenScraperEnums::ScreenScraperImageType::Marquee,          ScreenScraperEnums::ScreenScraperImageType::Marquee == imageCode });
  list.push_back({ _("box2D"),              ScreenScraperEnums::ScreenScraperImageType::Box2d,            ScreenScraperEnums::ScreenScraperImageType::Box2d == imageCode });
  list.push_back({ _("box3d"),              ScreenScraperEnums::ScreenScraperImageType::Box3d,            ScreenScraperEnums::ScreenScraperImageType::Box3d == imageCode });
  list.push_back({ _("ScreenScraper Mix V1"),ScreenScraperEnums::ScreenScraperImageType::MixV1,          ScreenScraperEnums::ScreenScraperImageType::MixV1 == imageCode });
  list.push_back({ _("ScreenScraper Mix V2"), ScreenScraperEnums::ScreenScraperImageType::MixV2,         ScreenScraperEnums::ScreenScraperImageType::MixV2 == imageCode });
  return list;
}

std::vector<GuiMenuBase::ListEntry<ScreenScraperEnums::ScreenScraperImageType>> GuiMenuScreenScraperOptions::GetThumbnailsEntries()
{
  ScreenScraperEnums::ScreenScraperImageType thumbnailCode = RecalboxConf::Instance().GetScreenScraperThumbnail();
  if(thumbnailCode == ScreenScraperEnums::ScreenScraperImageType::Unknown) thumbnailCode = ScreenScraperEnums::ScreenScraperImageType::None;

  std::vector<ListEntry<ScreenScraperEnums::ScreenScraperImageType>> list;
  list.push_back({ _("No thumbnail"),       ScreenScraperEnums::ScreenScraperImageType::None,             ScreenScraperEnums::ScreenScraperImageType::None == thumbnailCode });
  list.push_back({ _("In-game screenshot"), ScreenScraperEnums::ScreenScraperImageType::ScreenshotIngame, ScreenScraperEnums::ScreenScraperImageType::ScreenshotIngame == thumbnailCode });
  list.push_back({ _("Title screenshot"),   ScreenScraperEnums::ScreenScraperImageType::ScreenshotTitle,  ScreenScraperEnums::ScreenScraperImageType::ScreenshotTitle == thumbnailCode });
  list.push_back({ _("Clear logo"),         ScreenScraperEnums::ScreenScraperImageType::Wheel,            ScreenScraperEnums::ScreenScraperImageType::Wheel == thumbnailCode });
  list.push_back({ _("Marquee"),            ScreenScraperEnums::ScreenScraperImageType::Marquee,          ScreenScraperEnums::ScreenScraperImageType::Marquee == thumbnailCode });
  list.push_back({ _("box2D"),              ScreenScraperEnums::ScreenScraperImageType::Box2d,            ScreenScraperEnums::ScreenScraperImageType::Box2d == thumbnailCode });
  list.push_back({ _("box3d"),              ScreenScraperEnums::ScreenScraperImageType::Box3d,            ScreenScraperEnums::ScreenScraperImageType::Box3d == thumbnailCode });
  list.push_back({ _("ScreenScraper Mix V1"), ScreenScraperEnums::ScreenScraperImageType::MixV1,          ScreenScraperEnums::ScreenScraperImageType::MixV1 == thumbnailCode });
  list.push_back({ _("ScreenScraper Mix V2"), ScreenScraperEnums::ScreenScraperImageType::MixV2,         ScreenScraperEnums::ScreenScraperImageType::MixV2 == thumbnailCode });
  return list;
}

std::vector<GuiMenuBase::ListEntry<ScreenScraperEnums::ScreenScraperVideoType>> GuiMenuScreenScraperOptions::GetVideosEntries()
{
  ScreenScraperEnums::ScreenScraperVideoType videoCode = RecalboxConf::Instance().GetScreenScraperVideo();
  std::vector<ListEntry<ScreenScraperEnums::ScreenScraperVideoType>> list;
  list.push_back({ _("No video"), ScreenScraperEnums::ScreenScraperVideoType::None, ScreenScraperEnums::ScreenScraperVideoType::None == videoCode });
  list.push_back({ _("Original video"),   ScreenScraperEnums::ScreenScraperVideoType::Raw,  ScreenScraperEnums::ScreenScraperVideoType::Raw == videoCode});
  list.push_back({ _("Optimized/Normalized video"),         ScreenScraperEnums::ScreenScraperVideoType::Optimized,ScreenScraperEnums::ScreenScraperVideoType::Optimized == videoCode });
  return list;
}

std::vector<GuiMenuBase::ListEntry<ScreenScraperEnums::ScreenScraperRegionPriority>> GuiMenuScreenScraperOptions::GetRegionOptionsEntries()
{
  ScreenScraperEnums::ScreenScraperRegionPriority regionOption = RecalboxConf::Instance().GetScreenScraperRegionPriority();
  std::vector<ListEntry<ScreenScraperEnums::ScreenScraperRegionPriority>> list;

  list.push_back({_("DETECTED REGION"), ScreenScraperEnums::ScreenScraperRegionPriority::DetectedRegion, ScreenScraperEnums::ScreenScraperRegionPriority::DetectedRegion == regionOption });
  list.push_back({_("FAVORITE REGION"), ScreenScraperEnums::ScreenScraperRegionPriority::FavoriteRegion, ScreenScraperEnums::ScreenScraperRegionPriority::FavoriteRegion == regionOption });
  return list;
}

std::vector<GuiMenuBase::ListEntry<Regions::GameRegions>> GuiMenuScreenScraperOptions::GetRegionsEntries()
{
  Regions::GameRegions confRegion = RecalboxConf::Instance().GetScreenScraperRegion();
  if(Regions::GameRegions::Unknown == confRegion)
    confRegion = Regions::GameRegions::WOR;

  std::vector<ListEntry<Regions::GameRegions>> list;
  for(Regions::GameRegions region : Regions::AvailableRegions()){
    if(Regions::GameRegions::Unknown == region) continue;
    list.push_back({ Regions::RegionFullName(region), region , region == confRegion });
  }
  return list;
}

std::vector<GuiMenuBase::ListEntry<Languages>> GuiMenuScreenScraperOptions::GetLanguagesEntries()
{
  Languages  scrapingLanguage = GetScrapingLanguage();
  std::vector<ListEntry<Languages>> list;
  for(Languages language : LanguagesTools::AvailableLanguages()){
    if(Languages::Unknown == language)
      continue;
    list.push_back({ LanguagesTools::LanguagesFullName(language), language , language == scrapingLanguage });
  }
  return list;
}

Languages GuiMenuScreenScraperOptions::GetScrapingLanguage()
{
  Languages confLanguage = RecalboxConf::Instance().GetScreenScraperLanguage();
  if(Languages::Unknown != confLanguage)
    return confLanguage;

  const std::string languagesStd = RecalboxConf::Instance().GetSystemLanguage();
  Languages systemLanguage =  LanguagesTools::DeserializeLanguage(languagesStd.substr(0, 2));
  if(Languages::Unknown != systemLanguage)
    return systemLanguage;

  return Languages::EN;
}


void GuiMenuScreenScraperOptions::EditableComponentTextChanged(int id, const std::string& text)
{
  if ((Components)id == Components::Login) RecalboxConf::Instance().SetScreenScraperLogin(text).Save();
  else if ((Components)id == Components::Password) RecalboxConf::Instance().SetScreenScraperPassword(text).Save();
}

void GuiMenuScreenScraperOptions::OptionListComponentChanged(int id, int index,
                                                             const ScreenScraperEnums::ScreenScraperImageType& value)
{
  (void)index;
  if((Components)id == Components::Image)
    RecalboxConf::Instance().SetScreenScraperMainMedia(value);
  if((Components)id == Components::Thumbnail)
    RecalboxConf::Instance().SetScreenScraperThumbnail(value);
}

void GuiMenuScreenScraperOptions::OptionListComponentChanged(int id, int index,
                                                             const ScreenScraperEnums::ScreenScraperVideoType& value)
{
  (void)index;
  if((Components)id == Components::Video)
    RecalboxConf::Instance().SetScreenScraperVideo(value);
}

void GuiMenuScreenScraperOptions::SwitchComponentChanged(int id, bool status)
{
  if ((Components)id == Components::Manuals) RecalboxConf::Instance().SetScreenScraperWantManual(status).Save();;
  if ((Components)id == Components::Maps) RecalboxConf::Instance().SetScreenScraperWantMaps(status).Save();;
  if ((Components)id == Components::PK2) RecalboxConf::Instance().SetScreenScraperWantP2K(status).Save();;
}

void GuiMenuScreenScraperOptions::OptionListComponentChanged(int id, int index,
                                                             const ScreenScraperEnums::ScreenScraperRegionPriority& value)
{
  (void)index;
  if((Components)id == Components::RegionPriority)
    RecalboxConf::Instance().SetScreenScraperRegionPriority(value);
}

void GuiMenuScreenScraperOptions::OptionListComponentChanged(int id, int index,
                                                             const Regions::GameRegions& value)
{
  (void)index;
  if((Components)id == Components::Region)
    RecalboxConf::Instance().SetScreenScraperRegion(value);
}

void GuiMenuScreenScraperOptions::OptionListComponentChanged(int id, int index,
                                                             const Languages& value)
{
  (void)index;
  if((Components)id == Components::Language)
    RecalboxConf::Instance().SetScreenScraperLanguage(value);
}