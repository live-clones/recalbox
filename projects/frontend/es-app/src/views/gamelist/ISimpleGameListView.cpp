#include <RecalboxConf.h>
#include <guis/GuiNetPlay.h>
#include <systems/SystemManager.h>
#include "guis/GuiGamelistOptions.h"
#include "views/gamelist/ISimpleGameListView.h"
#include "systems/SystemData.h"
#include "Window.h"
#include "views/ViewController.h"
#include "Settings.h"
#include "utils/locale/LocaleHelper.h"

ISimpleGameListView::ISimpleGameListView(Window& window, SystemManager& systemManager, FolderData* root)
  : IGameListView(window, root),
    mSystemManager(systemManager),
    mHeaderText(window),
    mHeaderImage(window),
    mBackground(window),
    mThemeExtras(window)
{
  mFavoritesCount = getRoot()->getSystem()->FavoritesCount();
  mFavoritesOnly = mFavoritesCount > 0 && Settings::Instance().FavoritesOnly();

  mHeaderText.setText("Logo Text");
  mHeaderText.setSize(mSize.x(), 0);
  mHeaderText.setPosition(0, 0);
  mHeaderText.setHorizontalAlignment(TextAlignment::Center);
  mHeaderText.setDefaultZIndex(50);

  mHeaderImage.setResize(0, mSize.y() * 0.185f);
  mHeaderImage.setOrigin(0.5f, 0.0f);
  mHeaderImage.setPosition(mSize.x() / 2, 0);
  mHeaderImage.setDefaultZIndex(50);

  mBackground.setResize(mSize.x(), mSize.y());
  mBackground.setDefaultZIndex(0);

  mIsFavoriteSystem = getRoot()->getSystem() == mSystemManager.FavoriteSystem();

  addChild(&mHeaderText);
  addChild(&mBackground);
}

void ISimpleGameListView::onThemeChanged(const ThemeData& theme)
{
  mBackground.applyTheme(theme, getName(), "background", ThemeProperties::All);
  mHeaderImage.applyTheme(theme, getName(), "logo", ThemeProperties::All);
  mHeaderText.applyTheme(theme, getName(), "logoText", ThemeProperties::All);

  // Remove old theme extras
  for (auto extra : mThemeExtras.getmExtras()) {
    removeChild(extra);
  }
  mThemeExtras.getmExtras().clear();

  mThemeExtras.setExtras(ThemeData::makeExtras(theme, getName(), mWindow));
  mThemeExtras.sortExtrasByZIndex();

  // Add new theme extras

  for (auto extra : mThemeExtras.getmExtras()) {
    addChild(extra);
  }


  if (mHeaderImage.hasImage()) {
    removeChild(&mHeaderText);
    addChild(&mHeaderImage);
  } else {
    addChild(&mHeaderText);
    removeChild(&mHeaderImage);
  }
}

void ISimpleGameListView::onFileChanged(FileData* file, FileChangeType change)
{
  if (change == FileChangeType::Run)
  {
    updateInfoPanel();
    return ;
  }

  if ((change == FileChangeType::Removed) && (file == getEmptyListItem()))
    return;

  if (file->isGame())
  {
    SystemData* favoriteSystem = mSystemManager.FavoriteSystem();
    bool isInFavorite = favoriteSystem->getRootFolder()->Contains(file, true);
    bool isFavorite = file->Metadata().Favorite();

    if (isInFavorite != isFavorite)
    {
      if (isInFavorite) favoriteSystem->getRootFolder()->removeChild(file);
      else favoriteSystem->getRootFolder()->addChild(file, false);
      ViewController::Instance().setInvalidGamesList(mSystemManager.FavoriteSystem());
      ViewController::Instance().getSystemListView().manageFavorite();
      mFavoritesCount = mFavoritesCount + (isFavorite ? 1 : -1);
      if (mFavoritesCount == 0) { mFavoritesOnly = false; }
      updateHelpPrompts();
    }
  }

  if (change == FileChangeType::Removed)
  {
    bool favorite = file->Metadata().Favorite();
    delete file;
    if (favorite)
    {
      ViewController::Instance().setInvalidGamesList(mSystemManager.FavoriteSystem());
      ViewController::Instance().getSystemListView().manageFavorite();
    }
  }

  int cursor = getCursorIndex();
  if (RecalboxConf::Instance().AsBool(getRoot()->getSystem()->getName() + ".flatfolder"))
  {
    populateList(getRoot());
  }
  else
  {
    refreshList();
  }
  setCursorIndex(cursor);

  updateInfoPanel();
}

bool ISimpleGameListView::ProcessInput(const InputCompactEvent& event) {
  bool hideSystemView = RecalboxConf::Instance().AsBool("emulationstation.hidesystemview");

  // RUN GAME or ENTER FOLDER
  if (event.BPressed())
  {
    FileData* cursor = getCursor();
    if (cursor->isGame())
    {
      //Sound::getFromTheme(getTheme(), getName(), "launch")->play();
      launch(cursor);
    }
    else if (cursor->isFolder())
    {
      FolderData* folder = (FolderData*)cursor;
      if (folder->hasChildren())
      {
        mCursorStack.push(folder);
        populateList(folder);
        setCursorIndex(0);
      }
    }
    RecalboxSystem::NotifyGame(*getCursor(), false, false);
    return true;
  }

  // BACK to PARENT FOLDER or PARENT SYSTEM
  if (event.APressed())
  {
    if (!mCursorStack.empty())
    {
      FolderData* selected = mCursorStack.top();

      // remove current folder from stack
      mCursorStack.pop();

      FolderData* cursor = !mCursorStack.empty() ? mCursorStack.top() : getRoot();
      populateList(cursor);

      setCursor(selected);
      //Sound::getFromTheme(getTheme(), getName(), "back")->play();
      RecalboxSystem::NotifyGame(*getCursor(), false, false);
    }
    else if (!hideSystemView)
    {
      onFocusLost();
      ViewController::Instance().goToSystemView(getRoot()->getSystem());
    }
    return true;
  }

  // TOGGLE FAVORITES
  if (event.YPressed()) {
    FileData* cursor = getCursor();
    if (cursor->isGame() && cursor->getSystem()->getHasFavoritesInTheme()) {
      MetadataDescriptor& md = cursor->Metadata();
      SystemData *favoriteSystem = mSystemManager.FavoriteSystem();

      md.SetFavorite(!md.Favorite());

      if (favoriteSystem != nullptr) {
        if (md.Favorite()) {
          favoriteSystem->getRootFolder()->addChild(cursor, false);
        } else {
          favoriteSystem->getRootFolder()->removeChild(cursor);
        }

        if (mIsFavoriteSystem) {
          ViewController::Instance().setInvalidGamesList(cursor->getSystem());
        } else {
          ViewController::Instance().setInvalidGamesList(favoriteSystem);
        }
        ViewController::Instance().getSystemListView().manageFavorite();
      }

      // Reload to refresh the favorite icon
      int cursorPlace = getCursorIndex();
      refreshList();
      setCursorIndex(cursorPlace);

      mFavoritesCount = mFavoritesCount + (md.Favorite() ? 1 : -1);
      if (mFavoritesCount == 0) { mFavoritesOnly = false; }
      updateHelpPrompts();
    }
    RecalboxSystem::NotifyGame(*getCursor(), false, false);
    return true;
  }

  // MOVE to NEXT GAMELIST
  if (event.AnyRightPressed())
  {
    if (Settings::Instance().QuickSystemSelect() && !hideSystemView) {
      onFocusLost();
      ViewController::Instance().goToNextGameList();
      RecalboxSystem::NotifyGame(*getCursor(), false, false);
      return true;
    }
  }

  // MOVE to PREVIOUS GAMELIST
  if (event.AnyLeftPressed()) {
    if (Settings::Instance().QuickSystemSelect() && !hideSystemView) {
      onFocusLost();
      ViewController::Instance().goToPrevGameList();
      RecalboxSystem::NotifyGame(*getCursor(), false, false);
      return true;
    }
  }


  // JUMP TO NEXT LETTER
  if (event.L1Pressed())
  {
    jumpToNextLetter(mSystem->getSortId() == 1 ? 1 : -1);
    return true;
  }

  // JUMP TO PREVIOUS LETTER
  if (event.R1Pressed())
  {
      jumpToNextLetter(mSystem->getSortId() == 1 ? -1 : 1);
      return true;
  }

  // JUMP TO -10
  if (event.L2Pressed())
  {
    auto index = getCursorIndex();
    if (index > 0)
      setCursorIndex(index > 10 ? index - 10 : 0);
    else
      setCursorIndex(getCursorIndexMax());
    return true;
  }

  // JUMP TO +10
  if (event.R2Pressed())
  {
    auto index = getCursorIndex();
    auto max = getCursorIndexMax();
    if (index == max)
      setCursorIndex(0);
    else
      setCursorIndex(index > max - 10 ? max : index + 10);
    return true;
  }

  // NETPLAY
  if ((event.XPressed()) && (RecalboxConf::Instance().AsBool("global.netplay"))
      && (RecalboxConf::Instance().isInList("global.netplay.systems", getCursor()->getSystem()->getName())))
  {
    FileData* cursor = getCursor();
    if(cursor->isGame())
    {
      Vector3f target(Renderer::getDisplayWidthAsFloat() / 2.0f, Renderer::getDisplayHeightAsFloat() / 2.0f, 0);
      ViewController::Instance().launch(cursor, target, "host");
    }
  }

  if (event.StartPressed())
  {
    mWindow.pushGui(new GuiGamelistOptions(mWindow, getRoot()->getSystem()));
    return true;
  }

  if (event.SelectPressed() && !mIsFavoriteSystem)
  {
    if (mFavoritesCount != 0) {
      mFavoritesOnly = !mFavoritesOnly;
      Settings::Instance().SetFavoritesOnly(mFavoritesOnly);
      refreshList();
      updateInfoPanel();
      updateHelpPrompts();
    }
    return true;
  }

  bool result = IGameListView::ProcessInput(event);

  if (event.AnythingPressed())
      RecalboxSystem::NotifyGame(*getCursor(), false, false);

  return result;
}

bool ISimpleGameListView::getHelpPrompts(Help& help)
{
  bool hideSystemView = RecalboxConf::Instance().AsBool("emulationstation.hidesystemview");

  help.Set(HelpType::B, _("LAUNCH"));

  if (RecalboxConf::Instance().AsBool("global.netplay") && (RecalboxConf::Instance().isInList("global.netplay.systems", getCursor()->getSystem()->getName())))
    help.Set(HelpType::X, _("NETPLAY"));

  help.Set(HelpType::Y, mIsFavoriteSystem ? _("Remove from favorite") : _("Favorite"));

  if (!hideSystemView)
    help.Set(HelpType::A, _("BACK"));

  help.Set(HelpType::UpDown, _("CHOOSE"));

  if (Settings::Instance().QuickSystemSelect() && !hideSystemView)
    help.Set(HelpType::LeftRight, _("SYSTEM"));

  if (!mIsFavoriteSystem)
  {
    help.Set(HelpType::Start, _("OPTIONS"));
    if (mFavoritesCount != 0)
      help.Set(HelpType::Select, mFavoritesOnly ? _("ALL GAMES") : _("FAVORITES ONLY"));
  }

  return true;
}

std::vector<std::string> ISimpleGameListView::getAvailableLetters()
{
  // TODO: Algorithm!!! Use 128 array bitflag - Use better returning type - kill all vectors
  // TODO: Be consistent! Use gamelist items, not FileData
  std::vector<std::string> letters;
  std::set<std::string> setOfLetters;
  FileData::List files = getFileDataList();
  for (auto file : files) {
    if (file->getType() == ItemType::Game)
    {
      setOfLetters.insert(std::string(1, toupper(file->getName()[0])));
    }
  }

  for (const auto& letter : setOfLetters)
  {
    letters.push_back(letter);
  }

  std::sort(letters.begin(), letters.end());
  return letters;
}


void ISimpleGameListView::jumpToNextLetter(int increment)
{
  std::string letter = std::string(1, toupper(getCursor()->getName()[0]));
  auto letters = getAvailableLetters();
  unsigned long size = letters.size();
  unsigned long pos = std::find(letters.begin(), letters.end(), letter) - letters.begin();

  if (pos < size) {
    pos = (pos + increment + size) % size;
    jumpToLetter(letters.at(pos)[0]);
  }
}

void ISimpleGameListView::jumpToLetter(char letter)
{
  // Jump to letter requires an alpha sort
  if ( mSystem->getSortId() > 1) {
    // apply sort
    mSystem->setSortId(0);
    // notify that the root folder has to be sorted
    onFileChanged(mSystem->getRootFolder(), FileChangeType::Sorted);
  }

  FileData::List files = getFileDataList();

  unsigned long min, max;
  unsigned long mid = 0;

  bool asc = mSystem->getSortId() == 0;

  // look for first game position
  for (min = 0; (min < files.size() - 1) && (files.at(min)->getType() != ItemType::Game) ; min++) ;

  // look for last game position
  for (max = files.size() - 1; (max != 0u) && (files.at(max)->getType() != ItemType::Game) ; max--) ;

  while(max >= min) {
    mid = ((max - min) / 2) + min;

    // game somehow has no first character to check
    if (files.at(mid)->getName().empty()) {
      continue;
    }

    char checkLetter = (char) toupper(files.at(mid)->getName()[0]);

    if (asc) {
      if (checkLetter < letter) {
        min = mid + 1;
      } else if (checkLetter > letter || (mid > 0 && (letter == toupper(files.at(mid - 1)->getName()[0])))) {
        max = mid - 1;
      } else {
        break; //exact match found
      }
    } else {
      if (checkLetter > letter) {
        min = mid + 1;
      } else if (checkLetter < letter || (mid > 0 && (letter == toupper(files.at(mid - 1)->getName()[0])))) {
        max = mid - 1;
      } else {
        break; //exact match found
      }
    }
  }

  setCursor(files.at(mid));
}