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
                               , private IOptionListComponent<ScraperType>
                               , private ISwitchComponent
                               , private IGuiMenuBase
{
  public:
    explicit GuiMenuScraper(WindowManager& window, SystemManager& systemManager);

  private:
    enum class Components
    {
        Scraper,
        ScraperAuto,
        ScraperOptions,
        ScrapeNameFrom,
        ScrapingMethod,
        Systems,
    };

    std::shared_ptr<OptionListComponent<ScraperType>> mScrapers;
    std::shared_ptr<OptionListComponent<ScrapingMethod>> mScrapingMethod;
    std::shared_ptr<OptionListComponent<SystemData*>> mSystems;

    //! SystemManager reference
    SystemManager& mSystemManager;

    static std::vector<ListEntry<ScrapingMethod>> GetScrapingMethods();
    std::vector<ListEntry<SystemData*>> GetSystemsEntries();
    static std::vector<ListEntry<ScraperType>> GetScrapersEntries();
    static std::vector<ListEntry<ScraperNameOptions>> GetNameOptionsEntries();

    void SubMenuSelected(int id) override;

    void start();

    /*
     * IOptionListComponent<ScraperNameOptions> implementation
     */

    void OptionListComponentChanged(int id, int index, const ScraperNameOptions& value) override;

    /*
     * IOptionListComponent<ScraperFactory::ScraperType> implementation
     */

    void OptionListComponentChanged(int id, int index, const ScraperType& value) override;

    /*
     * ISwitchInterface implementation
     */

    void SwitchComponentChanged(int id, bool status) override;

    void start();
};
