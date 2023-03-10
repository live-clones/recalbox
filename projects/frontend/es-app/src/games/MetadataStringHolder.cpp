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
  Initialize();
}

void MetadataStringHolder::Initialize()
{
  // Reinit everything
  mMetaString.Clear();
  mIndexes.Clear();
  mStringToIndexes.clear();
  // Add empty string so that all uninitialized indexes must be set to 0
  AddString32(Strings::Empty);
}

void MetadataStringHolder::Finalize()
{
  // Just clear the temporary dictionnary
  mStringToIndexes.clear();
}

std::string MetadataStringHolder::GetString(Index32 index)
{
  // Synchronize
  Mutex::AutoLock locker(mSyncher);

  // Out of range
  if ((unsigned int)index >= (unsigned int)mIndexes.Count())
  #ifdef DEBUG
    return std::string("Unknown Index ").append(Strings::ToString(index));
  #else
  {
    { LOG(LogError) << "[MetadataStringHolder] GetString - Index error! " << index; }
    return std::string();
  }
  #endif
  // Regular string
  if (index < mIndexes.Count() - 1) return std::string(&mMetaString(mIndexes[index]), mIndexes[index + 1] - 1 - mIndexes[index]);
  // Last item is zero terminal
  return std::string(&mMetaString(mIndexes[index]), mMetaString.Count() - 1 - mIndexes[index]);
}

Path MetadataStringHolder::GetPath(MetadataStringHolder::Index32 index)
{
  // Synchronize
  Mutex::AutoLock locker(mSyncher);

  // Out of range
  if ((unsigned int)index >= (unsigned int)mIndexes.Count())
  #ifdef DEBUG
    return Path("/unknown/index") / Strings::ToString(index);
  #else
  {
    { LOG(LogError) << "[MetadataStringHolder] GetPath - Index error! " << index; }
    return Path::Empty;
  }
  #endif
  // Regular string
  if (index < mIndexes.Count() - 1) return Path(&mMetaString(mIndexes[index]), mIndexes[index + 1] - 1 - mIndexes[index]);
  // Last item is zero terminal
  return Path(&mMetaString(mIndexes[index]), mMetaString.Count() - 1 - mIndexes[index]);
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
  // Add string
  mIndexes.Add((int)mMetaString.Count());
  const char* stringData = newString.data();
  mMetaString.AddItems(stringData != nullptr ? stringData : "", (int)newString.size() + 1); // Include zero terminal
  return result;
}

void MetadataStringHolder::FindText(const std::string& text, MetadataStringHolder::FoundTextList& output, int context)
{
  const char* fdn = mMetaString.BufferReadOnly();             // Keep filedata name pointer for fast result computation
  const char* tts = text.c_str();                             // Keep text pointer for fast search reset
  int lmax = (int)mMetaString.Count() - (int)text.size() + 1; // MAximum byte to search in

  for(const char* p = fdn; --lmax >= 0; ++p)                  // Run through the game name, straight forward
    if ((*p | 0x20) == (*tts))                                // Try to catch the first char
      for (const char* s = tts, *ip = p; ; )                  // Got it, run through both string
      {
        const char c = *(++s);
        if ((c != 0) && ((*(++ip) | 0x20) != c) ) break;      // Chars are not equal, exit the inner loop
        if (c == 0)
        {
          output.Add(IndexFromPos((int) (p - fdn), context)); // Chars are equal, got a zero terminal? Found it!
          break;
        }
      }
}

void MetadataStringHolder::FindIndex(const std::string& index, MetadataStringHolder::FoundTextList& output, int context)
{
  const char* fdn = mMetaString.BufferReadOnly();             // Keep filedata name pointer for fast result computation
  int lmax = (int)mMetaString.Count() - (int)index.size() + 1; // MAximum byte to search in

  for(const char* p = fdn; --lmax >= 0; ++p)
  {
    bool equals = strcmp(index.c_str(), p) == 0;
    if(equals)
      output.Add(IndexFromPos((int) (p - fdn), context));
  //  output.Add(IndexFromPos((int) (p - fdn), context));
  }


}

MetadataStringHolder::IndexAndDistance MetadataStringHolder::IndexFromPos(int pos, int context)
{
  // Out of bounds?
  if ((unsigned int)pos > (unsigned int)mMetaString.Count()) return { -1, -1, -1 };
  // Last item? (avoid the end computation in the main loop)
  if (pos >= mIndexes.Last()) return { mIndexes.Count() - 1, (short)(pos - mIndexes.Last()), (short)context };

  for(int left = 0, right = mIndexes.Count() - 2; ; )
  {
    int pivot = (left + right) >> 1;
    if (pos < mIndexes[pivot]) right = pivot;
    else if (pos >= mIndexes[pivot + 1]) left = pivot;
    else return { pivot, (short)(pos - mIndexes[pivot]), (short)context };
  }
}
