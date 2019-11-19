#include <systems/SystemManager.h>
#include "guis/GuiScraperStart.h"
#include "guis/GuiScraperMulti.h"
#include "guis/GuiMsgBox.h"
#include "views/ViewController.h"

#include "components/OptionListComponent.h"
#include "components/SwitchComponent.h"
#include "Locale.h"
#include "Settings.h"

GuiScraperStart::GuiScraperStart(Window& window, SystemManager& systemManager)
  : GuiComponent(window),
    mSystemManager(systemManager),
    mMenu(window, _("SCRAPE NOW").c_str())
{
	addChild(&mMenu);

	// add filters (with first one selected)
	mFilters = std::make_shared< OptionListComponent<GameFilterFunc> >(mWindow, _("SCRAPE THESE GAMES"), false);
	mFilters->add(_("All Games"), 
		[](SystemData*, FileData*) -> bool { return true; }, false);
	mFilters->add(_("Only missing image"), 
		[](SystemData*, FileData* g) -> bool { return g->Metadata().Image().Empty(); }, true);
	mMenu.addWithLabel(mFilters, _("FILTER"));

	// add systems (all with a platformid specified selected)
	mSystems = std::make_shared< OptionListComponent<SystemData*> >(mWindow, _("SCRAPE THESE SYSTEMS"), true);
	for (auto it : mSystemManager.GetVisibleSystemList())
	{
		if(!it->hasPlatformId(PlatformIds::PlatformId::PLATFORM_IGNORE))
			mSystems->add(it->getFullName(), it, it->PlatformCount() != 0);
	}
	mMenu.addWithLabel(mSystems, _("SYSTEMS"));

	// add mix images option (if scraper = screenscraper)
	std::string scraperName = Settings::Instance().Scraper();

	if(scraperName == "Screenscraper") {
		mMixImages = std::make_shared<SwitchComponent>(mWindow);
		mMixImages->setState(true);
		mMenu.addWithLabel(mMixImages, _("USE COMPOSED VISUALS"));
	}

	mApproveResults = std::make_shared<SwitchComponent>(mWindow);
	mApproveResults->setState(false);
	mMenu.addWithLabel(mApproveResults, _("USER DECIDES ON CONFLICTS"));

	mMenu.addButton(_("START"), "start", std::bind(&GuiScraperStart::pressedStart, this));
	mMenu.addButton(_("BACK"), "back", [&] { Close(); });

	mMenu.setPosition((Renderer::getDisplayWidthAsFloat() - mMenu.getSize().x()) / 2, (Renderer::getDisplayHeightAsFloat() - mMenu.getSize().y()) / 2);
}

void GuiScraperStart::pressedStart()
{
	for (auto& system : mSystems->getSelectedObjects())
	{
		if(system->PlatformCount() == 0)
		{
			mWindow.pushGui(new GuiMsgBox(mWindow,
				_("WARNING: SOME OF YOUR SELECTED SYSTEMS DO NOT HAVE A PLATFORM SET. RESULTS MAY BE EVEN MORE INACCURATE THAN USUAL!\nCONTINUE ANYWAY?"), 
						       _("YES"), std::bind(&GuiScraperStart::start, this), 
						       _("NO"), nullptr));
			return;
		}
	}

	start();
}

void GuiScraperStart::start()
{
	std::queue<ScraperSearchParams> searches = getSearches(mSystems->getSelectedObjects(), mFilters->getSelected());
	if(Settings::Instance().Scraper() == "Screenscraper") {
		Settings::Instance().SetMixImages(mMixImages->getState());
	}

	if(searches.empty())
	{
		mWindow.pushGui(new GuiMsgBox(mWindow,
					       _("NO GAMES FIT THAT CRITERIA.")));
	}else{
		GuiScraperMulti* gsm = new GuiScraperMulti(mWindow, mSystemManager, searches, mApproveResults->getState());
		mWindow.pushGui(gsm);
		Close();
	}
}

std::queue<ScraperSearchParams> GuiScraperStart::getSearches(std::vector<SystemData*> systems, const GameFilterFunc& selector)
{
	std::queue<ScraperSearchParams> queue;
	for (auto& system : systems)
	{
		FileData::List games = system->getRootFolder()->getAllItemsRecursively(false);
		for (auto& game : games)
		{
			if(selector(system, game))
			{
				ScraperSearchParams search;
				search.game = game;
				search.system = system;
				
				queue.push(search);
			}
		}
	}

	return queue;
}

bool GuiScraperStart::ProcessInput(const InputCompactEvent& event)
{
	bool consumed = GuiComponent::ProcessInput(event);
	if(consumed)
		return true;
	
	if (event.APressed())
	{
		Close();
		return true;
	}

	if (event.StartPressed())
	{
		// close everything
		while((mWindow.peekGui() != nullptr) && mWindow.peekGui() != &ViewController::Instance())
			delete mWindow.peekGui();
	}


	return false;
}

std::vector<HelpPrompt> GuiScraperStart::getHelpPrompts()
{
	std::vector<HelpPrompt> prompts = mMenu.getHelpPrompts();
	prompts.push_back(HelpPrompt("a", _("BACK")));
	prompts.push_back(HelpPrompt("start", _("CLOSE")));
	return prompts;
}
