/* 
 * File:   Upgrade.cpp
 * Author: matthieu
 * 
 * Created on 6 février 2015, 11:40
 */

#include <guis/GuiInfoPopupBase.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include "utils/network/Http.h"
#include <utils/Files.h>
#include <algorithm>
#include "Upgrade.h"
#include "RecalboxConf.h"
#include "utils/locale/LocaleHelper.h"
#include "recalbox/RecalboxSystem.h"
#include <patreon/PatronInfo.h>
#include <guis/GuiInfoPopup.h>

std::string Upgrade::mDomainName;
std::string Upgrade::mRemoteVersion;
std::string Upgrade::mLocalVersion;
std::string Upgrade::mRemoteReleaseNote;
std::string Upgrade::mLocalReleaseNote;

Upgrade::Upgrade(WindowManager& window)
  : mWindow(window),
    mSender(this)
{
  Thread::Start("Upgrade");
}

Upgrade::~Upgrade()
{
  mSignal.Fire();
  Thread::Stop();
}

void Upgrade::Run()
{
  try
  {
    // Wait for patron response
    PatronInfo::Instance().WaitForAuthentication(*this);

    // First check at 15s
    int waitForSeconds = 15;
    while (IsRunning())
    {
      if (mSignal.WaitSignal(waitForSeconds * 1000LL))
        break;

      // Next checks, once an hour
      waitForSeconds = 3600;

      // Do we have to update?
      mRemoteVersion = GetRemoteVersion();
      mLocalVersion = Strings::Trim(Files::LoadFile(Path(sLocalVersionFile)), " \t\r\n");
      mRemoteReleaseNote = GetRemoteReleaseVersion();
      mLocalReleaseNote = Strings::Trim(Files::LoadFile(Path(sLocalReleaseNoteFile)), " \t\r\n");

      if (ValidateVersion(mRemoteVersion))
      {
        if (mRemoteVersion != mLocalVersion)
        {
          { LOG(LogInfo) << "[Update] Remote version " << mRemoteVersion << " does not match local version " << mLocalVersion << ". Update available!"; }

          // Popup, always shown
          mPopupMessage = _("AN UPDATE IS AVAILABLE FOR YOUR RECALBOX");
          mPopupMessage += "\n";
          mPopupMessage += mRemoteVersion;
          mPopupMessage += "\n\n";
          mPopupMessage += _("You're strongly recommended to update your Recalbox.\nNo support will be provided for older versions!");

          // Message box only if the option is on
          if (RecalboxConf::Instance().AsBool("updates.enabled"))
          {
            while (mWindow.HasGui() || mWindow.isSleeping())
              Thread::Sleep(5000);

            mMessageBoxMessage = _("NEW VERSION:");
            mMessageBoxMessage += " ";
            mMessageBoxMessage += mRemoteVersion;

            if (!mRemoteReleaseNote.empty())
            {
              mMessageBoxMessage += "\n\n";
              mMessageBoxMessage += _("CHANGELOG");
              mMessageBoxMessage += ":\n\n";
              mMessageBoxMessage += mRemoteReleaseNote;
            }
          }

          mSender.Call();
        }
        else { LOG(LogInfo) << "[Update] Remote version match local version. No update."; }
      }
      else { LOG(LogError) << "[Update] Invalid remote version! " << mRemoteVersion; }
    }
  }
  catch(std::exception& ex)
  {
    { LOG(LogError) << "[Update] Upgrade thread crashed."; }
    { LOG(LogError) << "[Update] Exception: " << ex.what(); }
  }
}

void Upgrade::ReceiveSyncCallback(const SDL_Event& event)
{
  (void)event;

  // Volatile popup
  mWindow.InfoPopupAdd(new GuiInfoPopup(mWindow, mPopupMessage, 60, GuiInfoPopupBase::PopupType::Recalbox));

  // Messagebox
  if (!mMessageBoxMessage.empty())
    mWindow.displayScrollMessage(_("AN UPDATE IS AVAILABLE FOR YOUR RECALBOX"), mMessageBoxMessage, false);
}

std::string Upgrade::GetDomainName()
{
  if (!mDomainName.empty()) return mDomainName;

  // Select DNS to query
  std::string target = Strings::ToLowerASCII(RecalboxConf::Instance().GetUpdatesType());
  if (target == "patron" && PatronInfo::Instance().IsPatron())
    target = "stable";
  Strings::ReplaceAllIn(target, ' ', "", 0);
  std::string domain(target + sUpgradeDNS);

  { LOG(LogDebug) << "[Update] updates.type implied dns to use: " << domain; }

  // Query TXT
  unsigned char buffer[4096];
  int l = res_query(domain.c_str(), C_IN, ns_t_txt, buffer, sizeof (buffer));
  if (l >= 0)
  {
    ns_msg msg;
    ns_initparse(buffer, l, &msg);
    ns_rr rr;
    if (ns_parserr(&msg, ns_s_an, 0, &rr) == 0)
    {
      mDomainName = std::string((char*) (ns_rr_rdata(rr) + 1), (int)ns_rr_rdata(rr)[0]);
      { LOG(LogDebug) << "[Update] Domain: " << mDomainName; }
    }
  }

  return mDomainName;
}

std::string Upgrade::GetRemoteVersion()
{
  // Get version
  std::string url = ReplaceMachineParameters(sVersionPatternUrl, Strings::Empty);

  std::string version;
  if (!Http().Execute(url, version)) version.clear();
  version = Strings::Trim(version, " \t\r\n");
  { LOG(LogDebug) << "[Update] Remote version: " << version << " (" << url << ')'; }

  // Return version
  return version;
}

std::string Upgrade::ImageUrl()
{
  // Get url
  std::string url = ReplaceMachineParameters(sDownloadPatternUrl, Strings::Empty);
  { LOG(LogDebug) << "[Update] Image file url: " << url; }

  return url;
}

std::string Upgrade::HashUrl()
{
  // Get url
  std::string url = ReplaceMachineParameters(sDownloadPatternUrl, ".sha1");
  { LOG(LogDebug) << "[Update] Hash file url: " << url; }

  return url;
}

std::string Upgrade::ReplaceMachineParameters(const std::string& url, const std::string& ext)
{
  std::string result(url);

  // Get domain
  GetDomainName();

  // Get arch
  std::string arch = Files::LoadFile(Path(sLocalArchFile));
  if (arch == "xu4") arch = "odroidxu4";

  // Get uuid
  std::string uuid = Strings::Trim(Files::LoadFile(Path(sLocalUUID)), " \t\r\n");
  if (uuid.empty()) uuid = "";

  // Replacements
  Strings::ReplaceAllIn(result, "#DOMAIN#", mDomainName);
  Strings::ReplaceAllIn(result, "#ARCH#", arch);
  Strings::ReplaceAllIn(result, "#UUID#", uuid);
  Strings::ReplaceAllIn(result, "#EXT#", ext);

  return result;
}

bool Upgrade::ValidateVersion(const std::string& version)
{
  static std::string _allowedCharacters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.- ()/";
  if (!version.empty())
    if (version[0] >= '0' && version[0] <= '9')
      return (version.find_first_not_of(_allowedCharacters) == std::string::npos);

  return false;
}

std::string Upgrade::GetRemoteReleaseVersion()
{
  // Get version
  std::string url = ReplaceMachineParameters(sReleasenotePatternUrl, Strings::Empty);

  std::string releaseNote;
  if (!Http().Execute(url, releaseNote)) releaseNote.clear();

  { LOG(LogDebug) << "[Update] Remote release note: " << releaseNote << " (" << url << ')'; }

  if (Strings::StartsWith(releaseNote, "<", 1)) releaseNote.clear();
  else releaseNote = Strings::Trim(releaseNote, " \t\r\n");

  // Return version
  return releaseNote;
}
