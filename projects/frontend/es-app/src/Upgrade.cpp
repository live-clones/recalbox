/* 
 * File:   Upgrade.cpp
 * Author: matthieu
 * 
 * Created on 6 février 2015, 11:40
 */

#include <guis/GuiInfoPopup.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <utils/Http.h>
#include <utils/Files.h>
#include "Upgrade.h"
#include "RecalboxConf.h"
#include "utils/locale/LocaleHelper.h"

std::string Upgrade::mDomainName;
std::string Upgrade::mRemoteVersion;
std::string Upgrade::mLocalVersion;


Upgrade::Upgrade(Window& window)
  : mWindow(window),
    mSender(this)
{
  Thread::Start("Upgrade");
}

Upgrade::~Upgrade()
{
  mSignal.Signal();
  Thread::Stop();
}

void Upgrade::Run()
{
  try
  {
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
      mLocalVersion = Files::LoadFile(Path(sLocalVersionFile));
      if (!mRemoteVersion.empty() && (mRemoteVersion != mLocalVersion))
      {
        LOG(LogInfo) << "[Update] Remote version " << mRemoteVersion << " does not match local version " << mLocalVersion << ". Update available!";

        // Popup, always shown
        mPopupMessage = _("AN UPDATE IS AVAILABLE FOR YOUR RECALBOX");
        mPopupMessage += "\n";
        mPopupMessage += mRemoteVersion;
        mPopupMessage += "\n\n";
        mPopupMessage += _("You're strongly recommended to update your Recalbox.\nNo support will be provided for older versions!");

        // Message box only if the option is on
        if (RecalboxConf::Instance().AsBool("updates.enabled"))
        {
          while (mWindow.HasGui())
            sleep(5);

          mMessageBoxMessage = _("NEW VERSION:");
          mMessageBoxMessage += " ";
          mMessageBoxMessage += mRemoteVersion;
        }

        mSender.Call();
      }
    }
  }
  catch(std::exception& ex)
  {
    LOG(LogError) << "Upgrade thread crashed.";
    LOG(LogError) << "Exception: " << ex.what();
  }
}

void Upgrade::ReceiveSyncCallback(const SDL_Event& event)
{
  (void)event;

  // Volatile popup
  mWindow.AddInfoPopup(new GuiInfoPopup(mWindow, mPopupMessage, 60, GuiInfoPopup::PopupType::Recalbox));

  // Messagebox
  if (!mMessageBoxMessage.empty())
    mWindow.displayScrollMessage(_("AN UPDATE IS AVAILABLE FOR YOUR RECALBOX"), mMessageBoxMessage, false);
}

std::string Upgrade::GetDomainName()
{
  if (!mDomainName.empty()) return mDomainName;

  // Select DNS to query
  std::string target = RecalboxConf::Instance().AsString("updates.type", "stable");
  std::string domain(target == "stable" ? sReleaseDNS :sReviewDNS);

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
      LOG(LogDebug) << "[Update] Domain: " << mDomainName;
    }
  }

  return mDomainName;
}

std::string Upgrade::GetRemoteVersion()
{
  // Get domain
  GetDomainName();

  // Get arch
  std::string arch = Files::LoadFile(Path(sLocalArchFile));
  Strings::ReplaceAllIn(arch, "odroidxu4", "xu4");

  // Get version
  std::string url = Strings::Replace(sVersionPatternUrl, "#DOMAIN#", mDomainName);
  Strings::ReplaceAllIn(url, "#ARCH#", arch);
  std::string version;
  if (!Http().Execute(url, version)) version.clear();
  version = Strings::Trim(version, " \t\r\n");
  LOG(LogDebug) << "[Update] Remote version: " << version << " (" << url << ')';

  // Return version
  return version;
}

std::string Upgrade::DownloadUrl()
{
  // Get domain
  GetDomainName();

  // Get arch
  std::string arch = Files::LoadFile(Path(sLocalArchFile));
  Strings::ReplaceAllIn(arch, "odroidxu4", "xu4");

  // Get url
  std::string url = Strings::Replace(sDownloadPatternUrl, "#DOMAIN#", mDomainName);
  Strings::ReplaceAllIn(url, "#ARCH#", arch);

  return url;
}
