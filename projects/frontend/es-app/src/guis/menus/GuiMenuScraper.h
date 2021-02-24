//
// Created by gugue_u on 03/01/2021.
//

#pragma once

#include <guis/menus/GuiMenuBase.h>
#include <components/SwitchComponent.h>
#include <systems/SystemManager.h>
#include <scraping/ScraperTools.h>
#include <scraping/ScraperFactory.h>

class GuiMenuScraper : public GuiMenuBase
                               , private IOptionListComponent<ScraperNameOptions>
                               , private IGuiMenuBase
{
  public:
    explicit GuiMenuScraper(WindowManager& window, SystemManager& systemManager);

  private:
    enum class Components
    {
        Scraper,
        ScraperOptions,
        ScrapeNameFrom,
        ScrapingMethod,
        Systems,
    };

    std::shared_ptr<OptionListComponent<ScraperFactory::ScraperType>> mScrapers;
    std::shared_ptr<OptionListComponent<ScraperNameOptions>> mScrapeNameOptions;
    std::shared_ptr<OptionListComponent<ScrappingMethod>> mScrapingMethod;
    std::shared_ptr<OptionListComponent<SystemData*>> mSystems;

    std::vector<ListEntry<ScrappingMethod>> GetScrapingMethods();
    std::vector<ListEntry<SystemData*>> GetSystemsEntries();
    std::vector<ListEntry<ScraperFactory::ScraperType>> GetScrapersEntries();
    std::vector<ListEntry<ScraperNameOptions>> GetNameOptionsEntries();

    void OptionListComponentChanged(int id, int index, const ScraperNameOptions& value) override;

    SystemManager& mSystemManager;

    void SubMenuSelected(int id) override;

    void pressedStart();
    void start();
};