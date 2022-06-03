#pragma once

#include <memory>

#include <guis/GuiInfoPopupBase.h>
#include <random>

#include "Sound.h"
#include "Music.h"

#include "utils/sdl2/SyncronousEvent.h"
#include "utils/sdl2/ISynchronousEvent.h"

class AudioManager : private ISynchronousEvent
{
  public:
    typedef unsigned long long AudioHandle;

  private:
    //! Music source folder
    static constexpr const char* sMusicFolder = "/recalbox/share/music/";

    //! Music source
    enum class MusicSource
    {
        None,        //!< Not selected yet
        ThemeSystem, //!< Theme music for the current system
        Theme,       //!< Theme musics
        User,        //!< User musics
        RemoteTrack, //!< Remote track
    };

    //! Max tries to find a different music to play
    static constexpr int sMaxTries = 8;

    //! Unique instance
    static AudioManager* sInstance;

    //! Path to Sound
    std::map<AudioHandle, Sound*> mSoundMap;
    //! Path to Music
    std::map<AudioHandle, Music*> mMusicMap;

    //! Window to attach popups to
    WindowManager& mWindow;

    //! Current playing music
    AudioHandle mCurrentMusic;
    //! Last popup text
    std::string mCurrentMusicTitle;
    //! Current music source
    MusicSource mCurrentMusicSource;

    //! Reserved SDL Event
    SyncronousEvent mSender;

    //! Random device to seed random generator
    std::random_device mRandomDevice;
    //! Random generator
    std::mt19937 mRandomGenerator;
    //! Random repartition (system)
    std::uniform_int_distribution<int> mSystemRandomizer;

    //! Theme music folder
    Path mThemeMusicFolder;
    //! Theme music
    Path mThemeMusic;

    /*!
     * @brief Synchronous event receiver
     * @param event Event
     */
    void ReceiveSyncCallback(const SDL_Event& event) override;

    /*!
     * @brief Initialize SDL audio
     */
    void Initialize();

    /*!
     * @brief Finalize SDL audio and free sound & music objects
     */
    void Finalize();

    /*!
     * @brief List music file from the given folder
     * @param path Path to folder
     * @return Music list, may be empty
     */
    static std::vector<Path> ListMusicInFolder(const Path& path);

    /*!
     * @brief Fetch a random music from the given folder
     * @param from source folder
     * @return Path to music
     */
    Path FetchRandomMusic(const Path& from, const Path& previousPath);

    /*!
     * @brief Play a new random music from theme/user music folder
     * @param allowTheSame Allow the same music to replay
     */
    void PlayRandomMusic();

  public:
    /*!
     * @brief Constructor - act as a singleton (Multiple instance is not possible)
     * That is, the application can control the lifecycle of this object
     */
    explicit AudioManager(WindowManager& window);

    /*!
     * @brief Singleton instance
     * @return Instance
     */
    static AudioManager& Instance();

    /*!
     * @brief Destructor
     */
    virtual ~AudioManager();

    /*!
     * @brief Send a signal to the AudioManager from another thread to instruct
     * current music has ended
     */
    void SignalEndOfMusic() { mSender.Call(); }

    /*!
     * @brief Load sound effect from the givenpath
     * @param path Sound file path
     * @return Non zero handle if the file has been successfully loaded
     */
    AudioHandle LoadSound(const Path& path);

    /*!
     * @brief Load music effect from the givenpath
     * @param path Music file path
     * @return Non zero handle if the file has been successfully loaded
     */
    AudioHandle LoadMusic(const Path& path);

    /*!
     * @brief Play sound
     * @param handle Sound handle to play
     * @return True if the handle is valid and the sound is playing
     */
    bool PlaySound(AudioHandle handle);

    /*!
     * @brief Play music
     * @param handle Music handle to play
     * @param loop True to play the music continuously
     * @return True if the handle is valid and the music is playing
     */
    bool PlayMusic(AudioHandle handle, bool loop);

    static void PauseMusic();
    static void ResumeMusic();
    /*!
     * @brief Stop all sounds and musics
     */
    void StopAll();

    /*!
     * @brief Start playing music from the theme or from the user playlist
     * @param theme Current theme
     */
    void StartPlaying(const ThemeData& theme);

    /*!
     * @brief Clear cached musics/sounds
     */
    void ClearCaches();

    /*!
     * @brief Deactivate the sound system to allow external application to run
     */
    void Deactivate()
    {
      Finalize();
    }

    /*!
     * @brief Reactivate the sound system if it has been previously deactivated.
     */
    void Reactivate();

    void PauseMusicIfNecessary();
    void ResumeMusicIfNecessary();
};
