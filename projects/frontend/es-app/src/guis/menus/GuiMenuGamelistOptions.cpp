#include "GuiMenuGamelistOptions.h"
#include <RecalboxConf.h>
#include <MainRunner.h>
#include <views/gamelist/ISimpleGameListView.h>
#include <games/GameFilesUtils.h>
#include "guis/GuiMetaDataEd.h"
#include "views/ViewController.h"
#include "components/SwitchComponent.h"
#include "utils/locale/LocaleHelper.h"
#include "guis/MenuMessages.h"
#include "guis/GuiMsgBox.h"
#include "guis/menus/GuiMenuQuit.h"
#include "GuiMenu.h"
#include "GuiMenuGamelistGameOptions.h"
#include "GuiMenuGamelistGameDeleteOptions.h"
#include "guis/GuiSearch.h"

GuiMenuGamelistOptions::GuiMenuGamelistOptions(WindowManager& window, SystemData& system, SystemManager& systemManager)
  :	GuiMenuBase(window, _("OPTIONS"), this)
  , mSystem(system)
  , mSystemManager(systemManager)
  , mGamelist(*ViewController::Instance().getGameListView(&system))
{
  // edit game metadata
  bool nomenu = RecalboxConf::Instance().GetMenuType() == RecalboxConf::Menu::None;
  bool bartop = RecalboxConf::Instance().GetMenuType() == RecalboxConf::Menu::Bartop;
  if (!nomenu && !bartop)
  {
    mGame = AddSubMenu(Strings::Empty, (int) Components::MetaData,_(MENUMESSAGE_GAMELISTOPTION_EDIT_METADATA_MSG));
    RefreshGameMenuContext();

    if(!mGamelist.getCursor()->IsEmpty())
    {
      if (!mSystem.IsVirtual() && mGamelist.getCursor()->IsGame() && !mGamelist.getCursor()->TopAncestor().ReadOnly() &&
          !mSystem.IsScreenshots())
      {
        std::string text = _("DELETE GAME %s");
        Strings::ReplaceAllIn(text, "%s", Strings::ToUpperUTF8(mGamelist.getCursor()->Name()));
        AddSubMenu(text, (int) Components::Delete, _(MENUMESSAGE_GAMELISTOPTION_DELETE_GAME_MSG));
      }

      if (mSystem.IsScreenshots())
      {
        AddSubMenu(_("DELETE SCREENSHOT"), (int) Components::DeleteScreeshot);
      }
    }

  }

  // Display filename
  AddSwitch(_("DISPLAY BY FILENAME"), RecalboxConf::Instance().GetDisplayByFileName(), (int)Components::DisplayByFileName, this, _(MENUMESSAGE_GAMELISTOPTION_HIDE_ADULT_MSG));


  // Jump to letter
	mJumpToLetterList = AddList<unsigned int>(_("JUMP TO LETTER"), (int)Components::JumpToLetter, this, GetLetterEntries());

  // open search wheel for this system
  if (!system.IsFavorite())
  AddSubMenu(_("SEARCH GAMES HERE"),  (int)Components::Search, Strings::Empty);

	// Sorting
	if (!system.IsSelfSorted())
	  mListSort = AddList<FileSorts::Sorts>(_("SORT GAMES BY"), (int)Components::Sorts, this, GetSortEntries(), _(MENUMESSAGE_GAMELISTOPTION_SORT_GAMES_MSG));

	// Adult games
  mAdult = AddSwitch(_("HIDE ADULT GAMES"), RecalboxConf::Instance().GetSystemFilterAdult(mSystem), (int)Components::Adult, this, _(MENUMESSAGE_GAMELISTOPTION_HIDE_ADULT_MSG));

  // Region filter
  mListRegion = AddList<Regions::GameRegions>(_("HIGHLIGHT GAMES OF REGION..."), (int)Components::Regions, this, GetRegionEntries(), _(MENUMESSAGE_GAMELISTOPTION_FILTER_REGION_MSG));

  if (!system.IsFavorite())
    mFavoritesOnly = AddSwitch(_("FAVORITES ONLY"), RecalboxConf::Instance().GetFavoritesOnly(), (int)Components::FavoritesOnly, this, _(MENUMESSAGE_GAMELISTOPTION_FAVORITES_ONLY_MSG));

  if (!system.IsFavorite())
    mShowHidden = AddSwitch(_("SHOW HIDDEN"), RecalboxConf::Instance().GetShowHidden(), (int)Components::ShowHidden, this, _(MENUMESSAGE_GAMELISTOPTION_SHOW_HIDDEN_MSG));

  // flat folders
  if (!system.IsFavorite())
    if (!system.IsAlwaysFlat())
      mFlatFolders = AddSwitch(_("SHOW FOLDERS CONTENT"), RecalboxConf::Instance().GetSystemFlatFolders(mSystem), (int)Components::FlatFolders, this, _(MENUMESSAGE_GAMELISTOPTION_SHOW_FOLDER_CONTENT_MSG));

  // update game list
  if (!system.IsFavorite())
    AddSubMenu(_("UPDATE GAMES LISTS"), (int)Components::UpdateGamelist, _(MENUMESSAGE_UI_UPDATE_GAMELIST_HELP_MSG));

	if (!nomenu)
  {
    // Main menu
    AddSubMenu(_("MAIN MENU"), (int)Components::MainMenu);
    // QUIT
    AddSubMenu(_("QUIT"), (int)Components::Quit);
  }
}

GuiMenuGamelistOptions::~GuiMenuGamelistOptions()
{
  // require list refresh
	mGamelist.onChanged(ISimpleGameListView::Change::Update);
}

void GuiMenuGamelistOptions::RefreshGameMenuContext()
{
  if (!mGame) return;

  FileData* file = mGamelist.getCursor();
  if ((file == nullptr) || file->IsEmpty())
    mGame->setText(_("NO GAME"));
  else if (file->TopAncestor().ReadOnly())
    mGame->setText(_("NON EDITABLE GAME"));
  else if (file->IsGame())
  {
    std::string text = _("EDIT GAME %s");
    Strings::ReplaceAllIn(text, "%s", Strings::ToUpperUTF8(file->Name()));
    mGame->setText(text);
  }
  else if (file->IsFolder())
  {
    std::string text = _("EDIT FOLDER %s");
    Strings::ReplaceAllIn(text, "%s", Strings::ToUpperUTF8(file->Name()));
    mGame->setText(text);
  }
}

std::vector<GuiMenuBase::ListEntry<Regions::GameRegions>> GuiMenuGamelistOptions::GetRegionEntries()
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

std::vector<GuiMenuBase::ListEntry<FileSorts::Sorts>> GuiMenuGamelistOptions::GetSortEntries()
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


std::vector<GuiMenuBase::ListEntry<unsigned int>> GuiMenuGamelistOptions::GetLetterEntries()
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

void GuiMenuGamelistOptions::Delete(IGameListView* gamelistview, FileData& game)
{
  game.FilePath().Delete();
  if (game.Parent() != nullptr)
    game.Parent()->RemoveChild(&game); //unlink it so list repopulations triggered from onFileChanged won't see it

  gamelistview->onFileChanged(&game, FileChangeType::Removed); //tell the view
}

void GuiMenuGamelistOptions::Modified(IGameListView* gamelistview, FileData& game)
{
  gamelistview->refreshList();
  gamelistview->setCursor(&game);
  //gamelistview->onFileChanged(&game, FileChangeType::MetadataChanged);
}

void GuiMenuGamelistOptions::OptionListComponentChanged(int id, int index, const unsigned int& value)
{
  (void)index;
  if ((Components)id == Components::JumpToLetter)
  {
    FileSorts::Sorts sortId = mListSort->getSelected();

    // if sort is not alpha, need to force an alpha
    if (sortId != FileSorts::Sorts::FileNameAscending && sortId != FileSorts::Sorts::FileNameDescending)
    {
      sortId = FileSorts::Sorts::FileNameAscending;
      mListSort->select(sortId);
      // apply sort
      if (sortId != (FileSorts::Sorts)RecalboxConf::Instance().GetSystemSort(mSystem))
      {
        RecalboxConf::Instance().SetSystemSort(mSystem, sortId);
        mGamelist.onChanged(ISimpleGameListView::Change::Resort);
      }
    }

    mGamelist.jumpToLetter(value);
    RefreshGameMenuContext();
  }
}

void GuiMenuGamelistOptions::OptionListComponentChanged(int id, int index, const FileSorts::Sorts& value)
{
  (void)index;
  if ((Components)id == Components::Sorts)
  {
    FileData* game = mGamelist.getCursor();

    RecalboxConf::Instance().SetSystemSort(mSystem, value).Save();
    mGamelist.onChanged(ISimpleGameListView::Change::Resort);

    mGamelist.refreshList();
    mGamelist.setCursor(game);
    RefreshGameMenuContext();
  }
}

void GuiMenuGamelistOptions::OptionListComponentChanged(int id, int index, const Regions::GameRegions& value)
{
  (void)index;
  if ((Components)id == Components::Regions)
    RecalboxConf::Instance().SetSystemRegionFilter(mSystem, value).Save();
}

void GuiMenuGamelistOptions::SubMenuSelected(int id)
{
  switch((Components)id)
  {
    case Components::MetaData:
    {
      FileData* file = mGamelist.getCursor();
      if ((file != nullptr) && (!file->IsEmpty()) && (!file->TopAncestor().ReadOnly()))
        mWindow.pushGui(new GuiMenuGamelistGameOptions(mWindow, mGamelist, mSystemManager, mSystem, *mGamelist.getCursor()));
      break;
    }
    case Components::MainMenu: mWindow.pushGui(new GuiMenu(mWindow, mSystemManager)); Close(); break;
    case Components::Quit: GuiMenuQuit::PushQuitGui(mWindow); break;
    case Components::UpdateGamelist:
    {
      mWindow.pushGui(new GuiMsgBox(mWindow, _("REALLY UPDATE GAMES LISTS ?"),
                      _("YES"), [] { MainRunner::RequestQuit(MainRunner::ExitState::Relaunch, true); },
                      _("NO"), nullptr ));
      break;
    }
    case Components::Delete:
    {
        mWindow.pushGui(new GuiMenuGamelistGameDeleteOptions(mWindow, mGamelist, *mGamelist.getCursor()));
      break;
    }
    case Components::DeleteScreeshot:
    {
        mWindow.pushGui(new GuiMsgBox(mWindow, _("DELETE SCREENSHOT, CONFIRM?"), _("YES"), [this]
        {
            mGamelist.getCursor()->FilePath().Delete();
            FolderData* folder = mGamelist.getCursor()->Parent();
            folder->deleteChild(mGamelist.getCursor());
            mGamelist.onFileChanged(mGamelist.getCursor(), FileChangeType::Removed);
            mGamelist.refreshList();
            mWindow.deleteAllGui();

        }, _("NO"), {}));
        break;
    }
    case Components::Search:
    {
      mWindow.pushGui(new GuiSearch(mWindow, mSystemManager));
      break;
    }
    case Components::JumpToLetter:
    case Components::Sorts:
    case Components::Adult:
    case Components::Regions:
    case Components::FavoritesOnly:
    case Components::ShowHidden:
    case Components::DisplayByFileName:
    case Components::FlatFolders: break;
  }
}

void GuiMenuGamelistOptions::SwitchComponentChanged(int id, bool status)
{
  switch((Components)id)
  {
    case Components::Adult: RecalboxConf::Instance().SetSystemFilterAdult(mSystem, status).Save(); break;
    case Components::FavoritesOnly:
      RecalboxConf::Instance().SetFavoritesOnly(status).Save();
      ViewController::Instance().setAllInvalidGamesList(nullptr);
      break;
    case Components::ShowHidden:
      RecalboxConf::Instance().SetShowHidden(status).Save();
      ViewController::Instance().setAllInvalidGamesList(nullptr);
      break;
      case Components::DisplayByFileName:
      RecalboxConf::Instance().SetDisplayByFileName(status).Save();
      ViewController::Instance().setAllInvalidGamesList(nullptr);
      break;
    case Components::FlatFolders: RecalboxConf::Instance().SetSystemFlatFolders(mSystem, status).Save(); break;
    case Components::Regions:
    case Components::Sorts:
    case Components::JumpToLetter:
    case Components::Search:
    case Components::MetaData:
    case Components::MainMenu:
    case Components::UpdateGamelist:
    case Components::Delete:
    case Components::DeleteScreeshot:
    case Components::Quit: break;
  }

  FileData* game = mGamelist.getCursor();
  mGamelist.refreshList();
  mGamelist.setCursor(game);
  RefreshGameMenuContext();
}

