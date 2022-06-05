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
#include <emulators/run/GameRunner.h>
#include <patreon/PatronInfo.h>

GuiMenuScraper::GuiMenuScraper(WindowManager& window, SystemManager& systemManager)
  : GuiMenuBase(window, _("SCRAPER"), this),
  mSystemManager(systemManager)
{
  mScrapers = AddList<ScraperType>(_("SCRAPE FROM"), (int)Components::Scraper, this, GetScrapersEntries(), _(MENUMESSAGE_SCRAPER_FROM_HELP_MSG));

  if (PatronInfo::Instance().IsPatron())
    AddSwitch(_("AUTOMATIC SCRAPING"), RecalboxConf::Instance().GetScraperAuto(), (int)Components::ScraperAuto, this);

  AddSubMenu(_("SCRAPER OPTIONS"), (int)Components::ScraperOptions);

  AddList<ScraperNameOptions>(_("GET GAME NAME FROM"), (int)Components::ScrapeNameFrom, this,
                              GetNameOptionsEntries(), _(MENUMESSAGE_SCRAPER_GET_NAME_FROM_HELP_MSG));

  mScrapingMethod = AddList<ScrapingMethod>(_("FILTER"), (int)Components::ScrapingMethod, nullptr, GetScrapingMethods(), "");

  mSystems = AddMultiList<SystemData*>(_("SYSTEMS"), (int)Components::Systems, nullptr, GetSystemsEntries(), "");

  // Buttons
  mMenu.addButton(_("SCRAPE NOW"), "start", [this] { start(); });
}

std::vector<GuiMenuBase::ListEntry<ScraperType>> GuiMenuScraper::GetScrapersEntries()
{
  std::vector<ListEntry<ScraperType>> list;
  for(const auto& kv : ScraperFactory::GetScraperList())
    list.push_back({ kv.second, kv.first, kv.first == RecalboxConf::Instance().GetScraperSource() });
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

std::vector<GuiMenuBase::ListEntry<ScrapingMethod>> GuiMenuScraper::GetScrapingMethods()
{
  std::vector<ListEntry<ScrapingMethod>> list;
  list.push_back({ _("All Games"), ScrapingMethod::All, false });
  list.push_back({ _("Only missing image"), ScrapingMethod::AllIfNoithingExists, true });
  return list;
}

std::vector<GuiMenuBase::ListEntry<SystemData*>> GuiMenuScraper::GetSystemsEntries()
{
  std::vector<ListEntry<SystemData*>> list;
  for(SystemData* system : mSystemManager.GetVisibleSystemList())
  {
    if(!system->hasPlatformId(PlatformIds::PlatformId::PLATFORM_IGNORE))
      if (!system->IsVirtual() || system->IsFavorite() || system->IsPorts()) // Allow scraping favorites, but not virtual systems
        if (system->HasScrapableGame())
          list.push_back({ system->FullName(), system, false });
  }
  return list;
}

void GuiMenuScraper::OptionListComponentChanged(int id, int index, const ScraperType& value)
{
  (void)index;
  if ((Components)id == Components::Scraper)
    RecalboxConf::Instance().SetScraperSource(value).Save();
}

void GuiMenuScraper::OptionListComponentChanged(int id, int index, const ScraperNameOptions & value)
{
  (void)index;
  if ((Components)id == Components::ScrapeNameFrom)
    RecalboxConf::Instance().SetScraperNameOptions(value).Save();
}

void GuiMenuScraper::SubMenuSelected(int id)
{
  if ((Components)id == Components::ScraperOptions)
    switch (mScrapers->getSelected())
    {
      case ScraperType::TheGameDB:
      case ScraperType::Recalbox:
      case ScraperType::ScreenScraper:
          mWindow.pushGui(new GuiMenuScreenScraperOptions(mWindow, mSystemManager, RecalboxConf::Instance().GetScraperSource())); break;
    }
}

void GuiMenuScraper::start()
{
  if (mSystems->getSelectedObjects().empty())
  {
    std::string text = _("Please select one or more systems to scrape!");
    GuiMsgBox* msgBox = new GuiMsgBox(mWindow, text, _("OK"), nullptr);
    mWindow.pushGui(msgBox);
  }
  else
    GuiScraperRun::CreateOrShow(mWindow, mSystemManager, mSystems->getSelectedObjects(), mScrapingMethod->getSelected(), &GameRunner::Instance());
}

void GuiMenuScraper::SwitchComponentChanged(int id, bool status)
{
  if ((Components)id == Components::ScraperAuto)
    RecalboxConf::Instance().SetScraperAuto(status).Save();
}
