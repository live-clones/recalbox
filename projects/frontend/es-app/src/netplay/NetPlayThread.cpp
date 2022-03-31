//
// Created by xizor on 06/06/18.
//

#include "NetPlayThread.h"
#include <guis/GuiInfoPopup.h>
#include <RecalboxConf.h>
#include <recalbox/RecalboxSystem.h>
#include <views/ViewController.h>
#include <utils/locale/LocaleHelper.h>
#include <utils/sdl2/SyncronousEventService.h>
#include <guis/GuiInfoPopupBase.h>
#include <rapidjson/document.h>
#include <emulators/run/GameRunner.h>

NetPlayThread::NetPlayThread(WindowManager&window)
  : mWindow(window),
    mSender(this)
{
  StartScan();
}

std::string NetPlayThread::getLobbyListCommand()
{
  static std::string command = "curl -s --connect-timeout 3 -m 3 -A libretro " + RecalboxConf::Instance().GetNetplayLobby();
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

    { LOG(LogInfo) << "[Netplay] NetPlayThread started"; }

    bool firstLoop = true;
    int popupDuration = RecalboxConf::Instance().GetPopupNetplay();
    if (popupDuration != 0)
    {
      std::vector<std::pair<std::string, std::string>> oldGames;
      std::vector<std::pair<std::string, std::string>> newGames;

      while (IsRunning())
      {
        bool enabled = RecalboxConf::Instance().GetNetplayEnabled();
        if (!GameRunner::IsGameRunning()) // Do not run while a game is running
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
        }

        // Sleep a bit
        if (Sleep(enabled))
          firstLoop = true;
      }
    }
  }
  catch(std::exception& ex)
  {
    { LOG(LogError) << "[Netplay] NetPlayThread thread crashed."; }
    { LOG(LogError) << "[Netplay] Exception: " << ex.what(); }
  }
}

void NetPlayThread::ReceiveSyncCallback(const SDL_Event& event)
{
  (void)event;
  int popupDuration = RecalboxConf::Instance().GetPopupNetplay();
  mWindow.InfoPopupAdd(new GuiInfoPopup(mWindow, mLastPopupText, popupDuration, GuiInfoPopupBase::PopupType::Netplay));
}

bool NetPlayThread::Sleep(bool& enabled)
{
  for (int i = 30; --i >= 0 && IsRunning();)
  {
    bool stillEnabled = RecalboxConf::Instance().GetNetplayEnabled();
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
  { LOG(LogInfo) << "[Netplay] NetPlayThread: Refresh lobby list"; }
  auto json_req = RecalboxSystem::execute(getLobbyListCommand());
  if (json_req.second == 0)
  {
    rapidjson::Document json;
    json.Parse(json_req.first.c_str());

    for (auto& item : json.GetArray())
    {
      const rapidjson::Value& fields = item["fields"];

      std::string player = fields["username"].GetString();
      std::string game   = fields["game_name"].GetString();
      if ((!filtered) ||
          (std::string(fields["frontend"].GetString()).find("@RECALBOX") != std::string::npos))
        list.push_back(std::make_pair(player, game));
    }
    return true;
  }
  return false;
}
