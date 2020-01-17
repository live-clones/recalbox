#include "AudioManager.h"

#include <SDL.h>
#include <views/SystemView.h>
#include "utils/Log.h"
#include "RecalboxConf.h"
#include "Settings.h"
#include "themes/ThemeData.h"
#include "RootFolders.h"
#include "utils/sdl2/SyncronousEventService.h"
#include "utils/locale/LocaleHelper.h"
#include <utils/Strings.h>

AudioManager* AudioManager::sInstance;

AudioManager::AudioManager(Window& window)
  : mWindow(window),
    mCurrentMusic(0),
    mSender(SyncronousEventService::Instance().ObtainSyncCallback(this)),
    mRandomGenerator(mRandomDevice()),
    mSystemRandomizer()
{
  if (sInstance == nullptr)
  {
    sInstance = this;
    Initialize();
  }
  else
  {
    LOG(LogError) << "AudioManager multiple instance detected";
    exit(-1);
  }
}

AudioManager::~AudioManager()
{
  // Finalize SDL
  Finalize();

  // Null instance
  sInstance = nullptr;

  // Delete all sounds
  for(auto& sound : mSoundMap)
    delete sound.second;

  // Delete all musics
  for(auto& music : mMusicMap)
    delete music.second;
}

AudioManager& AudioManager::Instance()
{
  if (sInstance == nullptr)
  {
    LOG(LogError) << "AudioManager not available!";
    exit(-1);
  }
  return *sInstance;
}

void AudioManager::Initialize()
{
  if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0)
  {
    LOG(LogError) << "Error initializing SDL audio!\n" << SDL_GetError();
    return;
  }

  // Open the audio device and pause
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) < 0)
  {
    LOG(LogError) << "MUSIC Error - Unable to open SDLMixer audio: " << SDL_GetError();
    return;
  }

  LOG(LogInfo) << "SDL AUDIO Initialized";
}

void AudioManager::Finalize()
{
  // Completely tear down SDL audio. else SDL hogs audio resources and emulators might fail to start...
  LOG(LogInfo) << "Shutting down SDL AUDIO";
  Mix_HookMusicFinished(nullptr);
  Mix_HaltMusic();
  Mix_CloseAudio();
  SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void AudioManager::Reactivate()
{
  Initialize();

  if (RecalboxConf::Instance().AsBool("audio.bgmusic"))
    PlayMusic(mCurrentMusic, false);
}

AudioManager::AudioHandle AudioManager::LoadSound(const Path& path)
{
  // Get handle
  AudioHandle handle = Strings::ToHash64(path.ToString()) | 1;

  // Already exists
  if (mSoundMap.find(handle) != mSoundMap.end())
    return handle;

  // Try to load
  Sound* sound = Sound::BuildFromPath(path);
  if (sound == nullptr) return 0; // Not found
  // Add and return the handle
  mSoundMap[handle] = sound;
  return handle;
}

AudioManager::AudioHandle AudioManager::LoadSound(const ThemeData& theme, const std::string& view, const std::string& elem)
{
  // Get handle
  AudioHandle handle = Strings::ToHash64("Theme" + view + elem) | 1;

  // Already exists
  if (mSoundMap.find(handle) != mSoundMap.end())
    return handle;

  // Try to load
  Sound* sound = Sound::BuildFromTheme(theme, view, elem);
  if (sound == nullptr) return 0; // Not found
  // Add and return the handle
  mSoundMap[handle] = sound;
  return handle;
}

AudioManager::AudioHandle AudioManager::LoadMusic(const Path& path)
{
  // Get handle
  AudioHandle handle = Strings::ToHash64(path.ToString()) | 1;

  // Already exists
  if (mMusicMap.find(handle) != mMusicMap.end())
    return handle;

  // Try to load
  Music* music = Music::BuildFromPath(path);
  if (music == nullptr) return 0; // Not found
  // Add and return the handle
  mMusicMap[handle] = music;
  return handle;
}

AudioManager::AudioHandle AudioManager::LoadMusic(const ThemeData& theme, const std::string& view, const std::string& elem)
{
  // Get handle
  AudioHandle handle = Strings::ToHash64("Theme" + view + elem) | 1;

  // Already exists
  if (mMusicMap.find(handle) != mMusicMap.end())
    return handle;

  // Try to load
  Music* music = Music::BuildFromTheme(theme, view, elem);
  if (music == nullptr) return 0; // Not found
  // Add and return the handle
  mMusicMap[handle] = music;
  return handle;
}


bool AudioManager::PlaySound(AudioManager::AudioHandle handle)
{
  auto it = mSoundMap.find(handle);
  if (it != mSoundMap.end())
  {
    StopAll();
    it->second->Play();
    return true;
  }
  return false;
}

bool AudioManager::PlayMusic(AudioManager::AudioHandle handle, bool loop)
{
  auto it = mMusicMap.find(handle);
  if (it != mMusicMap.end())
  {
    mCurrentMusicTitle = it->second->Name();
    StopAll();
    it->second->Play(loop);
    return true;
  }
  return false;
}

void AudioManager::StopAll()
{
  Music::Stop();
  Sound::Stop();
  mCurrentMusic = 0;
}

void AudioManager::StartPlaying(const ThemeData& theme)
{
  if (RecalboxConf::Instance().AsBool("audio.bgmusic"))
  {
    const ThemeElement* elem = theme.getElement("system", "directory", "sound");
    mThemeMusicFolder = ((elem == nullptr) || !elem->HasProperty("path")) ? Path::Empty : Path(elem->AsString("path"));

    AudioHandle handle = LoadMusic(theme, "system", "bgsound");
    // Found a music for the system
    if (handle != 0)
    {
      PlayMusic(handle, true);
      mCurrentMusic = handle;
      return;
    }
    else PlayRandomMusic();
  }
}

void AudioManager::PlayRandomMusic(bool allowTheSame)
{
  for(int i = 3; --i >= 0; )
  {
    // Find a random song in user directory or theme music directory
    AudioHandle music = FetchRandomMusic(mThemeMusicFolder);
    if (music == 0) return; // No music, exit
    if (i == 0) allowTheSame = true; // Last chance, allow the same
    if ((music != mCurrentMusic) || allowTheSame)
    {
      PlayMusic(music, false);
      mCurrentMusic = music;
      int popupDuration = Settings::Instance().MusicPopupTime();
      if (popupDuration != 0)
      {
        // Create music popup
        std::shared_ptr<GuiInfoPopup> popup =
          std::make_shared<GuiInfoPopup>(mWindow, _("Now playing") + ":\n" + mCurrentMusicTitle, popupDuration,10);
        mWindow.setInfoPopup(popup);
      }
      return;
    }
  }
}

std::vector<Path> AudioManager::ListMusicInFolder(const Path& path)
{
  std::vector<Path> musics;

  if (!path.IsDirectory()) return musics;

  Path::PathList list = path.GetDirectoryContent();
  for(Path& musicPath : list)
  {
    // Skip if not a file
    if (!musicPath.IsFile()) continue;

    // Skip if no match
    std::string ext = Strings::ToLowerASCII(musicPath.Extension());
    static std::string supportedExtensions = "|.mp3|.ogg|";
    if (supportedExtensions.find(ext) == std::string::npos) continue;

    // File matches, store it
    musics.push_back(musicPath);
  }

  return musics;
}

AudioManager::AudioHandle AudioManager::FetchRandomMusic(const Path& themeMusicDirectory)
{
  // 1 check in User music directory
  std::vector<Path> musics = ListMusicInFolder(Path(Settings::Instance().MusicDirectory()));
  if (musics.empty() && !themeMusicDirectory.IsEmpty())
    musics = ListMusicInFolder(themeMusicDirectory);
  if (musics.empty())
    return 0;

  // Adjust randomizer distribution
  if (mSystemRandomizer.b() != (int)musics.size() - 1)
    mSystemRandomizer = std::uniform_int_distribution<int>(0, (int)musics.size() - 1);

  int randomIndex = mSystemRandomizer(mRandomGenerator);
  AudioHandle music = LoadMusic(Path(musics.at(randomIndex)));
  return music;
}

void AudioManager::ReceiveSyncCallback(const SDL_Event& event)
{
  (void)event;
  PlayRandomMusic(true);
}
