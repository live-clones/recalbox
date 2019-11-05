/* 
 * File:   NetworkThread.cpp
 * Author: matthieu
 * 
 * Created on 6 février 2015, 11:40
 */

#include <utils/Log.h>
#include <guis/GuiInfoPopup.h>
#include <utils/sdl2/SyncronousEventService.h>
#include "NetworkThread.h"
#include "recalbox/RecalboxUpgrade.h"
#include "RecalboxConf.h"
#include "Locale.h"

NetworkThread::NetworkThread(Window* window)
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
        mPopupMessage = _("UPDATE VERSION:");
        mPopupMessage += " ";
        mPopupMessage += updateVersion;

        // Message box only if the option is on
        if (RecalboxConf::Instance().AsBool("updates.enabled"))
        {
          std::string changelog = RecalboxUpgrade::getUpdateChangelog();

          while (mWindow->isShowingPopup())
            sleep(5);

          mMessageBoxMessage = _("UPDATE VERSION:");
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
  {
    std::shared_ptr<GuiInfoPopup> popup = std::make_shared<GuiInfoPopup>(mWindow, mPopupMessage, 30, 10);
    mWindow->setInfoPopup(popup);
  }

  // Messagebox
  if (mMessageBoxMessage.empty())
    mWindow->displayScrollMessage(_("AN UPDATE IS AVAILABLE FOR YOUR RECALBOX"), mMessageBoxMessage);
}
