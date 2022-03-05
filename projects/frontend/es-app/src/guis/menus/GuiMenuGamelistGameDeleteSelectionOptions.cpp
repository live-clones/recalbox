#include <games/GameFilesUtils.h>
#include "GuiMenuGamelistGameDeleteSelectionOptions.h"
#include "GuiMenuTools.h"
#include "games/FileData.h"
#include <views/ViewController.h>
#include <guis/GuiMsgBox.h>
#include "components/ButtonComponent.h"
#include "utils/locale/LocaleHelper.h"

GuiMenuGamelistGameDeleteSelectionOptions::GuiMenuGamelistGameDeleteSelectionOptions(WindowManager& window, IGameListView& view, FileData& game)
  : GuiMenuBase(window, _("SELECT FILES TO DELETE"), nullptr)
  , mView(view)
  , mGame(game)
{
  if (mGame.IsGame())
  {
    // Footer
    std::string gameName(game.Name());
    gameName.append(" (").append(game.FilePath().Filename()).append(1, ')');
    SetFooter(Strings::Replace(_("GAME %s"), "%s", Strings::ToUpperUTF8(gameName)));

    // Delete
    mGameFiles = AddMultiList<Path>(_("GAME FILES (ROM | DISK IMAGE)"), (int) Components::Delete, nullptr, GetGameFileEntries());
    mMedias = AddMultiList<Path>(_("MEDIA"), (int) Components::Delete, nullptr, GetMediaEntries());
    mExtras = AddMultiList<Path>(_("CONF | PATCHES"), (int) Components::Delete, nullptr, GetExtraEntries());
    mSaves = AddMultiList<Path>(_("SAVES"), (int) Components::Delete, nullptr, GetSaveEntries());

    mMenu.addButton(_("OK"), "", [this]
    {
      if(mGameFiles->getSelectedObjects().empty() && mSaves->getSelectedObjects().empty() && mExtras->getSelectedObjects().empty() &&
      mMedias->getSelectedObjects().empty())
      {
        mWindow.pushGui(
          (new GuiMsgBox(mWindow, _("No file selected,\nyou must at least choose one."), _("BACK"), TextAlignment::Left)));
        mMenu.SetDefaultButton(1);

      }
      else
          mWindow.pushGui((new GuiMsgBoxScroll(mWindow, _("DELETE SELECTED FILES, CONFIRM?"), ComputeMessage(), _("YES"), [this]
      {
        DeleteSelectedFiles();
      }, _("BACK"), {}, "", nullptr, TextAlignment::Left))->SetDefaultButton(1));
    });

    mMenu.addButton(_("BACK"), "", [this] { Close(); });
    mMenu.setCursorToButtons();
    mMenu.SetDefaultButton(1);
  }
}

std::vector<GuiMenuBase::ListEntry<Path>> GuiMenuGamelistGameDeleteSelectionOptions::GetGameFileEntries()
{
  std::vector<ListEntry<Path>> list;
  list.push_back({ mGame.FilePath().Filename(),  mGame.FilePath(), true });

  for (const auto& file : GameFilesUtils::GetGameSubFiles(mGame))
  {
    Path path = Path(file);
    list.push_back({ path.Filename(), path, true });
  }
  return list;
}

std::vector<GuiMenuBase::ListEntry<Path>> GuiMenuGamelistGameDeleteSelectionOptions::GetMediaEntries()
{
  std::vector<ListEntry<Path>> list;
   if (mGame.Metadata().Image().Exists())
    {
        list.push_back({ "image", mGame.Metadata().Image(), true });
    }
    if (mGame.Metadata().Thumbnail().Exists())
    {
        list.push_back({ "thumbnail", mGame.Metadata().Thumbnail(), true });
    }
    if (mGame.Metadata().Video().Exists())
    {
        list.push_back({ "video", mGame.Metadata().Video(), true });
    }

  return list;
}

std::vector<GuiMenuBase::ListEntry<Path>> GuiMenuGamelistGameDeleteSelectionOptions::GetExtraEntries()
{
  std::vector<ListEntry<Path>> list;
  for (const auto& patch : GameFilesUtils::GetGameExtraFiles(mGame))
  {
    Path path = Path(patch);
      list.push_back({ path.Filename(), path, true });
  }
  return list;
}

std::vector<GuiMenuBase::ListEntry<Path>> GuiMenuGamelistGameDeleteSelectionOptions::GetSaveEntries()
{
  std::vector<ListEntry<Path>> list;
  std::vector<ListEntry<Path>> mediaList;
  for (const auto& patch : GameFilesUtils::GetGameSaveFiles(mGame))
  {
    Path path = Path(patch);
    list.push_back({ path.Filename(), path, true });
  }
  return list;
}

void GuiMenuGamelistGameDeleteSelectionOptions::DeleteSelectedFiles()
{
  HashSet<std::string> list;
  HashSet<std::string> mediaList;

  bool mainGameDeleted = false;

  for(const Path& path : mGameFiles->getSelectedObjects())
  {
    if(path == mGame.FilePath())
    {
      mainGameDeleted = true;
    }
    list.insert(path.ToString());
  }

  for(const Path& path : mExtras->getSelectedObjects())
  {
    list.insert(path.ToString());
  }
  for(const Path& path : mSaves->getSelectedObjects())
  {
    list.insert(path.ToString());
  }

  for(const Path& path : mMedias->getSelectedObjects())
  {
    mediaList.insert(path.ToString());
  }

  GameFilesUtils::DeleteSelectedFiles(mGame, list, mediaList);
  if(mainGameDeleted)
  {
    mView.removeEntry(&mGame);
  }
  mWindow.deleteAllGui();
}

std::string GuiMenuGamelistGameDeleteSelectionOptions::ComputeMessage()
{
  std::string message = _("GAME FILES (ROM | DISK IMAGE)").append("\n");
  for(const Path& path : mGameFiles->getSelectedObjects())
  {
    message.append(path.ToString()).append("\n");
  }
  if(!mExtras->getSelectedObjects().empty())
  {
    message.append("\n").append(_("CONF | PATCHES")).append("\n");
    for (const Path& path: mExtras->getSelectedObjects())
    {
      message.append(path.ToString()).append("\n");
    }
  }

  if(!mSaves->getSelectedObjects().empty())
  {
    message.append("\n").append(_("SAVES")).append("\n");
    for (const Path& path: mSaves->getSelectedObjects())
    {
      message.append(path.ToString()).append("\n");
    }
  }

  if(!mMedias->getSelectedObjects().empty())
  {
    message.append("\n").append(_("MEDIA")).append("\n");
    for (const Path& path: mMedias->getSelectedObjects())
    {
      message.append(path.ToString()).append("\n");
    }
  }

  return message;
}

