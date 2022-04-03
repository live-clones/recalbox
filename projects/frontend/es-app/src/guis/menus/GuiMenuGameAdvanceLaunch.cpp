//
// Created by gugue_u on 02/04/2022.
//

#include "GuiMenuGameAdvanceLaunch.h"
#include "utils/cplusplus/StaticLifeCycleControler.h"
#include "views/ViewController.h"


GuiMenuGameAdvanceLaunch::GuiMenuGameAdvanceLaunch(WindowManager& window, SystemData& system)
  : GuiMenuBase(window, _("GAMES ADVANDE LAUNCN"), nullptr)
  , mSystem(system)
  , mGamelist(*ViewController::Instance().getGameListView(&system))
  ,mFileData(*ViewController::Instance().getGameListView(&system)->getCursor())
{

  // Patch
  mPatch = AddList<Path>(_("PATCH"), (int)Components::Patch, nullptr, GetPatchEntries(), Strings::Empty);

  mMenu.addButton(_("START"), "", [this]{AdvancedLaunch();});

}

std::vector<GuiMenuBase::ListEntry<Path>> GuiMenuGameAdvanceLaunch::GetPatchEntries()
{

  std::vector<ListEntry<Path>> list;
  list.emplace_back( "none", Path(""), true );
  for (const auto& path : mFileData.FilePath().Directory().GetDirectoryContent())
  {
    std::string extension = Strings::ToLowerUTF8(path.Extension());

    if ((Strings::Contains(extension, ".ups") || Strings::Contains(extension, ".bps") ||
         Strings::Contains(extension, ".ips")) && path.Exists())
    list.emplace_back( path.Filename(), path, false );
  }
  return list;
}

void GuiMenuGameAdvanceLaunch::OptionListComponentChanged(int id, int index, const Path& value)
{
  (void)index;
  if ((Components)id == Components::Patch) return;
}

void GuiMenuGameAdvanceLaunch::AdvancedLaunch()
{
  GameLinkedData gameLinkedData = GameLinkedData();
  gameLinkedData.ConfigurablePatch().SetPatchFile(mPatch->getSelected());
  ViewController::Instance().Launch(&mFileData, gameLinkedData, Vector3f());
}

