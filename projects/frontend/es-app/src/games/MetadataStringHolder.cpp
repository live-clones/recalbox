//
// Created by bkg2k on 14/11/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include "MetadataStringHolder.h"

MetadataStringHolder::MetadataStringHolder(int capacity, int granularity)
  : mMetaString(capacity, granularity)
  , mIndexes(1024, 1024)
{
}

void MetadataStringHolder::Initialize()
{
  // Reinit everything
  mMetaString.Clear();
  mIndexes.Clear();
  mStringToIndexes.clear();
}

void MetadataStringHolder::Finalize()
{
  // Just clear the temporary dictionnary
  mStringToIndexes.clear();
}

std::string MetadataStringHolder::GetString(Index32 index)
{
  // Out of range
  if ((unsigned int)index >= (unsigned int)mIndexes.Count()) return std::string();
  // Regular string
  if (index < mIndexes.Count() - 1) return std::string(&mMetaString(mIndexes[index]), mIndexes[index + 1] - 1 - mIndexes[index]);
  // Last item is zero terminal
  return std::string(&mMetaString(mIndexes[index]), mMetaString.ByteSize() - 1 - mIndexes[index]);
}

MetadataStringHolder::Index32 MetadataStringHolder::AddString32(const std::string& newString)
{
  // Synchronize
  Mutex::AutoLock locker(mSyncher);

  // Already exists?
  Index32* index = mStringToIndexes.try_get(newString);
  if (index != nullptr) return *index;

  // Add new index
  Index32 result = mIndexes.Count();
  mStringToIndexes[newString] = result;
  // Adds string
  mIndexes.Add((int)mMetaString.Count());
  mMetaString.AddItems(newString.data(), (int)newString.size() + 1); // Include zero terminal
  return result;
}
