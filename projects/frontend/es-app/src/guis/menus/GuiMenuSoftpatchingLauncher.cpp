#include <games/GameFilesUtils.h>
#include "GuiMenuSoftpatchingLauncher.h"
#include "GuiMenuTools.h"
#include "games/FileData.h"
#include <views/ViewController.h>
#include "components/ButtonComponent.h"
#include "utils/locale/LocaleHelper.h"

GuiMenuSoftpatchingLauncher::GuiMenuSoftpatchingLauncher(WindowManager& window,
                                                         FileData& game,
                                                         std::list<Path>& patches,
                                                         int lastChoice,
                                                         const std::function<void()>& func1,
                                                         const std::function<void(const Path&)>& func2)
  : GuiMenuBase(window, _("SOFTPATCHING"), nullptr)
  , mGame(game)
  , mPatches(patches)
{
    // Footer
    std::string gameName(game.Name());
    gameName.append(" (").append(game.RomPath().Filename()).append(1, ')');
    SetFooter(Strings::Replace(_("GAME %s"), "%s", Strings::ToUpperUTF8(gameName)));

    // select
    mPaths = AddList<Path>(_("select a patch"),(int) Components::Patch, this,GetPatchesEntries(), "");

    mMenu.addButton(_("original"), "", func1);

    mMenu.addButton(_("patched"),
                    "",
                    [this, func2] { func2(mPaths->getSelected()); }
                    );

    mMenu.setCursorToButtons();
    mMenu.SetDefaultButton(lastChoice);
    GuiMenuBase::SetFooter(game.Name());
}

std::vector<GuiMenuBase::ListEntry<Path>> GuiMenuSoftpatchingLauncher::GetPatchesEntries()
{
  std::vector<ListEntry<Path>> list;
  int nb = 1;
  unsigned long patchListSize = mPatches.size();
  for(auto& path : mPatches)
  {
    bool isDefault = patchListSize == 1 || (path == mGame.Metadata().LastPatch() && mGame.Metadata().LastPatch().Exists()) || nb == 1;
   std::string patchName = path.Directory() == mGame.RomPath().Directory() ? path.Filename() + " (auto)" : path.Filename();

    list.push_back({ patchName, path , isDefault });
    nb++;
  }
  return list;
}

void GuiMenuSoftpatchingLauncher::OptionListComponentChanged(int id, int index, const Path& value)
{
  (void)index;
  if ((Components)id == Components::Patch)
    mGame.Metadata().SetLastPatch(value);
}

















