#pragma once

#include <utils/os/system/Thread.h>
#include <utils/sync/SyncMessageSender.h>
#include <utils/os/system/Signal.h>
#include <utils/Files.h>

// Forward declaration
class WindowManager;

class Upgrade: private Thread
             , private ISyncMessageReceiver<void>
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
    static String NewVersion() { return mRemoteVersion.empty() ? mLocalVersion : mRemoteVersion; }

    /*!
     * @brief Return trimmed current version.
     * @return Current version
     */
    static String CurrentVersion() { return Strings::Trim(Files::LoadFile(Path(sLocalVersionFile)), " \t\r\n"); }

    /*!
     * @brief Return trimmed current version.
     * @return Current version
     */
    static String CurrentArch() { return Strings::Replace(Strings::Trim(Files::LoadFile(Path(sLocalArchFile)), " \t\r\n"), '_', '/'); }

    /*!
     * @brief Return remote releasenote
     * @return Remote release note
     */
    static String NewReleaseNote() { return mRemoteReleaseNote.empty() ? mLocalReleaseNote : mRemoteReleaseNote; }

    /*!
     * @brief Return remote version.
     * @return Remote version
     */
    static String ImageUrl();

    /*!
     * @brief Return remote version.
     * @return Remote version
     */
    static String HashUrl();

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
    SyncMessageSender<void> mSender;
    //! Signal used to stop the thread
    Signal mSignal;
    //! Built popup message
    String mPopupMessage;
    //! Build MessageBox message
    String mMessageBoxMessage;
    //! Download URL
    static String mDomainName;
    //! Remote version
    static String mRemoteVersion;
    //! Local version
    static String mLocalVersion;
    //! Remote version
    static String mRemoteReleaseNote;
    //! Local version
    static String mLocalReleaseNote;

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
     * @brief Receive synchronous message
     */
    void ReceiveSyncMessage() override;

    /*!
     * @brief Get update url from DNS TXT records
     */
    static String GetDomainName();

    /*!
     * @brief Get remote version
     */
    static String GetRemoteVersion();

    /*!
     * @brief Get remote version
     */
    static String GetRemoteReleaseVersion();

    /*!
     * @brief Replace machine parameters parameters in the given url (Arch, uuid, domain, ...)
     * @param url Template url
     * @param ext Optional extention
     * @return Final url
     */
    static String ReplaceMachineParameters(const String& url, const String& ext);

    /*!
     * @brief Validate the given version
     * @param version Version to validate
     * @return True if the given version has been identified as valid, false otherwise
     */
    static bool ValidateVersion(const String& version);
};


