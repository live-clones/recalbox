#include "Music.h"
#include "utils/Log.h"
#include "Settings.h"
#include "themes/ThemeData.h"
#include "AudioManager.h"

Music* Music::BuildFromPath(const Path& path)
{
  LOG(LogInfo) << "Load music " << path.ToString();
  if (path.Exists()) return new Music(path);
  return nullptr;
}

Music* Music::BuildFromTheme(const ThemeData& theme, const std::string& view,
                             const std::string& element)
{
  LOG(LogInfo) << "Load music [" << view << "." << element << "]";
  const ThemeElement* elem = theme.getElement(view, element, "sound");
  if ((elem == nullptr) || !elem->HasProperty("path"))
  {
    LOG(LogError) << view << '.' << element << " not found";
    return nullptr;
  }

  return BuildFromPath(Path(elem->AsString("path")));
}

Music::Music(const Path& path)
  : mPath(path),
    mMusic(nullptr),
    mIsPlaying(false)
{
  Initialize();
}

Music::~Music()
{
  Finalize();
}

void Music::Initialize()
{
  if (mPath.Empty()) return;

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
  mIsPlaying = false;
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
  mIsPlaying = true;

  LOG(LogInfo) << "Playing " << Name();
  if (Mix_FadeInMusic(mMusic, repeat ? -1 : 1, 1000) == -1)
  {
    LOG(LogInfo) << "Mix_PlayMusic Error: " << Mix_GetError();
    return false;
  }
  if (!repeat)
    Mix_HookMusicFinished(MusicEndCallback);
  return true;
}

void Music::Stop()
{
  Mix_FadeOutMusic(1000);
  Mix_HaltMusic();
}

