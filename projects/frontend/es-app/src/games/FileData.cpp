#include "FileData.h"
#include "systems/SystemData.h"

#include <utils/Strings.h>

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
      const char* newName = PlatformIds::getCleanMameName(stem);
      if (newName != nullptr)
        return newName;
    }

  return stem;
}

std::string FileData::getScrappableName() const
{
  return Strings::RemoveParenthesis(getDisplayName());
}