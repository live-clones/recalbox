/* 
 * File:   NetworkThread.cpp
 * Author: matthieu
 * 
 * Created on 6 février 2015, 11:40
 */

#include <guis/GuiInfoPopup.h>
#include "NetworkThread.h"
#include "recalbox/RecalboxUpgrade.h"
#include "RecalboxConf.h"
#include "utils/locale/LocaleHelper.h"

NetworkThread::NetworkThread(Window&window)
  : mWindow(window),
    mSender(this)
{
  Thread::Start("NetworkThread");
  mSignal.Signal();
}

NetworkThread::~NetworkThread()
{
  mSignal.Signal();
  Thread::Stop();
}

void NetworkThread::Run()
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

      if (RecalboxUpgrade::canUpdate())
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
      }
    }
  }
  catch(std::exception& ex)
  {
    LOG(LogError) << "NetworkThread thread crashed.";
    LOG(LogError) << "Exception: " << ex.what();
  }
}

void NetworkThread::ReceiveSyncCallback(const SDL_Event& event)
{
  (void)event;

  // Volatile popup
  if (!mPopupMessage.empty())
    mWindow.AddInfoPopup(new GuiInfoPopup(mWindow, mPopupMessage, 120, GuiInfoPopup::Icon::Recalbox));

  // Messagebox
  if (!mMessageBoxMessage.empty())
    mWindow.displayScrollMessage(_("AN UPDATE IS AVAILABLE FOR YOUR RECALBOX"), mMessageBoxMessage, false);
}
