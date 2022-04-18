#pragma once

#include <utils/os/system/Thread.h>
#include <utils/sdl2/ISynchronousEvent.h>
#include <utils/sdl2/SyncronousEvent.h>
#include <utils/os/system/Signal.h>
#include <utils/Files.h>

// Forward declaration
class WindowManager;

class Upgrade: private Thread, private ISynchronousEvent
{
  public:
    //! Local version file
    static constexpr const char* sLocalReleaseNoteFile = "/recalbox/recalbox.releasenotes";
    //! Local version file
    static constexpr const char* sLocalVersionFile = "/recalbox/recalbox.version";
    //! Local version arch
    static constexpr const char* sLocalArchFile = "/recalbox/recalbox.arch";
    //! Local version arch
    static constexpr const char* sLocalUUID = "/recalbox/share/system/uuid";

    /*!
     * @brief Constructor
     * @param window main Window
     */
    explicit Upgrade(WindowManager& window);

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
     * @brief Return trimmed current version.
     * @return Current version
     */
    static std::string CurrentVersion() { return Strings::Trim(Files::LoadFile(Path(sLocalVersionFile)), " \t\r\n"); }

    /*!
     * @brief Return remote releasenote
     * @return Remote release note
     */
    static std::string NewReleaseNote() { return mRemoteReleaseNote.empty() ? mLocalReleaseNote : mRemoteReleaseNote; }

    /*!
     * @brief Return remote version.
     * @return Remote version
     */
    static std::string ImageUrl();

    /*!
     * @brief Return remote version.
     * @return Remote version
     */
    static std::string HashUrl();

    /*!
     * @brief Check if there is a pending update
     */
    static bool PendingUpdate()
    {
      if (mRemoteVersion.empty()) return false;
      return mRemoteVersion != mLocalVersion;
    }

    /*!
     * @brief Check if the network is ready
     * @return True if the network is ready
     */
    static bool NetworkReady() { return !GetDomainName().empty(); }

  private:
    //! Release DNS
    static constexpr const char* sUpgradeDNS = ".download.recalbox.com";

    //! Get remote version template URL
    static constexpr const char* sVersionPatternUrl = "https://#DOMAIN#/latest/#ARCH#/recalbox.version?source=recalbox&uuid=#UUID#";
    //! Image template url
    static constexpr const char* sDownloadPatternUrl = "https://#DOMAIN#/latest/#ARCH#/recalbox-#ARCH#.img.xz#EXT#?source=recalbox&uuid=#UUID#";
    //! Releasenote template url
    static constexpr const char* sReleasenotePatternUrl = "https://#DOMAIN#/latest/#ARCH#/recalbox.releasenotes";

    //! MainWindow
    WindowManager& mWindow;
    //! Syncronous event to display popup
    SyncronousEvent mSender;
    //! Signal used to stop the thread
    Signal mSignal;
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
    //! Remote version
    static std::string mRemoteReleaseNote;
    //! Local version
    static std::string mLocalReleaseNote;

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

    /*!
     * @brief Get remote version
     */
    static std::string GetRemoteReleaseVersion();

    /*!
     * @brief Replace machine parameters parameters in the given url (Arch, uuid, domain, ...)
     * @param url Template url
     * @param ext Optional extention
     * @return Final url
     */
    static std::string ReplaceMachineParameters(const std::string& url, const std::string& ext);

    /*!
     * @brief Validate the given version
     * @param version Version to validate
     * @return True if the given version has been identified as valid, false otherwise
     */
    static bool ValidateVersion(const std::string& version);
};


