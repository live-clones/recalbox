#include "Music.h"
#include "utils/Log.h"
#include "audio/AudioManager.h"

Music* Music::sCurrentlyPlaying = nullptr;

Music* Music::LoadFromPath(const Path& path)
{
  { LOG(LogInfo) << "[Music] Loading music " << path.ToString(); }
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

  // Check midi file
  std::string ext = Strings::ToLowerASCII(mPath.Extension());
  if (ext == ".mid" || ext == ".midi")
  {
    Path soundfontPath = mPath.Directory() / "soundfont.sf2";
    Mix_SetSoundFonts(soundfontPath.ToChars());
  }

  //load wav file via SDL
  Mix_Music* gMusic = nullptr;
  gMusic = Mix_LoadMUS(mPath.ToChars());
  if (gMusic == nullptr)
  {
    { LOG(LogError) << "[Music] Error loading music \"" << mPath.ToString() << "\"!\n" << "	" << SDL_GetError(); }
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

  { LOG(LogInfo) << "[Music] Playing " << Name(); }
  if (Mix_FadeInMusic(mMusic, repeat ? -1 : 1, 1000) == -1)
  {
    { LOG(LogInfo) << "[Music] Mix_PlayMusic Error: " << Mix_GetError(); }
    return false;
  }

  // Record currently playing music
  sCurrentlyPlaying = this;

  if (!repeat)
    Mix_HookMusicFinished(MusicEndCallback);
  return true;
}

void Music::Pause()
{
  if (Mix_PlayingMusic() != 1)
  {
    { LOG(LogDebug) << "[Music] none music is playing, could not be paused "; }
    return;
  }

  if (Mix_PausedMusic() != 1)
  {
    { LOG(LogInfo) << "[Music] music will be pause "; }
    Mix_PauseMusic();
  }
  else
  { LOG(LogDebug) << "[Music] music is already paused "; }
}

void Music::Resume()
{
  if (Mix_PlayingMusic() != 1)
  {
    { LOG(LogDebug) << "[Music] none music is playing, could not be paused "; }
    return;
  }

  if(Mix_PausedMusic() == 1 )
  {
    { LOG(LogInfo) << "[Music] music will be resume "; }
    Mix_ResumeMusic();
  }
  else
  { LOG(LogDebug) << "[Music] none music is paused "; }
}

void Music::Stop()
{
  sCurrentlyPlaying = nullptr;
  Mix_HookMusicFinished(nullptr);
  Mix_FadeOutMusic(1000);
  Mix_HaltMusic();
}
