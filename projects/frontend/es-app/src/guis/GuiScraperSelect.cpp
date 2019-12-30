//
// Created by bkg2k on 29/12/2019.
//

#include <scraping/new/ScraperFactory.h>
#include "GuiScraperSelect.h"
#include "GuiScraperOptions.h"
#include "MenuMessages.h"

GuiScraperSelect::GuiScraperSelect(Window& window, SystemManager& systemManager)
  :  Gui(window),
     mSystemManager(systemManager),
     mMenu(window, _("SCRAPER").c_str())
{
  addChild(&mMenu);

  // scrape from
  mScrapers = std::make_shared<OptionListComponent<std::string> >(mWindow, _("SCRAPE FROM"), false);
  std::vector<std::string> scrapers = ScraperFactory::GetScraperList();
  for (auto& scraper : scrapers)
    mScrapers->add(scraper, scraper, scraper == Settings::Instance().Scraper());
  mMenu.addWithLabel(mScrapers, _("SCRAPE FROM"), _(MenuMessages::SCRAPER_FROM_HELP_MSG));

  mMenu.addButton(_("SCRAPE NOW"), "start", [&window, &systemManager] { window.pushGui(new GuiScraperOptions(window, systemManager)); });
  mMenu.addButton(_("BACK"), "back", [&] { Close(); });

  mMenu.setPosition((Renderer::getDisplayWidthAsFloat() - mMenu.getSize().x()) / 2, (Renderer::getDisplayHeightAsFloat() - mMenu.getSize().y()) / 2);
}
