#pragma once

#include <scraping/scrapers/IScraperEngine.h>
#include <components/ButtonComponent.h>
#include "guis/Gui.h"
#include "components/ScraperSearchComponent.h"
#include "components/NinePatchComponent.h"
#include "components/TextComponent.h"

class GuiScraperSingleGameRun : public Gui, public INotifyScrapeResult
{
  public:
    class IScrappingComplete
    {
      public:
        virtual void ScrappingComplete(FileData& game) = 0;
    };

  private:
    //! Scraper interface
    IScraperEngine* mScraper;

    //! Target game
    FileData& mGame;

    //! Notification interface
    IScrappingComplete* mNotifier;

    /*
     * Components
     */

    ComponentGrid mGrid;
    NinePatchComponent mBox;

    std::shared_ptr<TextComponent> mGameName;
    std::shared_ptr<TextComponent> mSystemName;
    std::shared_ptr<ScraperSearchComponent> mSearch;
    std::shared_ptr<ButtonComponent> mButton;
    std::shared_ptr<ComponentGrid> mButtonGrid;

  public:
    explicit GuiScraperSingleGameRun(WindowManager&window, FileData& game, IScrappingComplete* notifier);

    void onSizeChanged() override;

    bool ProcessInput(const InputCompactEvent& event) override;
    void Update(int deltaTime) override;
    bool getHelpPrompts(Help& help) override;

    /*
     * INotifyScrapeResult implementation
     */

    /*!
     * @brief Notify a game has been scraped
     * @param index Game index
     * @param total Total game to scrape
     * @param result Result object
     */
    void GameResult(int index, int total, FileData* result) override;

    /*!
     * @brief Scraper site quota reached. Scraping is being aborted immediately.
     */
    void ScrapingComplete(ScrapeResult reason) override;
};
