//
// Created by xizor on 06/06/18.
//

#ifndef EMULATIONSTATION_ALL_NETPLAYTHREAD_H
#define EMULATIONSTATION_ALL_NETPLAYTHREAD_H

#include <Window.h>
#include <guis/GuiInfoPopup.h>
#include <boost/thread/thread.hpp>

class NetPlayThread
{
  public:
    NetPlayThread(Window* window, int event);

    ~NetPlayThread();

    /*!
     * Start the thread. Initialize the thread handle and set the running glaf to true
     */
    void Start();
    /*!
     * Stop the thread & wait till it's actually stopped. Reset the running flag
     */
    void Stop();

    /*!
     * Called from the main thread to get the popup to display
     */
    std::string GetLastPopupText();

  private:
    //! Attached wndow
    Window* mWindow;

    //! Running flag. True when Start() is called
    bool mRunning;

    //! Thread handle. Null until Start() is called
    boost::thread* mThreadHandle;

    //! Reserved SDL Event
    int mEvent;

    //! Lobby server
    std::string mLobby;

    //! Last created popup
    std::string mLastPopupText;

    //! Main thread runner
    void run();
};

#endif //EMULATIONSTATION_ALL_NETPLAYTHREAD_H
