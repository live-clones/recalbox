//
// Created by gugue_u on 03/01/2021.
//

#pragma once

#include <guis/menus/GuiMenuBase.h>
#include <components/SwitchComponent.h>
#include <systems/SystemManager.h>
#include <scraping/scrapers/IScraperEngine.h>
#include <scraping/scrapers/screenscraper/ScreenScraperApis.h>

class GuiMenuScreenScraperOptions : public GuiMenuBase
  , private IEditableComponent
  , private IOptionListComponent<ScreenScraperEnums::ScreenScraperImageType>
  , private IOptionListComponent<ScreenScraperEnums::ScreenScraperVideoType>
  , private IOptionListComponent<ScreenScraperEnums::ScreenScraperRegionPriority>
  , private IOptionListComponent<Languages>
  , private IOptionListComponent<Regions::GameRegions>
  , private ISwitchComponent

{
  public:

    explicit GuiMenuScreenScraperOptions(WindowManager& window, SystemManager& systemManager);

  private:
    enum class Components
    {
        Image,
        Thumbnail,
        Video,
        Manuals,
        Maps,
        PK2,
        RegionPriority,
        Region,
        Language,
        Login,
        Password,
    };
  private:

    std::shared_ptr<EditableComponent> mLogin;
    std::shared_ptr<EditableComponent> mPassword;
    std::shared_ptr<SwitchComponent>                      mP2k;
    std::shared_ptr<SwitchComponent>                      mMaps;
    std::shared_ptr<SwitchComponent>                      mManual;
    std::shared_ptr<SwitchComponent>                      mExtractRegionFromRom;
    std::shared_ptr<OptionListComponent<ScreenScraperEnums::ScreenScraperImageType>>     mImage;
    std::shared_ptr<OptionListComponent<ScreenScraperEnums::ScreenScraperImageType>>     mThumbnail;
    std::shared_ptr<OptionListComponent<ScreenScraperEnums::ScreenScraperVideoType>>     mVideo;
    std::shared_ptr<OptionListComponent<ScreenScraperEnums::ScreenScraperRegionPriority>>       mExtractRegionMethod;
    std::shared_ptr<OptionListComponent<Regions::GameRegions>>     mRegion;
    std::shared_ptr<OptionListComponent<Languages>>     mLanguage;
    std::shared_ptr<SwitchComponent> mApproveResults;

    //! System Manager instance
    SystemManager& mSystemManager;


    std::vector<ListEntry<ScrappingMethod>> GetScrapingMethods();
    std::vector<ListEntry<SystemData*>> GetSystemsEntries();
    static std::vector<ListEntry<ScreenScraperEnums::ScreenScraperImageType>> GetImagesEntries();
    static std::vector<ListEntry<ScreenScraperEnums::ScreenScraperImageType>> GetThumbnailsEntries();
    static std::vector<ListEntry<ScreenScraperEnums::ScreenScraperVideoType>> GetVideosEntries();
    std::vector<ListEntry<ScreenScraperEnums::ScreenScraperRegionPriority>> GetRegionOptionsEntries();
    std::vector<ListEntry<Regions::GameRegions>> GetRegionsEntries();
    std::vector<ListEntry<Languages>> GetLanguagesEntries();

    void EditableComponentTextChanged(int id, const std::string& text) override;
    void OptionListComponentChanged(int id, int index, const ScreenScraperEnums::ScreenScraperImageType& value) override;
    void OptionListComponentChanged(int id, int index, const ScreenScraperEnums::ScreenScraperVideoType& value) override;
    void OptionListComponentChanged(int id, int index, const ScreenScraperEnums::ScreenScraperRegionPriority& value) override;
    void OptionListComponentChanged(int id, int index, const Regions::GameRegions& value) override;
    void OptionListComponentChanged(int id, int index, const Languages& value) override;

    void SwitchComponentChanged(int id, bool status) override;
};