#ifndef _AUDIOMANAGER_H_
#define _AUDIOMANAGER_H_

#include <memory>

#include <guis/GuiInfoPopup.h>
#include <random>

#include "Sound.h"
#include "Music.h"

#include "Window.h"
#include "utils/sdl2/SyncronousEvent.h"
#include "utils/sdl2/ISyncronousEvent.h"

class AudioManager : private ISyncronousEvent
{
  public:
    typedef unsigned long long AudioHandle;

  private:
    //! Unique instance
    static AudioManager* sInstance;

    //! Path to Sound
    std::map<AudioHandle, Sound*> mSoundMap;
    //! Path to Music
    std::map<AudioHandle, Music*> mMusicMap;

    //! Window to attach popups to
    Window& mWindow;

    //! Current playing music
    AudioHandle mCurrentMusic;
    //! Last popup text
    std::string mCurrentMusicTitle;

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

    /*!
     * @brief Synchronous event receiver
     * @param event Event
     */
    void ReceiveSyncCallback(const SDL_Event& event) override;

    /*!
     * @brief Initialize SDL audio
     */
    static void Initialize();

    /*!
     * @brief Finalize SDL audio and free sound & music objects
     */
    static void Finalize();

    /*!
     * @brief List music file from the given folder
     * @param path Path to folder
     * @return Music list, may be empty
     */
    static std::vector<Path> ListMusicInFolder(const Path& path);

    /*!
     * @brief Fetch a random music from the given folder
     * @param themeMusicDirectory source folder
     * @return Music handle to a random music
     */
    AudioHandle FetchRandomMusic(const Path& themeMusicDirectory);

    /*!
     * @brief Play a new random music from theme/user music folder
     */
    void PlayRandomMusic();
  public:
    /*!
     * @brief Constructor - act as a singleton (Multiple instance is not possible)
     * That is, the application can control the lifecycle of this object
     */
    explicit AudioManager(Window& window);

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
     * @brief Load sound effect from tje given theme's view.element
     * @param theme Theme object
     * @param view View name
     * @param elem Element name
     * @return Non zero handle if the file has been successfully loaded
     */
    AudioHandle LoadSound(const ThemeData& theme, const std::string& view, const std::string& elem);

    /*!
     * @brief Load music effect from the givenpath
     * @param path Music file path
     * @return Non zero handle if the file has been successfully loaded
     */
    AudioHandle LoadMusic(const Path& path);

    /*!
     * @brief Load music effect from tje given theme's view.element
     * @param theme Theme object
     * @param view View name
     * @param elem Element name
     * @return Non zero handle if the file has been successfully loaded
     */
    AudioHandle LoadMusic(const ThemeData& theme, const std::string& view, const std::string& elem);

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
     * @brief Deactivate the sound system to allow external application to run
     */
    void Deactivate() { Finalize(); }

    /*!
     * @brief Reactivate the sound system if it has been previously deactivated.
     */
    void Reactivate();
};

#endif
