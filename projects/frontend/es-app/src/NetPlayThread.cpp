//
// Created by xizor on 06/06/18.
//

#include "NetPlayThread.h"
#include <RecalboxConf.h>
#include <recalbox/RecalboxSystem.h>
#include <Log.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <Settings.h>
#include <views/ViewController.h>
#include <Locale.h>

namespace json = boost::property_tree;

NetPlayThread::NetPlayThread(Window* window, int event)
  : mWindow(window),
    mRunning(false),
    mThreadHandle(nullptr),
    mEvent(event),
    mLobby(RecalboxConf::getInstance()->get("global.netplay.lobby"))
{
}

NetPlayThread::~NetPlayThread()
{
  Stop();
}

void NetPlayThread::Start()
{
  mRunning = true;
  mThreadHandle = new boost::thread(boost::bind(&NetPlayThread::run, this));
}

void NetPlayThread::Stop()
{
  if (mRunning)
  {
    mRunning = false;
    mThreadHandle->join();
  }
}

void NetPlayThread::run()
{
  try
  {
    boost::this_thread::sleep(boost::posix_time::seconds(5));

    LOG(LogInfo) << "NetPlayThread started";

    int popupDuration = Settings::getInstance()->getInt("NetplayPopupTime");
    if (popupDuration != 0)
    {
      std::vector<std::pair<std::string, std::string>> oldGames;
      std::string lobby = RecalboxConf::getInstance()->get("global.netplay.lobby");

      auto json_req = RecalboxSystem::getInstance()->execute("curl -s --connect-timeout 3 -m 3 " + lobby);
      if (json_req.second == 0)
      {
        json::ptree root;
        std::stringstream ss;
        ss << json_req.first;
        try
        {
          json::read_json(ss, root);
        }
        catch (const boost::property_tree::json_parser_error& e1)
        {
          LOG(LogInfo) << "NetPlayThread error";
        }

        for (json::ptree::value_type& array_element : root)
        {
          std::string player = array_element.second.get<std::string>("fields.username");
          std::string game = array_element.second.get<std::string>("fields.game_name");
          oldGames.push_back(std::make_pair(player, game));
        }
      }

      while (mRunning)
      {
        json_req = RecalboxSystem::getInstance()->execute("curl -s --connect-timeout 3 -m 3 " + mLobby);
        if (json_req.second == 0)
        {
          json::ptree root;
          std::stringstream ss;
          ss << json_req.first;
          try
          {
            json::read_json(ss, root);
          }
          catch (const boost::property_tree::json_parser_error& e1)
          {
            LOG(LogInfo) << "NetPlayThread error";
          }

          std::vector<std::pair<std::string, std::string>> newGames;

          for (json::ptree::value_type& array_element : root)
          {
            std::string player = array_element.second.get<std::string>("fields.username");
            std::string game = array_element.second.get<std::string>("fields.game_name");
            newGames.push_back(std::make_pair(player, game));
          }
          for (auto tmp : newGames)
          {
            std::vector<std::pair<std::string, std::string>>::const_iterator it;
            it = std::find(oldGames.begin(), oldGames.end(), tmp);
            if (it == oldGames.end())
            {
              if (tmp.first.find("RECALBOX") != std::string::npos)
              {
                // Create popup text
                mLastPopupText = _("A Recalbox friend has started a Netplay game!") + "\n " + _("Player") + ": " +
                                 tmp.first;
                +"\n " + _("Game") + ": " + tmp.second;

                // Create event
                SDL_Event event;
                event.user.type = (unsigned int) mEvent;

                // Push event to the main thread so that all GUI operations are safely run in the main thread.
                // SDL_PushEvent is synchronized & thread-safe
                SDL_PushEvent(&event);

                break;
              }
            }
          }
          oldGames = newGames;
        }

        for (int i = 30; --i >= 0 && mRunning;)
          boost::this_thread::sleep(boost::posix_time::seconds(1));
      }
    }
  }
  catch(std::exception& ex)
  {
    LOG(LogError) << "Main thread crashed.";
    LOG(LogError) << "Exception: " << ex.what();
  }
}

std::string NetPlayThread::GetLastPopupText()
{
  return mLastPopupText;
}
