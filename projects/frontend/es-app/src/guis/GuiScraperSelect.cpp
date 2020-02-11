//
// Created by bkg2k on 29/12/2019.
//

#include <scraping/ScraperFactory.h>
#include <scraping/ScraperTools.h>
#include <components/SwitchComponent.h>
#include "GuiScraperSelect.h"
#include "GuiScraperOptions.h"
#include "MenuMessages.h"

GuiScraperSelect::GuiScraperSelect(Window& window, SystemManager& systemManager)
  :  Gui(window),
     mSystemManager(systemManager),
     mMenu(window, _("SCRAPER"))
{
  addChild(&mMenu);

  // scrape from
  mScrapers = std::make_shared<OptionListComponent<std::string> >(mWindow, _("SCRAPE FROM"), false);
  std::vector<std::string> scrapers = ScraperFactory::Instance().GetScraperList();
  for (auto& scraper : scrapers)
    mScrapers->add(scraper, scraper, scraper == Settings::Instance().Scraper());
  mMenu.addWithLabel(mScrapers, _("SCRAPE FROM"), _(MENUMESSAGE_SCRAPER_FROM_HELP_MSG));

  ScraperNameOptions nameOption = ScraperTools::Clamp(RecalboxConf::Instance().AsInt("scraper.getnamefrom"));
  mScrapeNameFrom = std::make_shared<OptionListComponent<ScraperNameOptions>>(mWindow, _("GET GAME NAME FROM"), false);
  mScrapeNameFrom->add(_("Scrapper results"), ScraperNameOptions::GetFromScraper, nameOption == ScraperNameOptions::GetFromScraper);
  mScrapeNameFrom->add(_("Raw filename"), ScraperNameOptions::GetFromFilename, nameOption == ScraperNameOptions::GetFromFilename);
  mScrapeNameFrom->add(_("Undecorated filename"), ScraperNameOptions::GetFromFilenameUndecorated, nameOption == ScraperNameOptions::GetFromFilenameUndecorated);
  mMenu.addWithLabel(mScrapeNameFrom, _("GET GAME NAME FROM"), _(MENUMESSAGE_SCRAPER_GET_NAME_FROM_HELP_MSG));

  mExtractRegion = std::make_shared<SwitchComponent>(mWindow, RecalboxConf::Instance().AsBool("scraper.extractregionfromfilename"));
  mMenu.addWithLabel(mExtractRegion, _("GET REGION FROM FILENAME WHEN POSSIBLE"), _(MENUMESSAGE_SCRAPER_EXTRACT_REGION_FROM_FILENAME_HELP_MSG));

  mMenu.addButton(_("SCRAPE NOW"), "start", [this, &window, &systemManager] { SaveOptions(); window.pushGui(new GuiScraperOptions(window, systemManager)); });
  mMenu.addButton(_("BACK"), "back", [&] { Close(); });

  mMenu.setPosition((Renderer::getDisplayWidthAsFloat() - mMenu.getSize().x()) / 2, (Renderer::getDisplayHeightAsFloat() - mMenu.getSize().y()) / 2);
}

void GuiScraperSelect::SaveOptions()
{
  RecalboxConf::Instance().SetInt("scraper.getnamefrom", (int)mScrapeNameFrom->getSelected());
  RecalboxConf::Instance().SetBool("scraper.extractregionfromfilename", mExtractRegion->getState());
}
