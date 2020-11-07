#include "FileData.h"
#include "systems/SystemData.h"

#include <utils/Strings.h>
#include <MameNameMapManager.h>

FileData::FileData(ItemType type, const Path& path, SystemData* system)
	: mSystem(system),
    mParent(nullptr),
    mType(type),
    mPath(path),
    mMetadata(getDisplayName(), type) // TODO: Move clean name into metadata
{

}

FileData::FileData(const Path& path, SystemData* system) : FileData(ItemType::Game, path, system)
{
}

std::string FileData::getDisplayName() const
{
	std::string stem = mPath.FilenameWithoutExtension();
	if (mSystem != nullptr)
	  if ((mSystem->hasPlatformId(PlatformIds::PlatformId::ARCADE) || mSystem->hasPlatformId(PlatformIds::PlatformId::NEOGEO)))
    {
      const char* newName = MameNameMapManager::GetCleanMameName(stem);
      if (newName != nullptr)
        return newName;
    }

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
