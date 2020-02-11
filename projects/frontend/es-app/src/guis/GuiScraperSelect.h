//
// Created by bkg2k on 29/12/2019.
//
#pragma once

#include <guis/Gui.h>
#include <components/MenuComponent.h>
#include <components/OptionListComponent.h>
#include <scraping/ScraperTools.h>

class SystemManager;
class SwitchComponent;


class GuiScraperSelect : public Gui
{
  public:
    explicit GuiScraperSelect(Window& window, SystemManager& systemManager);

  private:
    /*!
     * @brief Save options
     */
    void SaveOptions();

    //! SystemManager instance
    SystemManager& mSystemManager;
    std::shared_ptr<OptionListComponent<std::string>>        mScrapers;
    std::shared_ptr<OptionListComponent<ScraperNameOptions>> mScrapeNameFrom;
    std::shared_ptr<SwitchComponent>                         mExtractRegion;
    MenuComponent mMenu;
};
