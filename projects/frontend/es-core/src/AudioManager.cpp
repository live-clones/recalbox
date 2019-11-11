#include "AudioManager.h"

#include <SDL.h>
#include <views/SystemView.h>
#include "utils/Log.h"
#include "RecalboxConf.h"
#include "Settings.h"
#include "themes/ThemeData.h"
#include "Locale.h"
#include "RootFolders.h"
#include "utils/sdl2/SyncronousEventService.h"
#include <unistd.h>
#include <ctime>
#include <utils/Strings.h>

std::vector<std::shared_ptr<Sound>> AudioManager::sSoundVector;
std::vector<std::shared_ptr<Music>> AudioManager::sMusicVector;


std::shared_ptr<AudioManager> AudioManager::sInstance;


AudioManager::AudioManager()
  : currentMusic(nullptr),
    mWindow(nullptr),
    mSender(SyncronousEventService::Instance().ObtainSyncCallback(this)),
    running(false),
    runningFromPlaylist(false)
{
  init();
}

AudioManager::~AudioManager()
{
  deinit();
}

std::shared_ptr<AudioManager>& AudioManager::getInstance()
{
  //check if an AudioManager instance is already created, if not create one
  if (sInstance == nullptr)
  {
    sInstance = std::shared_ptr<AudioManager>(new AudioManager);
  }
  return sInstance;
}

void AudioManager::init()
{
  runningFromPlaylist = false;
  if (!running)
  {
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0)
    {
      LOG(LogError) << "Error initializing SDL audio!\n" << SDL_GetError();
      return;
    }

    //Open the audio device and pause
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) < 0)
    {
      LOG(LogError) << "MUSIC Error - Unable to open SDLMixer audio: " << SDL_GetError() << std::endl;
    }
    else
    {
      LOG(LogInfo) << "SDL AUDIO Initialized";
      running = true;
    }
  }
}

void AudioManager::deinit()
{
  //stop all playback
  //stop();
  //completely tear down SDL audio. else SDL hogs audio resources and emulators might fail to start...
  LOG(LogInfo) << "Shutting down SDL AUDIO";
  Mix_HookMusicFinished(nullptr);
  Mix_HaltMusic();
  Mix_CloseAudio();
  SDL_QuitSubSystem(SDL_INIT_AUDIO);
  running = false;
}

void AudioManager::stopMusic()
{
  Mix_FadeOutMusic(1000);
  Mix_HaltMusic();
  currentMusic = nullptr;
}

void musicEndInternal()
{
  AudioManager::getInstance()->musicEnd();
}

void AudioManager::themeChanged(const ThemeData& theme)
{
  if (RecalboxConf::Instance().AsBool("audio.bgmusic"))
  {
    const ThemeElement* elem = theme.getElement("system", "directory", "sound");
    if ((elem == nullptr) || !elem->HasProperty("path"))
    {
      currentThemeMusicDirectory = Path();
    }
    else
    {
      currentThemeMusicDirectory = Path(elem->AsString("path"));
    }

    std::shared_ptr<Music> bgsound = Music::getFromTheme(theme, "system", "bgsound");

    // Found a music for the system
    if (bgsound)
    {
      runningFromPlaylist = false;
      stopMusic();
      bgsound->play(true, nullptr);
      currentMusic = bgsound;
      return;
    }

    if (!runningFromPlaylist)
    {
      playRandomMusic();
    }
  }
}

void AudioManager::playRandomMusic()
{
  // Find a random song in user directory or theme music directory
  std::shared_ptr<Music> bgsound = getRandomMusic(currentThemeMusicDirectory);
  if (bgsound)
  {
    runningFromPlaylist = true;
    stopMusic();
    bgsound->play(false, musicEndInternal);
    currentMusic = bgsound;
    int popupDuration = Settings::Instance().MusicPopupTime();
    if (popupDuration != 0)
    {
      // Create music popup
      mLastPopupText = _("Now playing") + ":\n" + currentMusic->getName();

      // Push synhroneous event
      mSender.Call();
    }
    return;
  }
  else
  {
    // Not running from playlist, and no theme song found
    stopMusic();
  }
}


void AudioManager::resumeMusic()
{
  this->init();
  if (currentMusic != nullptr && RecalboxConf::Instance().AsBool("audio.bgmusic"))
  {
    currentMusic->play(!runningFromPlaylist, runningFromPlaylist ? musicEndInternal : nullptr);
  }
}

void AudioManager::registerSound(std::shared_ptr<Sound>& sound)
{
  getInstance();
  sSoundVector.push_back(sound);
}

void AudioManager::registerMusic(std::shared_ptr<Music>& music)
{
  getInstance();
  sMusicVector.push_back(music);
}

void AudioManager::unregisterSound(std::shared_ptr<Sound>& sound)
{
  getInstance();
  for (unsigned int i = 0; i < sSoundVector.size(); i++)
  {
    if (sSoundVector.at(i) == sound)
    {
      sSoundVector[i]->stop();
      sSoundVector.erase(sSoundVector.begin() + i);
      return;
    }
  }
  LOG(LogError) << "AudioManager Error - tried to unregister a sound that wasn't registered!";
}

void AudioManager::unregisterMusic(std::shared_ptr<Music>& music)
{
  getInstance();
  for (unsigned int i = 0; i < (unsigned int)sMusicVector.size(); i++)
  {
    if (sMusicVector.at(i) == music)
    {
      //sMusicVector[i]->stop();
      sMusicVector.erase(sMusicVector.begin() + i);
      return;
    }
  }
  LOG(LogError) << "AudioManager Error - tried to unregister a music that wasn't registered!";
}

void AudioManager::play()
{
  getInstance();

  //unpause audio, the mixer will figure out if samples need to be played...
  //SDL_PauseAudio(0);
}

void AudioManager::stop()
{
  //stop playing all Sounds
  for (auto & i : sSoundVector)
  {
    if (i->isPlaying())
    {
      i->stop();
    }
  }
  //stop playing all Musics


  //pause audio
  //SDL_PauseAudio(1);
}


std::vector<Path> getMusicIn(const Path& path)
{
  std::vector<Path> all_matching_files;

  if (!path.IsDirectory()) return all_matching_files;

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
    all_matching_files.push_back(musicPath);
  }

  return all_matching_files;
}

std::shared_ptr<Music> AudioManager::getRandomMusic(const Path& themeSoundDirectory)
{
  // 1 check in User music directory
  std::vector<Path> musics = getMusicIn(Path(Settings::Instance().MusicDirectory()));
  if (musics.empty())
  {
    //  Check in theme sound directory
    if (!themeSoundDirectory.Empty())
    {
      musics = getMusicIn(themeSoundDirectory);
      if (musics.empty()) { return nullptr; }
    }
    else { return nullptr; }
  }
  srand(time(nullptr) % getpid() + getppid());
  int randomIndex = rand() % musics.size();
  std::shared_ptr<Music> bgsound = Music::get(musics.at(randomIndex));
  return bgsound;
}

void AudioManager::musicEnd()
{
  LOG(LogInfo) << "MusicEnded";
  if (runningFromPlaylist && RecalboxConf::Instance().AsBool("audio.bgmusic"))
  {
    playRandomMusic();
  }
}

void AudioManager::playCheckSound()
{
  std::string selectedTheme = Settings::Instance().ThemeSet();
  Path loadingMusic = RootFolders::DataRootFolder / "/system/.emulationstation/themes/" / selectedTheme / "/fx/loading.ogg";

  if (!loadingMusic.Exists())
  {
    loadingMusic = RootFolders::TemplateRootFolder / "/system/.emulationstation/themes/recalbox/fx/loading.ogg";
  }

  if (loadingMusic.Exists())
  {
    Music::get(loadingMusic)->play(false, nullptr);
  }
}

void AudioManager::ReceiveSyncCallback(const SDL_Event& event)
{
  (void)event;
  int popupDuration = Settings::Instance().MusicPopupTime();
  std::shared_ptr<GuiInfoPopup> popup = std::make_shared<GuiInfoPopup>(mWindow,
                                                                       AudioManager::getInstance()->GetLastPopupText(),
                                                                       popupDuration, 10);
  mWindow->setInfoPopup(popup);
}
