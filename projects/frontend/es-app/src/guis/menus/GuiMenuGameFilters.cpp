//
// Created by gugue_u on 29/03/2022.
//
//

#include "GuiMenuGameFilters.h"
#include <RecalboxConf.h>
#include <views/gamelist/ISimpleGameListView.h>
#include <games/GameFilesUtils.h>
#include "views/ViewController.h"
#include "components/SwitchComponent.h"
#include "utils/locale/LocaleHelper.h"
#include "guis/MenuMessages.h"
#include "GuiMenu.h"
#include "guis/GuiMsgBox.h"

GuiMenuGameFilters::GuiMenuGameFilters(WindowManager& window, SystemData& system)
  :	GuiMenuBase(window, _("Game Filters"), nullptr)
  , mSystem(system)
  , mGamelist(*ViewController::Instance().getGameListView(&system))
{

  if (!system.IsFavorite())
    AddSwitch(_("SHOW ONLY LATEST VERSION") + " (BETA)", RecalboxConf::Instance().GetShowOnlyLatestVersion(), (int)Components::ShowOnlyLatestVersion, this, _(MENUMESSAGE_GAMELISTOPTION_ONLY_LASt_VERSION));

  if (!system.IsFavorite())
    mFavoritesOnly = AddSwitch(_("SHOW ONLY FAVORITES"), RecalboxConf::Instance().GetFavoritesOnly(), (int)Components::FavoritesOnly, this, _(MENUMESSAGE_GAMELISTOPTION_FAVORITES_ONLY_MSG));

   if (!system.IsFavorite())
    AddSwitch(_("SHOW HIDDEN GAMES "), RecalboxConf::Instance().GetShowHidden(), (int)Components::ShowHidden, this, _(MENUMESSAGE_GAMELISTOPTION_SHOW_HIDDEN_MSG));

  AddSwitch(_("HIDE ADULT GAMES"), RecalboxConf::Instance().GetFilterAdultGames(), (int)Components::Adult, this, _(MENUMESSAGE_GAMELISTOPTION_HIDE_ADULT_MSG));

  AddSwitch(_("HIDE PREINSTALLED GAMES"), RecalboxConf::Instance().GetGlobalHidePreinstalled(), (int)Components::HidePreinstalled, this, _(MENUMESSAGE_GAME_HIDE_PREINSTALLED));

  AddSwitch(_("HIDE NO GAMES"), RecalboxConf::Instance().GetHideNoGames(), (int)Components::NoGames, this, _(MENUMESSAGE_GAME_HIDE_PREINSTALLED));

}

GuiMenuGameFilters::~GuiMenuGameFilters()
{
  if(!ViewController::Instance().CheckFilters())
   ManageSystems();
}

std::vector<GuiMenuBase::ListEntry<Regions::GameRegions>> GuiMenuGameFilters::GetRegionEntries()
{
  std::vector<GuiMenuBase::ListEntry<Regions::GameRegions>> list;

  Regions::GameRegions currentRegion = Regions::Clamp(RecalboxConf::Instance().GetSystemRegionFilter(mSystem));
  for(auto region : mGamelist.AvailableRegionsInGames())
  {
    std::string regionName = (region == Regions::GameRegions::Unknown) ? _("NONE") : Regions::RegionFullName(region);
    list.push_back({ regionName, region, region == currentRegion });
  }

  return list;
}

std::vector<GuiMenuBase::ListEntry<FileSorts::Sorts>> GuiMenuGameFilters::GetSortEntries()
{
  std::vector<GuiMenuBase::ListEntry<FileSorts::Sorts>> list;

  // Get & check sort id
  const std::vector<FileSorts::Sorts>& availableSorts = FileSorts::AvailableSorts(mSystem.IsVirtual());
  FileSorts::Sorts currentSort = (FileSorts::Sorts)RecalboxConf::Instance().GetSystemSort(mSystem);
  if (std::find(availableSorts.begin(), availableSorts.end(), currentSort) == availableSorts.end())
    currentSort = FileSorts::Sorts::FileNameAscending;

  for(FileSorts::Sorts sort : availableSorts)
    list.push_back({ FileSorts::Name(sort), sort, sort == currentSort });

  return list;
}


std::vector<GuiMenuBase::ListEntry<unsigned int>> GuiMenuGameFilters::GetLetterEntries()
{
  std::vector<GuiMenuBase::ListEntry<unsigned int>> list;

  // Get available letters
  std::vector<unsigned int> letters = mGamelist.getAvailableLetters();
  if (!letters.empty())
  {
    // Get current unicode char
    unsigned int currentUnicode = Strings::UpperChar(mGamelist.getCursor()->Name());
    // Build list
    for (unsigned int unicode : letters)
      list.push_back({ Strings::unicode2Chars(unicode), unicode, unicode == currentUnicode });
  }

  return list;
}

void GuiMenuGameFilters::SwitchComponentChanged(int id, bool status)
{
  switch((Components)id)
  {
    case Components::ShowOnlyLatestVersion:
      RecalboxConf::Instance().SetShowOnlyLatestVersion(status).Save();
      ManageSystems();
      break;
    case Components::FavoritesOnly:
      RecalboxConf::Instance().SetFavoritesOnly(status).Save();
      ManageSystems();
      break;
    case Components::ShowHidden:
      RecalboxConf::Instance().SetShowHidden(status).Save();
      ManageSystems();
      break;
    case Components::Adult:
      RecalboxConf::Instance().SetFilterAdultGames(status).Save();
      ManageSystems();
      break;
    case Components::HidePreinstalled:
      if(status)
      {
        RecalboxConf::Instance().SetGlobalHidePreinstalled(status).Save();
        ManageSystems();
        break;
      }
      else
      {
        RecalboxConf::Instance().SetGlobalHidePreinstalled(status).Save();
        ManageSystems();
        break;
      }
    case Components::NoGames:
      RecalboxConf::Instance().SetHideNoGames(status).Save();
      ManageSystems();
      break;
  }
}

void GuiMenuGameFilters::ManageSystems()
{
  FileData* game = mGamelist.getCursor();
  mGamelist.refreshList();
  mGamelist.setCursor(game);

  ViewController::Instance().setAllInvalidGamesList(nullptr);
  ViewController::Instance().getSystemListView().populate();
}
