#pragma once

#include "GuiComponent.h"
#include "systems/SystemData.h"
#include "scrapers/Scraper.h"
#include "components/MenuComponent.h"
#include <queue>

typedef std::function<bool(SystemData*, FileData*)> GameFilterFunc;

template<typename T>
class OptionListComponent;

class SwitchComponent;

//The starting point for a multi-game scrape.
//Allows the user to set various parameters (to set filters, to set which systems to scrape, to enable manual mode).
//Generates a list of "searches" that will be carried out by GuiScraperLog.
class GuiScraperStart : public GuiComponent
{
public:
	explicit GuiScraperStart(Window* window, SystemManager& systemManager);

	bool ProcessInput(const InputCompactEvent& event) override;

	std::vector<HelpPrompt> getHelpPrompts() override;

	std::shared_ptr<SwitchComponent> mMixImages;

private:
	void pressedStart();
	void start();
	static std::queue<ScraperSearchParams> getSearches(std::vector<SystemData*> systems, const GameFilterFunc& selector);

  //! SystemManager instance
	SystemManager& mSystemManager;

	std::shared_ptr< OptionListComponent<GameFilterFunc> > mFilters;
	std::shared_ptr< OptionListComponent<SystemData*> > mSystems;
	std::shared_ptr<SwitchComponent> mApproveResults;

	MenuComponent mMenu;
};
