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

    explicit GuiMenuScreenScraperOptions(WindowManager& window, SystemManager& systemManager, ScraperType scraperType);

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

    //! System Manager instance
    SystemManager& mSystemManager;

    //! Scraper type
    ScraperType mType;

    static std::vector<ListEntry<ScrapingMethod>> GetScrapingMethods();
    static std::vector<ListEntry<SystemData*>> GetSystemsEntries();
    static std::vector<ListEntry<ScreenScraperEnums::ScreenScraperImageType>> GetImagesEntries();
    static std::vector<ListEntry<ScreenScraperEnums::ScreenScraperImageType>> GetThumbnailsEntries();
    static std::vector<ListEntry<ScreenScraperEnums::ScreenScraperVideoType>> GetVideosEntries();
    static std::vector<ListEntry<ScreenScraperEnums::ScreenScraperRegionPriority>> GetRegionOptionsEntries();
    static std::vector<ListEntry<Regions::GameRegions>> GetRegionsEntries();
    static std::vector<ListEntry<Languages>> GetLanguagesEntries();

    void EditableComponentTextChanged(int id, const std::string& text) override;
    void OptionListComponentChanged(int id, int index, const ScreenScraperEnums::ScreenScraperImageType& value) override;
    void OptionListComponentChanged(int id, int index, const ScreenScraperEnums::ScreenScraperVideoType& value) override;
    void OptionListComponentChanged(int id, int index, const ScreenScraperEnums::ScreenScraperRegionPriority& value) override;
    void OptionListComponentChanged(int id, int index, const Regions::GameRegions& value) override;
    void OptionListComponentChanged(int id, int index, const Languages& value) override;

    void SwitchComponentChanged(int id, bool status) override;

    //! Get login regarding scraper type
    std::string GetLogin();

    //! Get password regarding scraper type
    std::string GetPassword();

    //! Get login regarding scraper type
    void SetLogin(const std::string& login);

    //! Get password regarding scraper type
    void SetPassword(const std::string& password);
};