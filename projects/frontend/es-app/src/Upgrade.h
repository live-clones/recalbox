#pragma once

#include <utils/os/system/Thread.h>
#include <utils/sdl2/ISynchronousEvent.h>
#include <utils/sdl2/SyncronousEvent.h>
#include <utils/os/system/Mutex.h>

class Upgrade: private Thread, private ISynchronousEvent
{
  public:
    /*!
     * @brief Constructor
     * @param window main Window
     */
    explicit Upgrade(Window& window);

    /*!
     * @brief Destructor
     */
    ~Upgrade() override;

  private:
    //! Release DNS
    static constexpr const char* sReleaseDNS = "stable.download.recalbox.com";
    //! Review DNS
    static constexpr const char* sReviewDNS = "review.download.recalbox.com";

    //! Get remote version template URL
    static constexpr const char* sGetVersionUrl = "https://#DOMAIN#/stable/v2/upgrade/rpi3/recalbox.version?source=recalbox";

    //! Local version file
    static constexpr const char* sLocalVersionFile = "/recalbox/recalbox.version";

    //! MainWindow
    Window& mWindow;
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

    /*!
     * @brief Get update url from DNS TXT records
     */
    static std::string GetUpdateUrl();

    /*!
     * @brief Get remote version
     */
    static std::string GetRemoteVersion();
};


