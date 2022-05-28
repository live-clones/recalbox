#include <RecalboxConf.h>
#include <systems/SystemManager.h>
#include <guis/GuiControlHints.h>
#include <guis/GuiNetPlayHostPasswords.h>
#include "guis/menus/GuiMenuGamelistOptions.h"
#include "views/gamelist/ISimpleGameListView.h"
#include "systems/SystemData.h"
#include "WindowManager.h"
#include "views/ViewController.h"
#include "utils/locale/LocaleHelper.h"

ISimpleGameListView::ISimpleGameListView(WindowManager& window, SystemManager& systemManager, SystemData& system)
  : IGameListView(window, system),
    mSystemManager(systemManager),
    mHeaderText(window),
    mHeaderImage(window),
    mBackground(window),
    mThemeExtras(window),
    mVerticalMove(false)
{
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

void ISimpleGameListView::onChanged(Change change)
{
  (void)change;

  // Store cursor
  int cursor = getCursorIndex();

  // Refresh list
  if (RecalboxConf::Instance().AsBool(mSystem.Name() + ".flatfolder")) populateList(mSystem.MasterRoot());
  else refreshList();

  // Restore cursor
  setCursorIndex(cursor);
  // And refresh game info
  updateInfoPanel();
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

  if (file->IsGame())
  {
    SystemData* favoriteSystem = mSystemManager.FavoriteSystem();
    bool isInFavorite = favoriteSystem->GetFavoriteRoot().Contains(file, true);
    bool isFavorite = file->Metadata().Favorite();

    if (isInFavorite != isFavorite)
    {
      if (isInFavorite) favoriteSystem->GetFavoriteRoot().RemoveChild(file);
      else favoriteSystem->GetFavoriteRoot().AddChild(file, false);
      ViewController::Instance().setInvalidGamesList(mSystemManager.FavoriteSystem());
      ViewController::Instance().getSystemListView().manageFavorite();
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
  if (RecalboxConf::Instance().AsBool(mSystem.Name() + ".flatfolder")) populateList(mSystem.MasterRoot());
  else refreshList();

  setCursorIndex(cursor);
  updateInfoPanel();
}

bool ISimpleGameListView::ProcessInput(const InputCompactEvent& event) {
  bool hideSystemView = RecalboxConf::Instance().GetStartupHideSystemView();
  FileData* cursor = getCursor();

  // RUN GAME or ENTER FOLDER
  if (event.ValidPressed())
  {
    clean();
    if (cursor->IsGame())
    {
      //Sound::getFromTheme(getTheme(), getName(), "launch")->play();
      launch(cursor);
    }
    else if (cursor->IsFolder())
    {
      FolderData* folder = (FolderData*)cursor;
      if (folder->HasChildren())
      {
        mCursorStack.push(folder);
        populateList(*folder);
        setCursorIndex(0);
      }
    }
    return true;
  }

  // BACK to PARENT FOLDER or PARENT SYSTEM
  if (event.CancelPressed())
  {
    clean();
    if (!mCursorStack.empty())
    {
      FolderData* selected = mCursorStack.top();

      // remove current folder from stack
      mCursorStack.pop();

      FolderData& cursor = !mCursorStack.empty() ? *mCursorStack.top() : mSystem.MasterRoot();
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
  if (event.YPressed() && !cursor->IsPreinstalled())
  {

    if (cursor->IsGame() && cursor->System().HasFavoritesInTheme())
    {
      MetadataDescriptor& md = cursor->Metadata();
      SystemData *favoriteSystem = mSystemManager.FavoriteSystem();

      md.SetFavorite(!md.Favorite());

      if (favoriteSystem != nullptr)
      {
        if (md.Favorite()) favoriteSystem->GetFavoriteRoot().AddChild(cursor, false);
        else favoriteSystem->GetFavoriteRoot().RemoveChild(cursor);

        ViewController::Instance().setInvalidGamesList(&cursor->System());
        ViewController::Instance().setInvalidGamesList(favoriteSystem);
      }
      ViewController::Instance().getSystemListView().manageFavorite();

      int popupDuration = RecalboxConf::Instance().GetPopupHelp();
      std::string message = md.Favorite() ? _("Added to favorites") : _("Removed from favorites");
      mWindow.InfoPopupAdd(new GuiInfoPopup(mWindow, message + ":\n" + cursor->DisplayName(), popupDuration,
                                            GuiInfoPopup::PopupType::None));

      // Reload to refresh the favorite icon
      int cursorPlace = getCursorIndex();
      refreshList();
      setCursorIndex(cursorPlace);

      updateHelpPrompts();
    }
    return true;
  }

  // Check vertical move
  if (event.AnyUpPressed() || event.AnyDownPressed()) mVerticalMove = true;
  if (event.AnyUpReleased() || event.AnyDownReleased()) mVerticalMove = false;

  // MOVE to NEXT GAMELIST
  if (event.AnyRightPressed())
  {
    if (RecalboxConf::Instance().GetQuickSystemSelect() && !hideSystemView && !mVerticalMove) {
      clean();
      onFocusLost();
      ViewController::Instance().goToNextGameList();
    }
    return true;
  }

  // MOVE to PREVIOUS GAMELIST
  if (event.AnyLeftPressed())
  {
    if (RecalboxConf::Instance().GetQuickSystemSelect() && !hideSystemView && !mVerticalMove) {
      clean();
      onFocusLost();
      ViewController::Instance().goToPrevGameList();
    }
    return true;
  }


  // JUMP TO NEXT LETTER
  if (event.L1Pressed())
  {
    jumpToNextLetter(!FileSorts::IsAscending((FileSorts::Sorts)RecalboxConf::Instance().GetSystemSort(mSystem)));
    return true;
  }

  // JUMP TO PREVIOUS LETTER
  if (event.R1Pressed())
  {
    jumpToNextLetter(FileSorts::IsAscending((FileSorts::Sorts)RecalboxConf::Instance().GetSystemSort(mSystem)));
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
  if (event.XPressed() && RecalboxConf::Instance().GetNetplayEnabled()
      && (getCursor()->System().Descriptor().HasNetPlayCores()))
  {
    clean();
    FileData* cursor = getCursor();
    mWindow.pushGui(new GuiNetPlayHostPasswords(mWindow, *cursor));
    return true;
  }
  else if (event.XPressed())
  {
    FileData* fd = getCursor();
    if (fd != nullptr)
      if (fd->HasP2K())
        mWindow.pushGui(new GuiControlHints(mWindow, fd->FilePath()));
    return true;
  }

  if (event.StartPressed())
  {
    clean();
    mWindow.pushGui(new GuiMenuGamelistOptions(mWindow, mSystem, mSystemManager));
    return true;
  }

  bool result = IGameListView::ProcessInput(event);

  return result;
}

bool ISimpleGameListView::getHelpPrompts(Help& help)
{
  bool hideSystemView = RecalboxConf::Instance().GetStartupHideSystemView();

  help.Set(Help::Valid(), _("LAUNCH"));

  bool netplay = RecalboxConf::Instance().GetNetplayEnabled();
  if (netplay && getCursor()->System().Descriptor().HasNetPlayCores())
    help.Set(HelpType::X, _("NETPLAY"));
  else
  {
    FileData* fd = getCursor();
    if (fd != nullptr)
      if (fd->HasP2K())
        help.Set(HelpType::X, _("P2K CONTROLS"));
  }
  FileData* fd = getCursor();
  if (!fd->IsPreinstalled())
    help.Set(HelpType::Y, IsFavoriteSystem() ? _("Remove from favorite") : _("Favorite"));

  if (!hideSystemView)
    help.Set(Help::Cancel(), _("BACK"));

  help.Set(HelpType::UpDown, _("CHOOSE"));

  if (RecalboxConf::Instance().GetQuickSystemSelect() && !hideSystemView)
    help.Set(HelpType::LeftRight, _("SYSTEM"));

  help.Set(HelpType::Start, _("OPTIONS"));

  return true;
}

std::vector<unsigned int> ISimpleGameListView::getAvailableLetters()
{
  constexpr int UnicodeSize = 0x10000;
  FileData::List files = getFileDataList(); // All file
  std::vector<unsigned int> unicode;        // 1 bit per unicode char used
  unicode.resize(UnicodeSize / (sizeof(unsigned int) * 8), 0);

  for (auto* file : files)
    if (file->IsGame())
    {
      // Tag every first characters from every game name
      unsigned int wc = Strings::UpperChar(file->Name());
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
  unsigned int currentUnicode = getCursor()->IsGame() ?(unsigned int)Strings::UpperChar(getCursor()->Name()) : 0;

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
  FileSorts::Sorts sort = RecalboxConf::Instance().GetSystemSort(mSystem);
  if (sort != FileSorts::Sorts::FileNameAscending && sort != FileSorts::Sorts::FileNameDescending)
  {
    // apply sort
    RecalboxConf::Instance().SetSystemSort(mSystem, FileSorts::Sorts::FileNameAscending);
    // notify that the root folder has to be sorted
    onChanged(Change::Resort);
  }

  FileData::List files = getFileDataList();
  for(int c = (int)files.size(), i = 0; --c >= 0; ++i)
    if (files[i]->IsGame())
      if (Strings::UpperChar(files[i]->Name()) == unicode)
      {
        setCursor(files[i]);
        break;
      }
}