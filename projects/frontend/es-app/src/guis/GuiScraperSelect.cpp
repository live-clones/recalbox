//
// Created by bkg2k on 29/12/2019.
//

#include <scraping/ScraperFactory.h>
#include <scraping/ScraperTools.h>
#include <components/SwitchComponent.h>
#include "GuiScraperSelect.h"
#include "GuiScraperOptions.h"
#include "MenuMessages.h"

GuiScraperSelect::GuiScraperSelect(WindowManager& window, SystemManager& systemManager)
  :  Gui(window),
     mSystemManager(systemManager),
     mMenu(window, _("SCRAPER"))
{
  addChild(&mMenu);

  // scrape from
  mScrapers = std::make_shared<OptionListComponent<std::string> >(mWindow, _("SCRAPE FROM"), false);
  std::vector<std::string> scrapers = ScraperFactory::GetScraperList();
  for (auto& scraper : scrapers)
    mScrapers->add(scraper, scraper, scraper == Settings::Instance().Scraper());
  mMenu.addWithLabel(mScrapers, _("SCRAPE FROM"), _(MENUMESSAGE_SCRAPER_FROM_HELP_MSG));

  ScraperNameOptions nameOption = ScraperTools::Clamp(RecalboxConf::Instance().GetScraperGetNameFrom());
  mScrapeNameFrom = std::make_shared<OptionListComponent<ScraperNameOptions>>(mWindow, _("GET GAME NAME FROM"), false);
  mScrapeNameFrom->add(_("Scrapper results"), ScraperNameOptions::GetFromScraper, nameOption == ScraperNameOptions::GetFromScraper);
  mScrapeNameFrom->add(_("Raw filename"), ScraperNameOptions::GetFromFilename, nameOption == ScraperNameOptions::GetFromFilename);
  mScrapeNameFrom->add(_("Undecorated filename"), ScraperNameOptions::GetFromFilenameUndecorated, nameOption == ScraperNameOptions::GetFromFilenameUndecorated);
  mScrapeNameFrom->setChangedCallback(std::bind(&GuiScraperSelect::SaveOptions, this));
  mMenu.addWithLabel(mScrapeNameFrom, _("GET GAME NAME FROM"), _(MENUMESSAGE_SCRAPER_GET_NAME_FROM_HELP_MSG));

  mExtractRegion = std::make_shared<SwitchComponent>(mWindow, RecalboxConf::Instance().GetScraperRegionFromFilename());
  mExtractRegion->setChangedCallback(std::bind(&GuiScraperSelect::SaveOptions, this));
  mMenu.addWithLabel(mExtractRegion, _("GET REGION FROM FILENAME WHEN POSSIBLE"), _(MENUMESSAGE_SCRAPER_EXTRACT_REGION_FROM_FILENAME_HELP_MSG));

  mMenu.addButton(_("SCRAPE NOW"), "start", [this, &window, &systemManager] { window.pushGui(new GuiScraperOptions(window, systemManager)); });
  mMenu.addButton(_("BACK"), "back", [this] { Close(); });

  mMenu.setPosition((Renderer::getDisplayWidthAsFloat() - mMenu.getSize().x()) / 2, (Renderer::getDisplayHeightAsFloat() - mMenu.getSize().y()) / 2);
}

void GuiScraperSelect::SaveOptions()
{
  RecalboxConf::Instance().SetScraperGetNameFrom((int)mScrapeNameFrom->getSelected());
  RecalboxConf::Instance().SetScraperRegionFromFilename(mExtractRegion->getState());
  RecalboxConf::Instance().Save();
}

bool GuiScraperSelect::ProcessInput(const InputCompactEvent& event)
{
  if (event.APressed())
  {
    Close();
    return true;
  }

  if (event.StartPressed())
  {
    // close everything
    mWindow.CloseAll();
    return true;
  }

  return Component::ProcessInput(event);
}

bool GuiScraperSelect::getHelpPrompts(Help& help)
{
  help.Set(HelpType::A, _("BACK"))
      .Set(HelpType::Start, _("CLOSE"));
  return true;
}