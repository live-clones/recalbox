#pragma once

#include <views/ViewController.h>
#include <views/gamelist/ISimpleGameListView.h>
#include <components/TextListComponent.h>
#include <games/EmptyData.h>
#include <scraping/scrapers/IScraperEngineStage.h>

class BasicGameListView : public ISimpleGameListView
                        , private IScraperEngineStage
{
public:
	BasicGameListView(WindowManager& window, SystemManager& systemManager, SystemData& system);

	// Called when a FileData* is added, has its metadata changed, or is removed
	void onFileChanged(FileData* file, FileChangeType change) override;

	void onThemeChanged(const ThemeData& theme) override;

  int Count() const override { return mList.Count(); };
  bool IsEmpty() const override { return mList.IsEmpty(); };

  FileData* getCursor() override { return mList.getSelected(); }
	int getCursorIndex() override { return mList.getCursorIndex(); }
	int getCursorIndexMax() override {	return mList.size() - 1; }
	void setCursorStack(FileData* file) override;
	void setCursor(FileData* file) override;
	void setCursorIndex(int index) override;
  void removeEntry(FileData* fileData) override;

	const char* getName() const override { return "basic"; }

	void populateList(const FolderData& folder) final;

	void refreshList() override { populateList(*mPopulatedFolder); }

	void DoUpdateGameInformation(bool update) override
	{
    (void)update;
	  mWindow.UpdateHelpSystem();
	}

	FileData::List getFileDataList() override;

  /*!
   * @brief Update list - ony used for seamless scraping
   * @param delta Elapsed time since the last update
   */
  void Update(int delta) override;

  /*!
   * @brief Get available regions from the current game list
   * @return Region list (may be empty)
   */
  Regions::List AvailableRegionsInGames() override;

  /*!
   * @brief Get available regions from the given listt
   * @return Region list (may be empty)
   */
  static Regions::List AvailableRegionsInGames(FileData::List& list);

protected:
	void launch(FileData* game) override { ViewController::Instance().Launch(game, GameLinkedData(), Vector3f()); }

  FileData* getEmptyListItem() override { return &mEmptyListItem; }

  TextListComponent<FileData*> mList;

  std::string GetDisplayName(FileData* game);

  //! Elapsed time on the selected game (hovering time)
  int mElapsedTimeOnGame;
  //! Scraping state (for visual feedback)
  bool mIsScraping;

	//! At least one game has a GenreID
	bool mHasGenre;

private:
  //! Hovering time in ms before starting a seamless scrape
  static constexpr int sMaxHoveringTimeBeforeScraping = 200; // 200ms
  //! Elapsed time since starting seamless scraping before displaying busy animation
  static constexpr int sMaxScrapingTimeBeforeBusyAnim = 200; // 200ms

  EmptyData mEmptyListItem;
  const FolderData *mPopulatedFolder;

	std::string getItemIcon(FileData* item);

  /*!
   * @brief Called when a game is selected
   */
  void OnGameSelected() final;

  /*
   * IScrapeEngineStage implementation
   */

  /*!
   * @brief Report scraping stage completion
   * @param game Target game
   * @param stage Last stage completed
   */
  void StageCompleted(FileData* game, Stage stage) override;
};
