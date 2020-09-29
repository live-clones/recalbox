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

    /*!
     * @brief Return remote version.
     * @return Remote version
     */
    static std::string NewVersion() { return mRemoteVersion.empty() ? mLocalVersion : mRemoteVersion; }

    /*!
     * @brief Return remote version.
     * @return Remote version
     */
    static std::string DownloadUrl();

    /*!
     * @brief Check if there is a pending update
     */
    static bool PendingUpdate()
    {
      if (mRemoteVersion.empty()) return false;
      return mRemoteVersion != mLocalVersion;
    }

  private:
    //! Release DNS
    static constexpr const char* sReleaseDNS = "stable.download.recalbox.com";
    //! Review DNS
    static constexpr const char* sReviewDNS = "review.download.recalbox.com";

    //! Get remote version template URL
    static constexpr const char* sVersionPatternUrl = "https://#DOMAIN#/latest/#ARCH#/recalbox.version?source=recalbox";
    //! Get file download template url
    static constexpr const char* sDownloadPatternUrl = "https://#DOMAIN#/latest/#ARCH#/recalbox-#ARCH#.img.xz?source=recalbox";

    //! Local version file
    static constexpr const char* sLocalVersionFile = "/recalbox/recalbox.version";
    //! Local version arch
    static constexpr const char* sLocalArchFile = "/recalbox/recalbox.arch";

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
    //! Download URL
    static std::string mDomainName;
    //! Remote version
    static std::string mRemoteVersion;
    //! Local version
    static std::string mLocalVersion;

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
    static std::string GetDomainName();

    /*!
     * @brief Get remote version
     */
    static std::string GetRemoteVersion();
};


