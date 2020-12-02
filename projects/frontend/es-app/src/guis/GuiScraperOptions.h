#pragma once

#include "guis/Gui.h"
#include "systems/SystemData.h"
#include "components/MenuComponent.h"

template<typename T>

class OptionListComponent;
class SwitchComponent;

// The starting point for a multi-game scrape.
// Allows the user to set various parameters.
class GuiScraperOptions : public Gui
{
public:
	explicit GuiScraperOptions(WindowManager& window, SystemManager& systemManager);

	bool ProcessInput(const InputCompactEvent& event) override;

	bool getHelpPrompts(Help& help) override;

private:
	void pressedStart();
	void start();

  //! SystemManager instance
	SystemManager& mSystemManager;

  std::shared_ptr<SwitchComponent>                      mP2k;
  std::shared_ptr<SwitchComponent>                      mMaps;
  std::shared_ptr<SwitchComponent>                      mManual;
  std::shared_ptr<OptionListComponent<std::string>>     mImage;
  std::shared_ptr<OptionListComponent<std::string>>     mThumbnail;
  std::shared_ptr<OptionListComponent<std::string>>     mVideo;
	std::shared_ptr<OptionListComponent<std::string>>     mRegion;
	std::shared_ptr<OptionListComponent<std::string>>     mLanguage;
	std::shared_ptr<SwitchComponent>                      mScrapeRatings;
	std::shared_ptr<OptionListComponent<ScrappingMethod>> mScrapingMethod;
	std::shared_ptr<OptionListComponent<SystemData*>>     mSystems;
	std::shared_ptr<SwitchComponent> mApproveResults;

	MenuComponent mMenu;
};
