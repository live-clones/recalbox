#ifndef _AUDIOMANAGER_H_
#define _AUDIOMANAGER_H_

#include <vector>
#include <memory>

#include <guis/GuiInfoPopup.h>

#include "Sound.h"
#include "Music.h"

#include "Window.h"
#include "utils/sdl2/SyncronousEvent.h"
#include "utils/sdl2/ISyncronousEvent.h"


class AudioManager : private ISyncronousEvent
{
  private:
    static std::vector<std::shared_ptr<Sound>> sSoundVector;
    static std::vector<std::shared_ptr<Music>> sMusicVector;

    static std::shared_ptr<AudioManager> sInstance;
    std::shared_ptr<Music> currentMusic;

    AudioManager();

    //! Last popup text
    std::string mLastPopupText;

    //! Window to attach popups to
    Window* mWindow;

    //! Reserved SDL Event
    SyncronousEvent mSender;

    /*!
     * @brief Synchronous event receiver
     * @param event Event
     */
    void ReceiveSyncCallback(const SDL_Event& event) override;

  public:
    static std::shared_ptr<AudioManager>& getInstance();

    void stopMusic();

    void themeChanged(const ThemeData& theme);

    void resumeMusic();

    void playCheckSound();

    void init();

    void deinit();

    void registerMusic(std::shared_ptr<Music>& music);

    void registerSound(std::shared_ptr<Sound>& sound);

    void unregisterMusic(std::shared_ptr<Music>& music);

    void unregisterSound(std::shared_ptr<Sound>& sound);

    void play();

    void stop();

    void musicEnd();

    virtual ~AudioManager();

    inline std::string getCurrentMusicName() { return currentMusic->getName(); }

    /*!
     * Instruct this class to use this event identifier to push notifications to the main thread
     * @param window Window class to attach popups to
     */
    void SetMusicStartEvent(Window* window) { mWindow = window; }

    /*!
     * Called from the main thread to get the popup to display
     */
    std::string GetLastPopupText() { return mLastPopupText; }

  private:
    bool running;
    int lastTime = 0;

    static std::shared_ptr<Music> getRandomMusic(const Path& themeSoundDirectory);

    bool runningFromPlaylist;

    bool update(int curTime);

    Path currentThemeMusicDirectory;

    void playRandomMusic();
};

#endif
