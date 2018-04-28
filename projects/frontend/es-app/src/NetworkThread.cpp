/* 
 * File:   NetworkThread.cpp
 * Author: matthieu
 * 
 * Created on 6 février 2015, 11:40
 */

#include "NetworkThread.h"
#include "recalbox/RecalboxSystem.h"
#include "recalbox/RecalboxUpgrade.h"
#include "RecalboxConf.h"
#include "Locale.h"

NetworkThread::NetworkThread(Window* window) : mWindow(window){
    mFirstRun = true;
    mRunning = true;
    mThreadHandle = new boost::thread(boost::bind(&NetworkThread::run, this));
}

NetworkThread::~NetworkThread() {
    mThreadHandle->join();
}

void NetworkThread::run(){
    while(mRunning){
        if(mFirstRun){
            boost::this_thread::sleep(boost::posix_time::seconds(15));
            mFirstRun = false;
        } else {
            boost::this_thread::sleep(boost::posix_time::hours(1));
        }

        if (RecalboxUpgrade::getInstance()->canUpdate()) {
            if(RecalboxConf::getInstance()->get("updates.enabled") == "1") {
                std::string changelog = RecalboxUpgrade::getInstance()->getUpdateChangelog();

                while (mWindow->isShowingPopup()) {
                    boost::this_thread::sleep(boost::posix_time::seconds(5));
                }

                if (changelog != "") {
                    std::string message = changelog;
                    std::string updateVersion = RecalboxUpgrade::getInstance()->getUpdateVersion();
                    mWindow->displayScrollMessage(_("AN UPDATE IS AVAILABLE FOR YOUR RECALBOX"),
                                                _("UPDATE VERSION:") + " " + updateVersion + "\n" +
                                                _("UPDATE CHANGELOG:") + "\n" + message);
                } else {
                    mWindow->displayMessage(_("AN UPDATE IS AVAILABLE FOR YOUR RECALBOX"));
                }
            }
            mRunning = false;
        }
    }
}

