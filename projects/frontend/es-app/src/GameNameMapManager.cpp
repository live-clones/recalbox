//
// Created by bkg2k on 03/04/2020.
//

#include <utils/Strings.h>
#include <cstring>
#include "GameNameMapManager.h"

void GameNameMapManager::GenerateMameNameHashes()
{
  for(int i = sMameListSize; --i >=0; )
    mMameNameHashes[i] = Strings::ToHash(mMameNameToRealName[i * 2]);
}

void GameNameMapManager::GenerateFlashbackNameHashes()
{
  for(int i = sFlashbackListSize; --i >=0; )
    mFlashbackNameHashes[i] = Strings::ToHash(mFlashbackNameToRealName[i * 2]);
}

const char* GameNameMapManager::GetCleanMameName(const std::string& from)
{
  static bool HashGenerated = false;
  if (!HashGenerated)
  {
    GenerateMameNameHashes();
    HashGenerated = true;
  }

  int hash = Strings::ToHash(from);
  for(int i = sMameListSize; --i >= 0; )
    if (mMameNameHashes[i] == hash)
      if (strcmp(from.c_str(), mMameNameToRealName[i << 1]) == 0)
        return mMameNameToRealName[(i << 1) + 1];

  return nullptr;
}


const char* GameNameMapManager::GetCleanFlashbackName(const std::string& from)
{
  static bool HashGenerated = false;
  if (!HashGenerated)
  {
    GenerateFlashbackNameHashes();
    HashGenerated = true;
  }

  int hash = Strings::ToHash(from);
  for(int i = sFlashbackListSize; --i >= 0; )
    if (mFlashbackNameHashes[i] == hash)
      if (strcmp(from.c_str(), mFlashbackNameToRealName[i << 1]) == 0)
        return mFlashbackNameToRealName[(i << 1) + 1];

  return nullptr;
}

bool GameNameMapManager::HasRenaming(const SystemData& system)
{
  return system.Descriptor().Type() == SystemDescriptor::SystemType::Arcade ||
         system.Descriptor().Name() == "neogeo" ||
         system.Descriptor().Name() == "flashback";
}

bool GameNameMapManager::HasFiltering(const SystemData& system)
{
  return system.Descriptor().Type() == SystemDescriptor::SystemType::Arcade ||
         system.Descriptor().Name() == "neogeo";
}

bool GameNameMapManager::IsFiltered(const SystemData& system, const std::string& filenameWoExt)
{
  if (HasFiltering(system))
  {
    return (mMameBios.contains(filenameWoExt) || mMameDevices.contains(filenameWoExt));
  }
  return false;
}

std::string GameNameMapManager::Rename(const SystemData& system, const std::string& filenameWoExt)
{
  if (HasRenaming(system))
  {
    if (system.Descriptor().Type() == SystemDescriptor::SystemType::Arcade ||
        system.Descriptor().Name() == "neogeo")
    {
      const char* newName = GetCleanMameName(filenameWoExt);
      return newName != nullptr ? std::string(newName) : filenameWoExt;
    }
    if (system.Descriptor().Name() == "flashback")
    {
      const char* newName = GetCleanFlashbackName(Strings::ToLowerASCII(filenameWoExt));
      return newName != nullptr ? std::string(newName) : filenameWoExt;
    }
  }
  { LOG(LogError) << "[GameName] Renaming called on non-renamed platform game: " << filenameWoExt; }
  return filenameWoExt;
}
