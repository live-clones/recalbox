#ifndef _AUDIOMANAGER_H_
#define _AUDIOMANAGER_H_

#include <vector>
#include <memory>

#include <guis/GuiInfoPopup.h>

#include "SDL_audio.h"

#include "Sound.h"
#include "Music.h"

#include "Window.h"


class AudioManager
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
    int mEvent;

  public:
    static std::shared_ptr<AudioManager>& getInstance();

    void stopMusic();

    void themeChanged(const std::shared_ptr<ThemeData>& theme);

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
     * @param event event identifier to use when pushing events
     */
    void SetMusicStartEvent(Window* window, int event) { mWindow = window; mEvent = event; }

    /*!
     * Called from the main thread to get the popup to display
     */
    std::string GetLastPopupText() { return mLastPopupText; }

  private:
    bool running;
    int lastTime = 0;

    std::shared_ptr<Music> getRandomMusic(std::string themeSoundDirectory);

    bool runningFromPlaylist;

    bool update(int curTime);

    std::string currentThemeMusicDirectory;

    void playRandomMusic();
};

#endif
