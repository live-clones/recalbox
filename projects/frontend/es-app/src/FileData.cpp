#include "FileData.h"
#include "FileSorts.h"
#include "SystemData.h"
#include "Log.h"

#include <string.h>

namespace fs = boost::filesystem;

FileData::FileData(FileData::FileType type, const fs::path& path, SystemData* system)
	: mSystem(system),
    mParent(NULL),
    mType(type),
    mPath(path),
    mMetadata(getCleanName()) // TODO: Move clean name into metadata
{
}

FileData::FileData(const fs::path& path, SystemData* system) : FileData(FileData::FileType::Game, path, system)
{
}

std::string FileData::getCleanName() const
{
	std::string stem = mPath.stem().generic_string();
	if (mSystem != nullptr)
	  if ((mSystem->hasPlatformId(PlatformIds::ARCADE) || mSystem->hasPlatformId(PlatformIds::NEOGEO)))
		  stem = PlatformIds::getCleanMameName(stem.c_str());

  return stem;
}