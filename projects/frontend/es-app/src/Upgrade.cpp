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
#include "recalbox/RecalboxUpgrade.h"
#include "RecalboxConf.h"
#include "utils/locale/LocaleHelper.h"

Upgrade::Upgrade(Window& window)
  : mWindow(window),
    mSender(this)
{
  GetUpdateUrl();
  Thread::Start("Upgrade");
  mSignal.Signal();
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
      std::string remoteVersion = GetRemoteVersion();
      std::string localVersion = Files::LoadFile(Path(sLocalVersionFile));
      if (!remoteVersion.empty() && (remoteVersion != localVersion))
      {
        LOG(LogInfo) << "[Update] Remote version " << remoteVersion << " does not match local version " << localVersion << ". Update available!";
      }

      /*if (RecalboxUpgrade::canUpdate())
      {
        std::string updateVersion = RecalboxUpgrade::getUpdateVersion();

        // Popup, always shown
        mPopupMessage = _("AN UPDATE IS AVAILABLE FOR YOUR RECALBOX");
        mPopupMessage += "\n";
        mPopupMessage += updateVersion;
        mPopupMessage += "\n\n";
        mPopupMessage += _("You're strongly recommended to update your Recalbox.\nNo support will be provided for older versions!");


        // Message box only if the option is on
        if (RecalboxConf::Instance().AsBool("updates.enabled"))
        {
          std::string changelog = RecalboxUpgrade::getUpdateChangelog();

          while (mWindow.HasGui())
            sleep(5);

          mMessageBoxMessage = _("NEW VERSION:");
          mMessageBoxMessage += " ";
          mMessageBoxMessage += updateVersion;
          if (!changelog.empty())
          {
            mMessageBoxMessage += "\n";
            mMessageBoxMessage += _("UPDATE CHANGELOG:");
            mMessageBoxMessage += "\n";
            mMessageBoxMessage += changelog;
          }
        }

        mSender.Call();
      }*/
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
  if (!mPopupMessage.empty())
    mWindow.AddInfoPopup(new GuiInfoPopup(mWindow, mPopupMessage, 60, GuiInfoPopup::PopupType::Recalbox));

  // Messagebox
  if (!mMessageBoxMessage.empty())
    mWindow.displayScrollMessage(_("AN UPDATE IS AVAILABLE FOR YOUR RECALBOX"), mMessageBoxMessage, false);
}

std::string Upgrade::GetUpdateUrl()
{
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
      return std::string((char*)(ns_rr_rdata(rr) + 1));
  }
  return std::string();
}

std::string Upgrade::GetRemoteVersion()
{
  // Get domain
  std::string domain = GetUpdateUrl();
  if (domain.empty()) return domain;
  LOG(LogDebug) << "[Update] Domain: " << domain;

  // Get version
  std::string url = Strings::Replace(sGetVersionUrl, "#DOMAIN#", domain);
  std::string version;
  if (!Http().Execute(url, version)) version.clear();
  LOG(LogDebug) << "[Update] Remote version: " << version << " (" << url << ')';

  // Return version
  return version;
}
