#include "FileData.h"
#include "systems/SystemData.h"

#include <utils/Strings.h>
#include <GameNameMapManager.h>

FileData::FileData(ItemType type, const Path& path, RootFolderData& ancestor)
	: mTopAncestor(ancestor),
    mParent(nullptr),
    mType(type),
    mPath(path),
    mMetadata(getDisplayName(), type) // TODO: Move clean name into metadata
{

}

FileData::FileData(const Path& path, RootFolderData& ancestor) : FileData(ItemType::Game, path, ancestor)
{
}

std::string FileData::getDisplayName() const
{
	std::string stem = mPath.FilenameWithoutExtension();
	if (mType == ItemType::Game)
    if (GameNameMapManager::HasRenaming(mTopAncestor.System()))
      stem = GameNameMapManager::Rename(mTopAncestor.System(), stem);

  return stem;
}

std::string FileData::getScrappableName() const
{
  return Strings::RemoveParenthesis(getDisplayName());
}

bool FileData::HasP2K() const
{
  // Check game file
  Path p2k(P2KPath());
  if (p2k.Exists()) return true;

  // Check folder file until reaching the root
  for(p2k = p2k.Directory(); !p2k.IsEmpty(); p2k = p2k.Directory())
    if ((p2k / ".p2k.cfg").Exists())
      return true;

  return false;
}

SystemData* FileData::getSystem() const
{
  return &mTopAncestor.System();
}
