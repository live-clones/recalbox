#include "FileData.h"
#include "FileSorts.h"
#include "SystemData.h"
#include "Log.h"

#include <string.h>
#include <utils/StringUtil.h>

namespace fs = boost::filesystem;

FileData::FileData(ItemType type, const fs::path& path, SystemData* system)
	: mSystem(system),
    mParent(nullptr),
    mType(type),
    mPath(path),
    mMetadata(getDisplayName(), type) // TODO: Move clean name into metadata
{

}

FileData::FileData(const fs::path& path, SystemData* system) : FileData(ItemType::Game, path, system)
{
}

std::string FileData::getDisplayName() const
{
	std::string stem = mPath.stem().generic_string();
	if (mSystem != nullptr)
	  if ((mSystem->hasPlatformId(PlatformIds::PlatformId::ARCADE) || mSystem->hasPlatformId(PlatformIds::PlatformId::NEOGEO)))
		  stem = PlatformIds::getCleanMameName(stem.c_str());

  return stem;
}

std::string FileData::getScrappableName() const
{
  return StringUtil::removeParenthesis(getDisplayName());
}