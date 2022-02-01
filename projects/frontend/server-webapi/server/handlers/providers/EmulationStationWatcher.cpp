//
// Created by bkg2k on 10/04/2020.
//

#include <utils/Log.h>
#include <server/handlers/MqttTopics.h>
#include "EmulationStationWatcher.h"

EmulationStationWatcher::EmulationStationWatcher()
  : mMqttClient("recalbox-api-server-watcher", this),
    mEventMethod
    ({
       { "start", &EmulationStationWatcher::EventStart },
       { "stop" , &EmulationStationWatcher::EventStop  },
       { "shutdown", &EmulationStationWatcher::EventShutdown },
       { "reboot", &EmulationStationWatcher::EventReboot },
       { "quit", &EmulationStationWatcher::EventQuit },
       { "relaunch", &EmulationStationWatcher::EventRelaunch },
       { "systembrowsing", &EmulationStationWatcher::EventSystemBrowsing },
       { "gamelistbrowsing", &EmulationStationWatcher::EventGamelistBrowsing },
       { "rungame", &EmulationStationWatcher::EventRunGame },
       { "rundemo", &EmulationStationWatcher::EventRunDemo },
       { "endgame", &EmulationStationWatcher::EventEndGame },
       { "enddemo", &EmulationStationWatcher::EventEndDemo },
       { "sleep", &EmulationStationWatcher::EventSleep },
       { "wakeup", &EmulationStationWatcher::EventWakup },
       { "scrapstart", &EmulationStationWatcher::EventScrapStart },
       { "scrapstop", &EmulationStationWatcher::EventScrapStop },
       { "scrapgame", &EmulationStationWatcher::EventScrapGame },
       { "configurationchanged", &EmulationStationWatcher::EventConfigurationChanged },
     })
{
  
  mMqttClient.Subscribe(MqttTopics::sEventTopic);
}

void EmulationStationWatcher::MqttMessageReceived(const String& topic, const String& payload)
{
  if (topic == MqttTopics::sEventTopic)
  {
    LOG(LogDebug) << "EmulationStation Event " << payload;

    SimpleMethodReference* reference = mEventMethod.try_get(payload);
    if (reference != nullptr)
    {
      static Path sPath(sEmulationStationEventFile);
      IniFile data(sPath, false);
      JSONBuilder result = (*reference)(data);
      mMqttClient.Send(MqttTopics::sEventObjectTopic, result);
    }
    else LOG(LogError) << "No processing available for " << payload;
  }
}

JSONBuilder EmulationStationWatcher::BuildGameObject(const IniFile& data)
{
  JSONBuilder result;
  result.Open()
        .Field("romPath", data.AsString("GamePath"))
        .Field("name", data.AsString("Game"))
        .Field("isFolder", data.AsBool("IsFolder"))
        .OpenObject("metadata")
        .CloseObject()
          .Field("developer", data.AsString("Developer"))
          .Field("publisher", data.AsString("Publisher"))
          .Field("players", data.AsString("Players"))
          .Field("region", data.AsString("Region"))
          .Field("genreName", data.AsString("Genre"))
          .Field("genreId", data.AsString("GenreId"))
          .Field("Favorite", data.AsBool("Favorite"))
          .Field("Hidden", data.AsBool("Hidden"))
          .Field("Adult", data.AsBool("Adult"))
        .OpenObject("media")
          .Field("image", data.AsString("ImagePath"))
          .Field("thumbnail", data.AsString("ThumbnailPath"))
          .Field("video", data.AsString("VideoPath"))
        .CloseObject()
        .Close();
  return result;
}

JSONBuilder EmulationStationWatcher::BuildSystemObject(const IniFile& data)
{
  JSONBuilder result;
  result.Open()
        .Field("name", data.AsString("SystemId"))
        .Field("fullname", data.AsString("System"))
        .Field("defaultEmulator", data.AsString("DefaultEmulator"))
        .Field("defaultCore", data.AsString("DefaultCore"))
        .Close();
  return result;
}

JSONBuilder EmulationStationWatcher::EventStart(const IniFile& data)
{
  JSONBuilder result(1024);
  result.Open()
        .Field("event", "start")
        .Field("param", data.AsString("ActionData"))
        .Close();
  return result;
}

JSONBuilder EmulationStationWatcher::EventStop(const IniFile& data)
{
  JSONBuilder result(1024);
  result.Open()
        .Field("event", "stop")
        .Field("param", data.AsString("ActionData"))
        .Close();
  return result;
}

JSONBuilder EmulationStationWatcher::EventShutdown(const IniFile& data)
{
  JSONBuilder result(1024);
  result.Open()
        .Field("event", "shutdown")
        .Field("param", data.AsString("ActionData"))
        .Close();
  return result;
}

JSONBuilder EmulationStationWatcher::EventReboot(const IniFile& data)
{
  JSONBuilder result(1024);
  result.Open()
        .Field("event", "reboot")
        .Field("param", data.AsString("ActionData"))
        .Close();
  return result;
}

JSONBuilder EmulationStationWatcher::EventQuit(const IniFile& data)
{
  JSONBuilder result(1024);
  result.Open()
        .Field("event", "quit")
        .Field("param", data.AsString("ActionData"))
        .Close();
  return result;
}

JSONBuilder EmulationStationWatcher::EventRelaunch(const IniFile& data)
{
  JSONBuilder result(1024);
  result.Open()
        .Field("event", "relaunch")
        .Field("param", data.AsString("ActionData"))
        .Close();
  return result;
}

JSONBuilder EmulationStationWatcher::EventSystemBrowsing(const IniFile& data)
{
  JSONBuilder result(4096);
  result.Open()
        .Field("event", "systembrowsing")
        .Field("param", data.AsString("ActionData"))
        .Field("system", BuildSystemObject(data))
        .Close();
  return result;
}

JSONBuilder EmulationStationWatcher::EventGamelistBrowsing(const IniFile& data)
{
  JSONBuilder result(4096);
  result.Open()
        .Field("event", "gamebrowsing")
        .Field("param", data.AsString("ActionData"))
        .Field("system", BuildSystemObject(data))
        .Field("game", BuildGameObject(data))
        .Close();
  return result;
}

JSONBuilder EmulationStationWatcher::EventRunGame(const IniFile& data)
{
  JSONBuilder result(4096);
  result.Open()
        .Field("event", "rungame")
        .Field("param", data.AsString("ActionData"))
        .Field("system", BuildSystemObject(data))
        .Field("game", BuildGameObject(data))
        .Close();
  return result;
}

JSONBuilder EmulationStationWatcher::EventRunDemo(const IniFile& data)
{
  JSONBuilder result(4096);
  result.Open()
        .Field("event", "rundemo")
        .Field("param", data.AsString("ActionData"))
        .Field("system", BuildSystemObject(data))
        .Field("game", BuildGameObject(data))
        .Close();
  return result;
}

JSONBuilder EmulationStationWatcher::EventEndGame(const IniFile& data)
{
  JSONBuilder result(4096);
  result.Open()
        .Field("event", "endgame")
        .Field("param", data.AsString("ActionData"))
        .Field("system", BuildSystemObject(data))
        .Field("game", BuildGameObject(data))
        .Close();
  return result;
}

JSONBuilder EmulationStationWatcher::EventEndDemo(const IniFile& data)
{
  JSONBuilder result(4096);
  result.Open()
        .Field("event", "enddemo")
        .Field("param", data.AsString("ActionData"))
        .Field("system", BuildSystemObject(data))
        .Field("game", BuildGameObject(data))
        .Close();
  return result;
}

JSONBuilder EmulationStationWatcher::EventSleep(const IniFile& data)
{
  JSONBuilder result(1024);
  result.Open()
        .Field("event", "sleep")
        .Field("param", data.AsString("ActionData"))
        .Close();
  return result;
}

JSONBuilder EmulationStationWatcher::EventWakup(const IniFile& data)
{
  JSONBuilder result(1024);
  result.Open()
        .Field("event", "wakeup")
        .Field("param", data.AsString("ActionData"))
        .Close();
  return result;
}

JSONBuilder EmulationStationWatcher::EventScrapStart(const IniFile& data)
{
  JSONBuilder result(1024);
  result.Open()
        .Field("event", "scrapstart")
        .Field("param", data.AsString("ActionData"))
        .Close();
  return result;
}

JSONBuilder EmulationStationWatcher::EventScrapStop(const IniFile& data)
{
  JSONBuilder result(1024);
  result.Open()
        .Field("event", "scrapstop")
        .Field("param", data.AsString("ActionData"))
        .Close();
  return result;
}

JSONBuilder EmulationStationWatcher::EventScrapGame(const IniFile& data)
{
  JSONBuilder result(4096);
  result.Open()
        .Field("event", "scrapgame")
        .Field("param", data.AsString("ActionData"))
        .Field("system", BuildSystemObject(data))
        .Field("game", BuildGameObject(data))
        .Close();
  return result;
}

JSONBuilder EmulationStationWatcher::EventConfigurationChanged(const IniFile& data)
{
  JSONBuilder result(1024);
  result.Open()
        .Field("event", "configurationchanged")
        .Field("param", data.AsString("ActionData"))
        .Close();
  return result;
}


