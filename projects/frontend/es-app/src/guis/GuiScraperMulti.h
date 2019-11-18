#pragma once

#include "GuiComponent.h"
#include "components/NinePatchComponent.h"
#include "components/ComponentGrid.h"
#include "scrapers/Scraper.h"
#include "components/TextComponent.h"

#include <queue>

class ScraperSearchComponent;
class TextComponent;

class GuiScraperMulti : public GuiComponent
{
public:
	GuiScraperMulti(Window* window, SystemManager& systemManager, const std::queue<ScraperSearchParams>& searches, bool approveResults);
	~GuiScraperMulti() override;

	void onSizeChanged() override;
	std::vector<HelpPrompt> getHelpPrompts() override { return mGrid.getHelpPrompts(); }

private:
	void acceptResult(const ScraperSearchResult& result);
	void skip();
	void doNextSearch();
	
	void finish();

  //! SystemManager instance
	SystemManager& mSystemManager;

	unsigned int mTotalGames;
	unsigned int mCurrentGame;
	unsigned int mTotalSuccessful;
	unsigned int mTotalSkipped;
	std::queue<ScraperSearchParams> mSearchQueue;

	NinePatchComponent mBackground;
	ComponentGrid mGrid;

	std::shared_ptr<TextComponent> mTitle;
	std::shared_ptr<TextComponent> mSystem;
	std::shared_ptr<TextComponent> mSubtitle;
	std::shared_ptr<ScraperSearchComponent> mSearchComp;
	std::shared_ptr<ComponentGrid> mButtonGrid;
};
