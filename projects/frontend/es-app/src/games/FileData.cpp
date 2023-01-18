#include "FileData.h"
#include <systems/SystemData.h>
#include <games/adapter/GameAdapter.h>

#include <utils/Strings.h>
#include <utils/Zip.h>
#include <utils/hash/Crc32File.h>

FileData::FileData(ItemType type, const Path& path, RootFolderData& ancestor)
	: mTopAncestor(ancestor)
    , mParent(nullptr)
    , mType(type)
    , mMetadata(path, type != ItemType::Root ? GameAdapter::RawDisplayName(ancestor.System(), path) : path.FilenameWithoutExtension(), type)
{
}

FileData::FileData(const Path& path, RootFolderData& ancestor) : FileData(ItemType::Game, path, ancestor)
{
}

std::string FileData::DisplayName(const Path& romPath) const
{
  (void)romPath;
  GameAdapter adapter(*this);
  return adapter.DisplayName();
}

std::string FileData::DisplayableName() const
{
  RecalboxConf& recalboxConf = RecalboxConf::Instance();

  std::string displayableName = Name();
  DisplayGameBy  displayGameBy = recalboxConf.GetDisplayGameBy();

  switch (displayGameBy)
  {
    case DisplayGameBy::Filename : displayableName = RomPath().Filename(); break;
    case DisplayGameBy::Alias :
      if (!Metadata().Alias().empty())
        displayableName = Metadata().Alias();
      break;
    case DisplayGameBy::Name : break;
  }

  if (recalboxConf.GetDisplayGameRegions() && !Regions().empty())
    displayableName.append(" [").append(Regions()).append("]");

  return displayableName;
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
  Path path(mMetadata.Rom());

  if (mType != ItemType::Game) return *this;
  if (path.Size() > (20 << 20)) return *this; // Ignore file larger than 20Mb

  bool done = false;
  if (Strings::ToLowerASCII(path.Extension()) == ".zip")
  {
    Zip zip(path);
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
    if (Crc32File(path).Crc32(result))
      mMetadata.SetRomCrc32((int) result);
  }

  return *this;
}

std::string FileData::Regions() const
{
  std::string fileName = mMetadata.RomFileOnly().ToString();
  Regions::RegionPack regions = Regions::ExtractRegionsFromNoIntroName(fileName);
  if (!regions.HasRegion())
    regions = Regions::ExtractRegionsFromTosecName(fileName);
  if (!regions.HasRegion())
    regions = Regions::ExtractRegionsFromName(Strings::ToLowerASCII(Name()));
  if (!regions.HasRegion())
    regions = Metadata().Region();

  return Regions::Serialize4Regions(regions);
}

bool FileData::UpdateMetadataFrom(FileData& from)
{
  if (from.RomPath() != RomPath()) return false;
  mMetadata = std::move(from.mMetadata);
  return true;
}

bool FileData::IsDisplayable() const
{
  RecalboxConf& conf = RecalboxConf::Instance();

  if(IsFolder())
    return false;
  if(conf.GetShowOnlyLatestVersion() && !Metadata().LatestVersion())
    return false;
  if(conf.GetFavoritesOnly() && !Metadata().Favorite())
    return false;

  if(!conf.GetShowHidden() && Metadata().Hidden())
    return false;
  if(conf.GetHideNoGames() && Metadata().NoGame())
    return false;
  if(conf.GetFilterAdultGames() && Metadata().Adult())
    return false;
  if(conf.GetGlobalHidePreinstalled() && TopAncestor().PreInstalled())
    return false;

  return true;
}