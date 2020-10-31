#include <RecalboxConf.h>
#include <systems/SystemManager.h>
#include <guis/GuiControlHints.h>
#include <guis/GuiNetPlayHostPasswords.h>
#include "guis/GuiGamelistOptions.h"
#include "views/gamelist/ISimpleGameListView.h"
#include "systems/SystemData.h"
#include "Window.h"
#include "views/ViewController.h"
#include "Settings.h"
#include "utils/locale/LocaleHelper.h"

ISimpleGameListView::ISimpleGameListView(Window& window, SystemManager& systemManager, SystemData& system)
  : IGameListView(window, system),
    mSystemManager(systemManager),
    mHeaderText(window),
    mHeaderImage(window),
    mBackground(window),
    mThemeExtras(window)
{
  int favoritesCount = system.FavoritesCount();
  mFavoritesOnly = favoritesCount > 0 && Settings::Instance().FavoritesOnly();

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

  addChild(&mHeaderText);
  addChild(&mBackground);
}

void ISimpleGameListView::onThemeChanged(const ThemeData& theme)
{
  mBackground.applyTheme(theme, getName(), "background", ThemeProperties::All);
  mHeaderImage.applyTheme(theme, getName(), "logo", ThemeProperties::All);
  mHeaderText.applyTheme(theme, getName(), "logoText", ThemeProperties::All);

  // Remove old theme extras
  for (auto* extra : mThemeExtras.getmExtras()) {
    removeChild(extra);
  }
  mThemeExtras.getmExtras().clear();

  mThemeExtras.setExtras(ThemeData::makeExtras(theme, getName(), mWindow));
  mThemeExtras.sortExtrasByZIndex();

  // Add new theme extras

  for (auto* extra : mThemeExtras.getmExtras()) {
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
    bool isInFavorite = favoriteSystem->getRootFolder().Contains(file, true);
    bool isFavorite = file->Metadata().Favorite();

    if (isInFavorite != isFavorite)
    {
      if (isInFavorite) favoriteSystem->getRootFolder().removeChild(file);
      else favoriteSystem->getRootFolder().addChild(file, false);
      ViewController::Instance().setInvalidGamesList(mSystemManager.FavoriteSystem());
      ViewController::Instance().getSystemListView().manageFavorite();
      if (mSystem.FavoritesCount() == 0) mFavoritesOnly = false;
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
  if (RecalboxConf::Instance().AsBool(mSystem.getName() + ".flatfolder"))
  {
    populateList(mSystem.getRootFolder());
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
        populateList(*folder);
        setCursorIndex(0);
      }
    }
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

      FolderData& cursor = !mCursorStack.empty() ? *mCursorStack.top() : mSystem.getRootFolder();
      populateList(cursor);

      setCursor(selected);
      //Sound::getFromTheme(getTheme(), getName(), "back")->play();
    }
    else if (!hideSystemView)
    {
      onFocusLost();
      ViewController::Instance().goToSystemView(&mSystem);
    }
    return true;
  }

  // TOGGLE FAVORITES
  if (event.YPressed())
  {
    FileData* cursor = getCursor();
    if (cursor->isGame() && cursor->getSystem()->getHasFavoritesInTheme())
    {
      MetadataDescriptor& md = cursor->Metadata();
      SystemData *favoriteSystem = mSystemManager.FavoriteSystem();

      md.SetFavorite(!md.Favorite());

      if (favoriteSystem != nullptr)
      {
        if (md.Favorite()) favoriteSystem->getRootFolder().addChild(cursor, false);
        else               favoriteSystem->getRootFolder().removeChild(cursor);

        ViewController::Instance().setInvalidGamesList(cursor->getSystem());
        ViewController::Instance().setInvalidGamesList(favoriteSystem);
      }
      ViewController::Instance().getSystemListView().manageFavorite();

      // Reset favorite-only view if no more favorites
      if (mSystem.FavoritesCount() == 0) mFavoritesOnly = false;

      // Reload to refresh the favorite icon
      int cursorPlace = getCursorIndex();
      refreshList();
      setCursorIndex(cursorPlace);

      updateHelpPrompts();
    }
    return true;
  }

  // MOVE to NEXT GAMELIST
  if (event.AnyRightPressed())
  {
    if (RecalboxConf::Instance().GetQuickSystemSelect() && !hideSystemView) {
      onFocusLost();
      ViewController::Instance().goToNextGameList();
      return true;
    }
  }

  // MOVE to PREVIOUS GAMELIST
  if (event.AnyLeftPressed())
  {
    if (RecalboxConf::Instance().GetQuickSystemSelect() && !hideSystemView) {
      onFocusLost();
      ViewController::Instance().goToPrevGameList();
      return true;
    }
  }


  // JUMP TO NEXT LETTER
  if (event.L1Pressed())
  {
    jumpToNextLetter(mSystem.getSortId() == 1);
    return true;
  }

  // JUMP TO PREVIOUS LETTER
  if (event.R1Pressed())
  {
    jumpToNextLetter(mSystem.getSortId() == 0);
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
  if ((event.XPressed()) && (RecalboxConf::Instance().AsBool("global.netplay.active"))
      && (RecalboxConf::Instance().isInList("global.netplay.systems", getCursor()->getSystem()->getName())))
  {
    FileData* cursor = getCursor();
    mWindow.pushGui(new GuiNetPlayHostPasswords(mWindow, *cursor));
    return true;
  }
  else if (event.XPressed())
  {
    FileData* fd = getCursor();
    if (fd != nullptr)
      if (fd->HasP2K())
        mWindow.pushGui(new GuiControlHints(mWindow, fd->getPath()));
    return true;
  }

  if (event.StartPressed())
  {
    mWindow.pushGui(new GuiGamelistOptions(mWindow, mSystem, mSystemManager));
    return true;
  }

  if (event.SelectPressed() && !IsFavoriteSystem())
  {
    if (mSystem.FavoritesCount() != 0)
    {
      mFavoritesOnly = !mFavoritesOnly;
      Settings::Instance().SetFavoritesOnly(mFavoritesOnly);
      refreshList();
      updateInfoPanel();
      updateHelpPrompts();
    }
    return true;
  }

  bool result = IGameListView::ProcessInput(event);

  return result;
}

bool ISimpleGameListView::getHelpPrompts(Help& help)
{
  bool hideSystemView = RecalboxConf::Instance().AsBool("emulationstation.hidesystemview");

  help.Set(HelpType::B, _("LAUNCH"));

  bool netplay = RecalboxConf::Instance().AsBool("global.netplay.active");
  if (netplay && (RecalboxConf::Instance().isInList("global.netplay.systems", getCursor()->getSystem()->getName())))
    help.Set(HelpType::X, _("NETPLAY"));
  else
  {
    FileData* fd = getCursor();
    if (fd != nullptr)
      if (fd->HasP2K())
        help.Set(HelpType::X, _("P2K CONTROLS"));
  }

  help.Set(HelpType::Y, IsFavoriteSystem() ? _("Remove from favorite") : _("Favorite"));

  if (!hideSystemView)
    help.Set(HelpType::A, _("BACK"));

  help.Set(HelpType::UpDown, _("CHOOSE"));

  if (RecalboxConf::Instance().GetQuickSystemSelect() && !hideSystemView)
    help.Set(HelpType::LeftRight, _("SYSTEM"));

  help.Set(HelpType::Start, _("OPTIONS"));
  if (!IsFavoriteSystem())
  {
    if (mSystem.FavoritesCount() != 0)
      help.Set(HelpType::Select, mFavoritesOnly ? _("ALL GAMES") : _("FAVORITES ONLY"));
  }

  return true;
}

std::vector<unsigned int> ISimpleGameListView::getAvailableLetters()
{
  constexpr int UnicodeSize = 0x10000;
  FileData::List files = getFileDataList(); // All file
  std::vector<unsigned int> unicode;        // 1 bit per unicode char used
  unicode.resize(UnicodeSize / (sizeof(unsigned int) * 8), 0);

  for (auto* file : files)
    if (file->isGame())
    {
      // Tag every first characters from every game name
      unsigned int wc = Strings::UpperChar(file->getName());
      if (wc < UnicodeSize) // Ignore extended unicodes
        unicode[wc >> 5] |= 1 << (wc & 0x1F);
    }

  // Rebuild a self-sorted unicode list with all tagged characters
  int unicodeOffset = 0;
  std::vector<unsigned int> result;
  for(unsigned int i : unicode)
  {
    if (i != 0)
      for (int bit = 0; bit < 32; ++bit)
        if (((i >> bit) & 1) != 0)
          result.push_back(unicodeOffset + bit);
    unicodeOffset += 32;
  }

  return result;
}


void ISimpleGameListView::jumpToNextLetter(bool forward)
{
  // Get current unicode
  unsigned int currentUnicode = getCursor()->isGame() ?(unsigned int)Strings::UpperChar(getCursor()->getName()) : 0;

  // Get available unicodes
  std::vector<unsigned int> availableUnicodes = getAvailableLetters();
  if (availableUnicodes.empty()) return;

  // Lookup current unicode
  int position = 0;
  if (currentUnicode == 0) // Folder
    position = forward ? -1 : 0;
  else
    for(int i = (int)availableUnicodes.size(); --i >= 0;)
      if (availableUnicodes[i] == currentUnicode)
      {
        position = i;
        break;
      }

  int size = (int)availableUnicodes.size();
  unsigned int nextUnicode = availableUnicodes[(size + position + (forward ? 1 : -1)) % size];
  jumpToLetter(nextUnicode);
}

void ISimpleGameListView::jumpToLetter(unsigned int unicode)
{
  // Jump to letter requires an alpha sort
  if ( mSystem.getSortId() > 1)
  {
    // apply sort
    mSystem.setSortId(0);
    // notify that the root folder has to be sorted
    onFileChanged(&mSystem.getRootFolder(), FileChangeType::Sorted);
  }

  FileData::List files = getFileDataList();
  for(int c = (int)files.size(), i = 0; --c >= 0; ++i)
    if (files[i]->isGame())
      if (Strings::UpperChar(files[i]->getName()) == unicode)
      {
        setCursor(files[i]);
        break;
      }
}