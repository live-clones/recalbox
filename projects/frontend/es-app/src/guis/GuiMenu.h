#pragma once

#include "guis/Gui.h"
#include "components/MenuComponent.h"
#include "components/OptionListComponent.h"
#include <functional>
#include <Window.h>
#include <systems/SystemData.h>
#include "guis/GuiSettings.h"

class StrInputConfig
{
 public:
  StrInputConfig(const std::string& ideviceName, const std::string& ideviceGUIDString) {
    deviceName = ideviceName;
    deviceGUIDString = ideviceGUIDString;
  }

  std::string deviceName;
  std::string deviceGUIDString;
};

class GuiMenu : public Gui
{
public:
	GuiMenu(Window& window, SystemManager& systemManager);
	~GuiMenu() override;

	bool ProcessInput(const InputCompactEvent& event) override;
	void onSizeChanged() override;
	std::vector<HelpPrompt> getHelpPrompts() override;

private:
	void addEntry(const char* name, unsigned int color, bool add_arrow, const std::function<void()>& func, const Path& iconName = Path());
	void addEntryWithHelp(const char* name, const std::string& help, unsigned int color, bool add_arrow, const std::function<void()>& func, const Path& iconName = Path());

	void createInputTextRow(GuiSettings * gui, const std::string& title, const char* settingsID, bool password, const std::string& help);
	void menuSystem();
	void menuUpdates();
	void menuGameSettings();
	void menuControllers();
	void menuUISettings();
	void menuSoundSettings();
	void menuNetworkSettings();
	void menuScrapper();
	void menuAdvancedSettings();
	void menuQuit();

	//! SystemManager instance
	SystemManager& mSystemManager;

	MenuComponent mMenu;
	TextComponent mVersion;
	std::shared_ptr<MenuTheme> mMenuTheme;


	std::shared_ptr<OptionListComponent<std::string>> createRatioOptionList(Window& window,
                                                                        const std::string& configname) const;

	void popSystemConfigurationGui(SystemData *systemData) const;

	std::vector<StrInputConfig*> mLoadedInput; // used to keep information about loaded devices in case there are unpluged between device window load and save
	void clearLoadedInput();
};
