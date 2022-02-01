#include "Sound.h"
#include <utils/Log.h>

Sound* Sound::BuildFromPath(const Path& path)
{
  if (path.Exists()) return new Sound(path);
  return nullptr;
}

Sound::Sound(const Path& path)
  : mPath(path),
    mSampleData(nullptr)
{
  Initialize();
}

Sound::~Sound()
{
  Finalize();
}

void Sound::Initialize()
{
  if (mPath.IsEmpty()) return;

  //load wav file via SDL
  mSampleData = Mix_LoadWAV(mPath.ToChars());
  if (mSampleData == nullptr)
  { LOG(LogError) << "[Sound] Error loading sound \"" << mPath.ToString() << "\"!\n" << "	" << SDL_GetError(); }
}

void Sound::Finalize()
{
  if (mSampleData != nullptr)
    Mix_FreeChunk(mSampleData);
}

void Sound::Play()
{
  if (mSampleData == nullptr) return;
  Mix_PlayChannel(-1, mSampleData, 0);
}

void Sound::Stop()
{
  Mix_HaltChannel(-1);
}

