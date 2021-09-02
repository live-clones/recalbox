#include "AudioManager.h"

#include <SDL.h>
#include <views/SystemView.h>
#include "RecalboxConf.h"
#include "RootFolders.h"
#include "utils/sdl2/SyncronousEventService.h"
#include "utils/locale/LocaleHelper.h"
#include <utils/Strings.h>

AudioManager* AudioManager::sInstance;

AudioManager::AudioManager(WindowManager& window)
  : mWindow(window),
    mCurrentMusic(0),
    mCurrentMusicSource(MusicSource::None),
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
    { LOG(LogError) << "[AudioManager] multiple instance detected"; }
    exit(-1);
  }
}

AudioManager::~AudioManager()
{
  // Finalize SDL
  Finalize();

  // Null instance
  sInstance = nullptr;
}

void AudioManager::ClearCaches()
{
  // Delete all sounds
  for(auto& sound : mSoundMap)
    delete sound.second;
  mSoundMap.clear();

  // Delete all musics
  for(auto& music : mMusicMap)
    delete music.second;
  mMusicMap.clear();
}

AudioManager& AudioManager::Instance()
{
  if (sInstance == nullptr)
  {
    { LOG(LogError) << "[AudioManager] AudioManager not available!"; }
    exit(-1);
  }
  return *sInstance;
}

void AudioManager::Initialize()
{
  ClearCaches();
  mCurrentMusic = 0;
  mCurrentMusicSource = MusicSource::None;

  /*if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0)
  {
    { LOG(LogError) << "[AudioManager] Error initializing SDL audio!\n" << SDL_GetError(); }
    return;
  }*/

  // Open the audio device and pause
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) < 0)
  {
    { LOG(LogError) << "[AudioManager] MUSIC Error - Unable to open SDLMixer audio: " << SDL_GetError(); }
    return;
  }

  { LOG(LogInfo) << "[AudioManager] SDL AUDIO Initialized"; }
}

void AudioManager::Finalize()
{
  // Completely tear down SDL audio. else SDL hogs audio resources and emulators might fail to start...
  { LOG(LogInfo) << "[AudioManager] Shutting down SDL AUDIO"; }
  Mix_HookMusicFinished(nullptr);
  Mix_HaltMusic();
  Mix_CloseAudio();
  //SDL_QuitSubSystem(SDL_INIT_AUDIO);

  // Free musics/sounds
  ClearCaches();
}

void AudioManager::Reactivate()
{
  Initialize();

  if (AudioModeTools::CanPlayMusic())
    PlayRandomMusic();
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

AudioManager::AudioHandle AudioManager::LoadMusic(const Path& path)
{
  // Get handle
  AudioHandle handle = Strings::ToHash64(path.ToString()) | 1;

  // Already exists
  if (mMusicMap.find(handle) != mMusicMap.end())
    return handle;

  // Try to load
  Music* music = Music::LoadFromPath(path);
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

void AudioManager::PauseMusic()
{
    Music::Pause();
}

void AudioManager::ResumeMusic()
{
    Music::Resume();
}

void AudioManager::StopAll()
{
  Music::Stop();
  Sound::Stop();
  mCurrentMusic = 0;
}

void AudioManager::StartPlaying(const ThemeData& theme)
{
  if (AudioModeTools::CanPlayMusic())
  {
    const ThemeElement* elem = theme.getElement("system", "directory", "sound");
    mThemeMusicFolder = ((elem == nullptr) || !elem->HasProperty("path")) ? Path::Empty : Path(elem->AsString("path"));
    elem = theme.getElement("system", "bgsound", "sound");
    mThemeMusic = ((elem == nullptr) || !elem->HasProperty("path")) ? Path::Empty : Path(elem->AsString("path"));

    PlayRandomMusic();
  }
}

void AudioManager::PlayRandomMusic()
{
  Path previousPath = Music::CurrentlyPlaying() != nullptr ? Music::CurrentlyPlaying()->FilePath() : Path::Empty;
  AudioHandle musicToPlay = 0;
  const char* log = "No music found.";
  MusicSource source = MusicSource::None;

  // check Theme music first
  if (!mThemeMusic.IsEmpty())
  {
    musicToPlay = LoadMusic(mThemeMusic);
    log = "Theme music found (Background).";
    source = MusicSource::ThemeSystem;
  }

  // Then check user folder
  Path userMusics(sMusicFolder);
  if (source == MusicSource::None && !userMusics.IsEmpty())
  {
    Path musicPath = FetchRandomMusic(userMusics, previousPath);
    if (!musicPath.IsEmpty())
    {
      musicToPlay = LoadMusic(musicPath);
      log = "User music found.";
      source = MusicSource::User;
    }
  }

  // Finally check theme folder
  if (source == MusicSource::None && !mThemeMusicFolder.IsEmpty())
  {
    Path musicPath = FetchRandomMusic(mThemeMusicFolder, previousPath);
    if (!musicPath.IsEmpty())
    {
      musicToPlay = LoadMusic(musicPath);
      log = "Theme music found (From theme folder).";
      source = MusicSource::Theme;
    }
  }

  { LOG(LogInfo) << "[AudioManager] " << log; }
  if (source == MusicSource::None)
  {
    { LOG(LogError) << "[AudioManager] No music source!"; }
    return;
  }

  // Do not relaunch currently playing song
  if (mCurrentMusic == musicToPlay)
    return;

  // Do not interrupt musics except for a theme system music
  if (mCurrentMusic != 0)
    if ((source != MusicSource::ThemeSystem) && (source == mCurrentMusicSource))
      return;

  // Play!
  PlayMusic(musicToPlay, false);
  mCurrentMusic = musicToPlay;
  mCurrentMusicSource = source;

  // Popup?
  int popupDuration = RecalboxConf::Instance().GetPopupMusic();
  if (popupDuration != 0)
  {
    // Create music popup
    mWindow.InfoPopupAdd(new GuiInfoPopup(mWindow, _("Now playing") + ":\n" + mCurrentMusicTitle, popupDuration,
                                          GuiInfoPopup::PopupType::Music));
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
    static std::string supportedExtensions = "|.wav|.mp3|.ogg|.flac|.midi|.mid|.mod|.s3m|.xm|.it|.669|.apun|.dsm|.far|.amf|.gdm|.imf|.med|.mtm|.okt|.stm|.stx|.ult|.uni|.opus|";
    if (supportedExtensions.find(ext) == std::string::npos) continue;

    // File matches, store it
    musics.push_back(musicPath);
  }

  return musics;
}

Path AudioManager::FetchRandomMusic(const Path& from, const Path& previousPath)
{
  // Get musics from folders
  std::vector<Path> musics = ListMusicInFolder(from);
  if (musics.empty())
    return Path::Empty;

  // Adjust randomizer distribution
  if (mSystemRandomizer.b() != (int)musics.size() - 1)
    mSystemRandomizer = std::uniform_int_distribution<int>(0, (int)musics.size() - 1);

  // Try to find a new music
  for(int i = sMaxTries; --i >= 0; )
  {
    int randomIndex = mSystemRandomizer(mRandomGenerator);
    if (!previousPath.IsEmpty() && (musics[randomIndex] == previousPath.ToString()))
      continue;
    return Path(musics[randomIndex]);
  }

  return Path::Empty;
}

void AudioManager::ReceiveSyncCallback(const SDL_Event& event)
{
  (void)event;
  mCurrentMusic = 0;
  PlayRandomMusic();
}

void AudioManager::PauseMusicIfNecessary()
{
  if(AudioMode::MusicsXorVideosSound == RecalboxConf::Instance().GetAudioMode())
    PauseMusic();
}

void AudioManager::ResumeMusicIfNecessary()
{
  if(AudioMode::MusicsXorVideosSound == RecalboxConf::Instance().GetAudioMode())
    ResumeMusic();
}
