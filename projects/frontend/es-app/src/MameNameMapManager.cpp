//
// Created by bkg2k on 03/04/2020.
//

#include <utils/Strings.h>
#include <cstring>
#include "MameNameMapManager.h"

void MameNameMapManager::GenerateMameNameHashes()
{
  for(int i = sMameListSize; --i >=0; )
    mMameNameHashes[i] = Strings::ToHash(mMameNameToRealName[i * 2]);
}

const char* MameNameMapManager::GetCleanMameName(const std::string& from)
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
