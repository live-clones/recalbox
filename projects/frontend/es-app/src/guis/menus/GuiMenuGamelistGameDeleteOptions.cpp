#include <games/GameFilesUtils.h>
#include "GuiMenuGamelistGameDeleteOptions.h"
#include "GuiMenuTools.h"
#include "games/FileData.h"
#include <views/ViewController.h>
#include "components/ButtonComponent.h"
#include "GuiMenuGamelistGameDeleteSelectionOptions.h"
#include "utils/locale/LocaleHelper.h"

GuiMenuGamelistGameDeleteOptions::GuiMenuGamelistGameDeleteOptions(WindowManager& window, IGameListView& view, FileData& game)
  : GuiMenuBase(window, _("DELETE ALL FILES"), this)
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
    mGameFiles = GameFilesUtils::GetGameSubFiles(mGame);
    std::string fileCount = Strings::Replace(_N("%i file", "%i files", (int) mGameFiles.size() + 1), "%i",
                                   Strings::ToString((int) mGameFiles.size() + 1));
    AddText(_("GAME FILES (ROM | DISK IMAGE)"), fileCount);

    mMediaFiles = GameFilesUtils::GetMediaFiles(mGame);
    if(!mMediaFiles.empty()) {
        fileCount = Strings::Replace(_N("%i file", "%i files", (int) mMediaFiles.size()), "%i",
                                     Strings::ToString((int) mMediaFiles.size()));
        AddText(_("MEDIA FILES"), fileCount);
    }

    mExtraFiles = GameFilesUtils::GetGameExtraFiles(mGame);
    if(!mExtraFiles.empty()) {
        fileCount = Strings::Replace(_N("%i file", "%i files", (int) GameFilesUtils::GetGameExtraFiles(mGame).size()), "%i",
                                     Strings::ToString((int) GameFilesUtils::GetGameExtraFiles(mGame).size()));
        AddText(_("CONFIGURATION AND PATCH FILES"), fileCount);
    }

    mSaveFiles = GameFilesUtils::GetGameSaveFiles(mGame);
    if(!mSaveFiles.empty()) {
        fileCount = Strings::Replace(_N("%i file", "%i files", (int) mSaveFiles.size()), "%i",
                                     Strings::ToString((int) mSaveFiles.size()));
        AddText(_("SAVE FILES"), fileCount);
    }

    AddSubMenu(_("ADVANCED DELETE"), mTheme.menuIconSet.advanced, (int)Components::Advanced, "");

    mMenu.addButton(_("OK"), "", [this]
    {
      mWindow.pushGui((new GuiMsgBoxScroll(mWindow, _("DELETE ALL FILES, CONFIRM?"), ComputeMessage(), _("YES"), [this]
      {
        DeleteAllFiles();
      }, _("NO"), {}, "", nullptr, TextAlignment::Left))->SetDefaultButton(1));
    });

    mMenu.addButton(_("CANCEL"), "", [this] { Close(); });
    mMenu.setCursorToButtons();
    mMenu.SetDefaultButton(0);
  }
}

void GuiMenuGamelistGameDeleteOptions::DeleteAllFiles()
{
  GameFilesUtils::DeleteAllFiles(mGame);
  mView.removeEntry(&mGame);
  mWindow.deleteAllGui();
}

bool GuiMenuGamelistGameDeleteOptions::ProcessInput(const InputCompactEvent& event)
{

    return GuiMenuBase::ProcessInput(event);
}

void GuiMenuGamelistGameDeleteOptions::SubMenuSelected(int id)
{
  switch((Components)id)
  {
    case Components::Advanced:
      mWindow.pushGui(new GuiMenuGamelistGameDeleteSelectionOptions(mWindow, mView, mGame));
    case Components::Delete:
      break;
  }
}

std::string GuiMenuGamelistGameDeleteOptions::ComputeMessage()
{
  std::string message = _("Game").append(": ").append(mGame.Name()).append("\n");
  message.append(_("You are about to delete this files, confirm ?"));
  message.append("\n\n");
  message.append(mGame.FilePath().Filename()).append("\n");
  for(const auto& path : mGameFiles)
  {
    message.append(Path(path).Filename()).append("\n");
  }
  if(!mExtraFiles.empty())
  {
    for (const auto& path: mExtraFiles)
    {
      message.append(Path(path).Filename()).append("\n");
    }
  }

  if(!mSaveFiles.empty())
  {
    for (const auto& path: mSaveFiles)
    {
      message.append(Path(path).Filename()).append("\n");
    }
  }

  if(!mMediaFiles.empty())
  {
    for (const auto& path: mMediaFiles)
    {
      if(Strings::Contains(path, "/media/images/"))
        message.append(_("Image")).append("\n");
      if(Strings::Contains(path, "/media/thumbnails/"))
        message.append(_("Thumbnail")).append("\n");
      if(Strings::Contains(path, "/media/videos/"))
        message.append(_("Video")).append("\n");
    }
  }
  return message;
}