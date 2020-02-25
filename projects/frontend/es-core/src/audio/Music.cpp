#include "Music.h"
#include "utils/Log.h"
#include "audio/AudioManager.h"

Music* Music::sCurrentlyPlaying = nullptr;

Music* Music::LoadFromPath(const Path& path)
{
  LOG(LogInfo) << "Loading music " << path.ToString();
  if (path.Exists())
    return new Music(path);
  return nullptr;
}

Music::Music(const Path& path)
  : mPath(path),
    mMusic(nullptr)
{
  Initialize();
}

Music::~Music()
{
  Finalize();
}

void Music::Initialize()
{
  if (mPath.IsEmpty()) return;

  //load wav file via SDL
  Mix_Music* gMusic = nullptr;
  gMusic = Mix_LoadMUS(mPath.ToChars());
  if (gMusic == nullptr)
  {
    LOG(LogError) << "Error loading sound \"" << mPath.ToString() << "\"!\n" << "	" << SDL_GetError();
    return;
  }
  mMusic = gMusic;
}

void Music::Finalize()
{
  // This one is currently playing?
  if (sCurrentlyPlaying == this)
    sCurrentlyPlaying = nullptr;

  // Valid SDL music?
  if (mMusic != nullptr)
  {
    Mix_FreeMusic(mMusic);
    mMusic = nullptr;
  }
}

static void MusicEndCallback()
{
  AudioManager::Instance().SignalEndOfMusic();
}

bool Music::Play(bool repeat)
{
  if (mMusic == nullptr) return false;

  LOG(LogInfo) << "Playing " << Name();
  if (Mix_FadeInMusic(mMusic, repeat ? -1 : 1, 1000) == -1)
  {
    LOG(LogInfo) << "Mix_PlayMusic Error: " << Mix_GetError();
    return false;
  }

  // Record currently playing music
  sCurrentlyPlaying = this;

  if (!repeat)
    Mix_HookMusicFinished(MusicEndCallback);
  return true;
}

void Music::Stop()
{
  sCurrentlyPlaying = nullptr;
  Mix_HookMusicFinished(nullptr);
  Mix_FadeOutMusic(1000);
  Mix_HaltMusic();
}

