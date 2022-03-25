#pragma once

#include "guis/Gui.h"
#include "components/NinePatchComponent.h"
#include "components/ComponentGrid.h"
#include "components/TextComponent.h"
#include <scraping/scrapers/IScraperEngine.h>
#include <components/ButtonComponent.h>
#include "components/ProgressBarComponent.h"

class ScraperSearchComponent;
class TextComponent;

class GuiScraperRun : public Gui, public INotifyScrapeResult
{
public:
	GuiScraperRun(WindowManager&window, SystemManager& systemManager, const SystemManager::SystemList& systems, ScrapingMethod method);
	~GuiScraperRun() override = default;

	void onSizeChanged() override;
	bool getHelpPrompts(Help& help) override { return mGrid.getHelpPrompts(help); }

private:
	void finish();

  //! SystemManager instance
	SystemManager& mSystemManager;

	//! Scraper interface
	IScraperEngine* mScraper;

	//! Start time
	DateTime mStart;

  //! Last scrapping result
  ScrapeResult mResult;

	SystemManager::SystemList mSearchQueue;

	NinePatchComponent mBackground;
	ComponentGrid mGrid;

	std::shared_ptr<TextComponent> mTitle;
	std::shared_ptr<TextComponent> mSystem;
	std::shared_ptr<TextComponent> mSubtitle;
	std::shared_ptr<ScraperSearchComponent> mSearchComp;
	std::shared_ptr<TextComponent> mTiming;
  std::shared_ptr<TextComponent> mDatabaseMessage;
  std::shared_ptr<ButtonComponent> mButton;
	std::shared_ptr<ComponentGrid> mButtonGrid;
  std::shared_ptr<TextComponent> mFinalReport;
  std::shared_ptr<ComponentGrid> mProgressGrid;
  std::shared_ptr<ProgressBarComponent> mBar;

	// Running state
	enum class State
  {
	  Running,     //!< Running (scraping)
	  StopPending, //!< Stop pending
	  Stopped,     //!< Stopped, display statistics
	  OverQuota,   //!< Stopped, over quota
	  DiskFull,    //!< Stopped, disk full
  };

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
