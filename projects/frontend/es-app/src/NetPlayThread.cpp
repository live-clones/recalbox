//
// Created by xizor on 06/06/18.
//

#include "NetPlayThread.h"
#include <RecalboxConf.h>
#include <recalbox/RecalboxSystem.h>
#include <utils/Log.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <Settings.h>
#include <views/ViewController.h>
#include <Locale.h>
#include <utils/sdl2/SyncronousEventService.h>
#include <guis/GuiInfoPopup.h>

namespace json = boost::property_tree;

NetPlayThread::NetPlayThread(Window* window)
  : mWindow(window),
    mSender(this)
{
  StartScan();
}

std::string NetPlayThread::getLobbyListCommand()
{
  static std::string command = "curl -s --connect-timeout 3 -m 3 -A libretro " + RecalboxConf::Instance().AsString("global.netplay.lobby");
  return command;
}

NetPlayThread::~NetPlayThread()
{
  StopScan();
}

void NetPlayThread::StartScan()
{
  Thread::Start("NetplayThread");
}

void NetPlayThread::StopScan()
{
  Thread::Stop();
}

void NetPlayThread::Run()
{
  try
  {
    sleep(5);

    LOG(LogInfo) << "NetPlayThread started";

    bool firstLoop = true;
    bool enabled = RecalboxConf::Instance().AsBool("global.netplay");
    int popupDuration = Settings::getInstance()->getInt("NetplayPopupTime");
    if (popupDuration != 0)
    {
      std::vector<std::pair<std::string, std::string>> oldGames;
      std::vector<std::pair<std::string, std::string>> newGames;

      while (IsRunning())
      {
        if (firstLoop && enabled)
        {
          RefreshNetplayList(oldGames, true);
          firstLoop = false;
        }

        if (enabled)
          if (RefreshNetplayList(newGames, true))
          {
            for (const auto& tmp : newGames)
            {
              auto it = std::find(oldGames.begin(), oldGames.end(), tmp);
              if (it == oldGames.end())
              {
                PopupTriggered(tmp.first, tmp.second);
                break;
              }
            }
            oldGames = newGames;
          }

        // Sleep a bit
        if (Sleep(enabled))
          firstLoop = true;
      }
    }
  }
  catch(std::exception& ex)
  {
    LOG(LogError) << "NetPlayThread thread crashed.";
    LOG(LogError) << "Exception: " << ex.what();
  }
}

void NetPlayThread::ReceiveSyncCallback(const SDL_Event& event)
{
  (void)event;
  int popupDuration = Settings::getInstance()->getInt("NetplayPopupTime");
  std::shared_ptr<GuiInfoPopup> popup = std::make_shared<GuiInfoPopup>(mWindow,
                                                                       mLastPopupText,
                                                                       popupDuration, 10);
  mWindow->setInfoPopup(popup);
}

bool NetPlayThread::Sleep(bool& enabled)
{
  for (int i = 30; --i >= 0 && IsRunning();)
  {
    bool stillEnabled = RecalboxConf::Instance().AsBool("global.netplay");
    if (enabled != stillEnabled)
    {
      enabled = stillEnabled;
      return true;
    }
    sleep(1);
  }
  return false;
}

void NetPlayThread::PopupTriggered(const std::string& player, const std::string& game)
{
  // Create popup text
  mLastPopupText = _("A Recalbox friend has started a Netplay game!") + "\n " + _("Player") + ": " +
                   player + "\n " + _("Game") + ": " + game;

  // Push event to the main thread so that all GUI operations are safely run in the main thread.
  // SDL_PushEvent is synchronized & thread-safe
  mSender.Call();

}

bool NetPlayThread::RefreshNetplayList(PlayerGameList& list, bool filtered)
{
  list.clear();
  LOG(LogInfo) << "NetPlayThread: Refresh lobby list";
  auto json_req = RecalboxSystem::execute(getLobbyListCommand());
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
      return false;
    }

    for (json::ptree::value_type& array_element : root)
    {
      std::string player = array_element.second.get<std::string>("fields.username");
      std::string game = array_element.second.get<std::string>("fields.game_name");
      if (!filtered)
        list.push_back(std::make_pair(player, game));
      else if (array_element.second.get<std::string>("fields.frontend").find("@RECALBOX") != std::string::npos)
        list.push_back(std::make_pair(player, game));
    }
    return true;
  }
  return false;
}
