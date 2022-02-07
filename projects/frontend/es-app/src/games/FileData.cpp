#include "FileData.h"
#include "systems/SystemData.h"

#include <utils/Strings.h>
#include <GameNameMapManager.h>
#include <utils/Zip.h>
#include <utils/hash/Crc32File.h>

FileData::FileData(ItemType type, const Path& path, RootFolderData& ancestor)
	: mTopAncestor(ancestor),
    mParent(nullptr),
    mType(type),
    mPath(path),
    mMetadata(DisplayName(), type) // TODO: Move clean name into metadata
{

}

FileData::FileData(const Path& path, RootFolderData& ancestor) : FileData(ItemType::Game, path, ancestor)
{
}

std::string FileData::DisplayName() const
{
	std::string stem = mPath.FilenameWithoutExtension();
	if (mType == ItemType::Game)
    if (GameNameMapManager::HasRenaming(System()))
      stem = GameNameMapManager::Rename(System(), stem);

  return stem;
}

std::string FileData::ScrappableName() const
{
  return Strings::RemoveParenthesis(DisplayName());
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

SystemData& FileData::System() const
{
  return mTopAncestor.RootSystem();
}

FileData& FileData::CalculateHash()
{
  if (mType != ItemType::Game) return *this;
  if (mPath.Size() > (20 << 20)) return *this; // Ignore file larger than 20Mb

  bool done = false;
  if (Strings::ToLowerASCII(mPath.Extension()) == ".zip")
  {
    Zip zip(mPath);
    if (zip.Count() == 1)
    {
      mMetadata.SetRomCrc32(zip.Crc32(0));
      done = true;
    }
  }

  if (!done)
  {
    // Hash file
    unsigned int result = 0;
    if (Crc32File(mPath).Crc32(result))
      mMetadata.SetRomCrc32((int) result);
  }

  return *this;
}