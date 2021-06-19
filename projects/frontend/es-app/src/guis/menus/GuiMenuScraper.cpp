//
// Created by gugue_u on 03/01/2021.
//

#include <guis/menus/GuiMenuScraper.h>
#include <utils/locale/LocaleHelper.h>
#include <guis/MenuMessages.h>
#include <scraping/ScraperFactory.h>
#include <scraping/ScraperTools.h>
#include <guis/GuiScraperRun.h>
#include <guis/GuiMsgBox.h>
#include "GuiMenuNetwork.h"
#include "GuiMenuScreenScraperOptions.h"

GuiMenuScraper::GuiMenuScraper(WindowManager& window, SystemManager& systemManager)
  : GuiMenuBase(window, _("SCRAPER"), this),
  mSystemManager(systemManager)
{
  mScrapers = AddList<ScraperFactory::ScraperType>(_("SCRAPE FROM"), (int)Components::Scraper, nullptr, GetScrapersEntries(), _(MENUMESSAGE_SCRAPER_FROM_HELP_MSG));
  AddSubMenu(_("SCRAPER OPTIONS"), (int)Components::ScraperOptions);
  mScrapeNameOptions  = AddList<ScraperNameOptions>(_("GET GAME NAME FROM"), (int)Components::ScrapeNameFrom, this,
                                                    GetNameOptionsEntries(), _(MENUMESSAGE_SCRAPER_GET_NAME_FROM_HELP_MSG));
  mScrapingMethod = AddList<ScrappingMethod>(_("FILTER"), (int)Components::ScrapingMethod, nullptr, GetScrapingMethods(), "");
  mSystems = AddMultiList<SystemData*>(_("SYSTEMS"), (int)Components::Systems, nullptr, GetSystemsEntries(), "");

  // Buttons
  mMenu.addButton(_("SCRAPE NOW"), "start", [this] { pressedStart();});
}

std::vector<GuiMenuBase::ListEntry<ScraperFactory::ScraperType>> GuiMenuScraper::GetScrapersEntries()
{
  std::vector<ListEntry<ScraperFactory::ScraperType>> list;
  for(std::basic_string<char> scraper : ScraperFactory::GetScraperList())
  {
    list.push_back({ scraper, ScraperFactory::ScraperType::ScreenScraper, true });
  }
  return list;
}

std::vector<GuiMenuBase::ListEntry<ScraperNameOptions>> GuiMenuScraper::GetNameOptionsEntries()
{
  ScraperNameOptions nameOption = RecalboxConf::Instance().GetScraperNameOptions();
  std::vector<ListEntry<ScraperNameOptions>> list;

  list.push_back({ _("Scraper results"), ScraperNameOptions::GetFromScraper, nameOption == ScraperNameOptions::GetFromScraper });
  list.push_back({ _("Raw filename"), ScraperNameOptions::GetFromFilename, nameOption == ScraperNameOptions::GetFromFilename });
  list.push_back({ _("Undecorated filename"), ScraperNameOptions::GetFromFilenameUndecorated, nameOption == ScraperNameOptions::GetFromFilenameUndecorated });

  return list;
}

std::vector<GuiMenuBase::ListEntry<ScrappingMethod>> GuiMenuScraper::GetScrapingMethods()
{
  std::vector<ListEntry<ScrappingMethod>> list;
  list.push_back({ _("All Games"), ScrappingMethod::All, false });
  list.push_back({ _("Only missing image"), ScrappingMethod::IncompleteKeep, true });
  return list;
}

std::vector<GuiMenuBase::ListEntry<SystemData*>> GuiMenuScraper::GetSystemsEntries()
{
  std::vector<ListEntry<SystemData*>> list;
  for(SystemData* system : mSystemManager.GetVisibleSystemList())
  {
    if(!system->hasPlatformId(PlatformIds::PlatformId::PLATFORM_IGNORE))
      if (!system->IsVirtual() || system->IsFavorite() || system->IsPorts()) // Allow scraping favorites, but not virtual systems
        if (system->HasGame())
          list.push_back({ system->FullName(), system, system->HasPlatform() });
  }
  return list;
}

void GuiMenuScraper::OptionListComponentChanged(int id, int index, const ScraperNameOptions & value)
{
  (void)index;
  if ((Components)id == Components::ScrapeNameFrom)
    RecalboxConf::Instance().SetScraperNameOptions(value);
}

void GuiMenuScraper::SubMenuSelected(int id)
{
  if ((Components)id == Components::ScraperOptions)
  switch (mScrapers->getSelected())
  {
    case ScraperFactory::ScraperType::TheGameDB:
    case ScraperFactory::ScraperType::ScreenScraper:
        mWindow.pushGui(new GuiMenuScreenScraperOptions(mWindow, mSystemManager)); break;
  }
}

void GuiMenuScraper::pressedStart()
{
    for (auto& system : mSystems->getSelectedObjects())
    {
      if (!system->HasPlatform())
      {
        mWindow.pushGui(new GuiMsgBox(mWindow,
                                      _("WARNING: SOME OF YOUR SELECTED SYSTEMS DO NOT HAVE A PLATFORM SET. RESULTS MAY BE EVEN MORE INACCURATE THAN USUAL!\nCONTINUE ANYWAY?"),
                                      _("YES"), std::bind(&GuiMenuScraper::start, this), _("NO"),
                                      nullptr));
        return;
      }
    }
    start();
}

void GuiMenuScraper::start()
{
  GuiScraperRun* gsm = new GuiScraperRun(mWindow, mSystemManager, mSystems->getSelectedObjects(),

                                         mScrapingMethod->getSelected());
  mWindow.pushGui(gsm);
  Close();
}