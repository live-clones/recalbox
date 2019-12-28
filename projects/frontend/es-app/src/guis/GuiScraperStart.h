#pragma once

#include "guis/Gui.h"
#include "systems/SystemData.h"
#include "components/MenuComponent.h"

typedef std::function<bool(SystemData*, FileData*)> GameFilterFunc;

template<typename T>
class OptionListComponent;

class SwitchComponent;

//The starting point for a multi-game scrape.
//Allows the user to set various parameters (to set filters, to set which systems to scrape, to enable manual mode).
//Generates a list of "searches" that will be carried out by GuiScraperLog.
class GuiScraperStart : public Gui
{
public:
	explicit GuiScraperStart(Window& window, SystemManager& systemManager);

	bool ProcessInput(const InputCompactEvent& event) override;

	bool getHelpPrompts(Help& help) override;

	std::shared_ptr<SwitchComponent> mMixImages;

private:
	void pressedStart();
	void start();

  //! SystemManager instance
	SystemManager& mSystemManager;

	std::shared_ptr< OptionListComponent<GameFilterFunc> > mFilters;
	std::shared_ptr< OptionListComponent<SystemData*> > mSystems;
	std::shared_ptr<SwitchComponent> mApproveResults;

	MenuComponent mMenu;
};
