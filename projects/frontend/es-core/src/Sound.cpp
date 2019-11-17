#include "Sound.h"
#include "AudioManager.h"
#include "utils/Log.h"
#include "Settings.h"
#include "themes/ThemeData.h"

Sound* Sound::BuildFromPath(const Path& path)
{
  if (path.Exists()) return new Sound(path);
  return nullptr;
}

Sound* Sound::BuildFromTheme(const ThemeData& theme, const std::string& view, const std::string& element)
{
  LOG(LogInfo) << "Load sound [" << view << "." << element << "]";

  const ThemeElement* elem = theme.getElement(view, element, "sound");
  if ((elem == nullptr) || !elem->HasProperty("path"))
  {
    LOG(LogError) << view << '.' << element << " not found";
    return nullptr;
  }

  return new Sound(Path(elem->AsString("path")));
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
  if (mPath.Empty()) return;

  //load wav file via SDL
  mSampleData = Mix_LoadWAV(mPath.ToChars());
  if (mSampleData == nullptr)
    LOG(LogError) << "Error loading sound \"" << mPath.ToString() << "\"!\n" << "	" << SDL_GetError();
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

