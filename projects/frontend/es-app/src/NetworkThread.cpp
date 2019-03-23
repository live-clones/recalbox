/* 
 * File:   NetworkThread.cpp
 * Author: matthieu
 * 
 * Created on 6 février 2015, 11:40
 */

#include <Log.h>
#include "NetworkThread.h"
#include "recalbox/RecalboxSystem.h"
#include "recalbox/RecalboxUpgrade.h"
#include "RecalboxConf.h"
#include "Locale.h"

NetworkThread::NetworkThread(Window* window)
  : mWindow(window)
{
  mRunning = true;
  mThreadHandle = new boost::thread(boost::bind(&NetworkThread::run, this));
}

NetworkThread::~NetworkThread()
{
  mRunning = false;
  mThreadHandle->interrupt();
}

void NetworkThread::run()
{
  try
  {
    if (RecalboxConf::getInstance()->get("updates.enabled") != "1")
      mRunning = false;

    int waitForSeconds = 15;
    while (mRunning)
    {
      boost::this_thread::sleep(boost::posix_time::seconds(waitForSeconds));
      waitForSeconds = 3600;

      if (RecalboxUpgrade::getInstance()->canUpdate())
      {
        std::string changelog = RecalboxUpgrade::getInstance()->getUpdateChangelog();

        while (mWindow->isShowingPopup())
        {
          boost::this_thread::sleep(boost::posix_time::seconds(5));
        }

        if (!changelog.empty())
        {
          std::string message = changelog;
          std::string updateVersion = RecalboxUpgrade::getInstance()->getUpdateVersion();
          mWindow->displayScrollMessage(_("AN UPDATE IS AVAILABLE FOR YOUR RECALBOX"),
                                        _("UPDATE VERSION:") + " " + updateVersion + "\n" + _("UPDATE CHANGELOG:") +
                                        "\n" + message);
        }
        else
        {
          mWindow->displayMessage(_("AN UPDATE IS AVAILABLE FOR YOUR RECALBOX"));
        }
      }
    }
  }
  catch(std::exception& ex)
  {
    LOG(LogError) << "Main thread crashed.";
    LOG(LogError) << "Exception: " << ex.what();
  }
}

