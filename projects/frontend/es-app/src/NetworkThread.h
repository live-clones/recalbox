#pragma once

#include <utils/os/system/Thread.h>
#include <utils/sdl2/ISyncronousEvent.h>
#include <utils/sdl2/SyncronousEvent.h>
#include <utils/os/system/Mutex.h>

class NetworkThread: private Thread, private ISyncronousEvent
{
  public:
    /*!
     * @brief Constructor
     * @param window main Window
     */
    explicit NetworkThread(Window * window);

    /*!
     * @brief Destructor
     */
    ~NetworkThread() override;

  private:
    //! MainWindow
    Window* mWindow;
    //! Syncronous event to display popup
    SyncronousEvent mSender;
    //! Signal used to stop the thread
    Mutex mSignal;
    //! Built popup message
    std::string mPopupMessage;
    //! Build MessageBox message
    std::string mMessageBoxMessage;

    /*
     * Thread implementation
     */

    /*!
     * Thread run method
     */
    void Run() override;

    /*
     * Synchronous event implementation
     */


    /*!
     * @brief Receive SDL event from the main thread
     * @param event SDL event
     */
    void ReceiveSyncCallback(const SDL_Event& event) override;
};


