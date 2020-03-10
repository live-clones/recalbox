//
// Created by Bkg2k on 10/03/2020.
//

#include <utils/storage/HashMap.h>
#include <utils/Files.h>
#include <systems/SystemData.h>
#include <systems/SystemManager.h>
#include <VideoEngine.h>
#include "NotificationManager.h"

Path NotificationManager::sStatusFilePath("/tmp/es_state.inf");

NotificationManager::NotificationManager()
  : StaticLifeCycleControler<NotificationManager>("NotificationManager")
{
  LoadScriptList();
}

const char* NotificationManager::ActionToString(Notification action)
{
  switch(action)
  {
    case Notification::None:                 return "none";
    case Notification::Start:                return "start";
    case Notification::Stop:                 return "stop";
    case Notification::Shutdown:             return "shutdown";
    case Notification::Reboot:               return "reboot";
    case Notification::Quit:                 return "quit";
    case Notification::Relaunch:             return "relaunch";
    case Notification::SystemBrowsing:       return "systembrowsing";
    case Notification::GamelistBrowsing:     return "gamelistbrowsing";
    case Notification::RunGame:              return "rungame";
    case Notification::RunDemo:              return "rundemo";
    case Notification::EndGame:              return "endgame";
    case Notification::EndDemo:              return "enddemo";
    case Notification::Sleep:                return "sleep";
    case Notification::WakeUp:               return "wakeup";
    case Notification::ThemeChanged:         return "themechanged";
    case Notification::ConfigurationChanged: return "configurationchanged";
    default: break;
  }
  return "error";
}

Notification NotificationManager::ActionFromString(const std::string& action)
{
  static HashMap<std::string, Notification> sStringToAction
  ({
    { "start"               , Notification::Start                },
    { "stop"                , Notification::Stop                 },
    { "shutdown"            , Notification::Shutdown             },
    { "reboot"              , Notification::Reboot               },
    { "relaunch"            , Notification::Relaunch             },
    { "quit"                , Notification::Quit                 },
    { "systembrowsing"      , Notification::SystemBrowsing       },
    { "gamelistbrowsing"    , Notification::GamelistBrowsing     },
    { "rungame"             , Notification::RunGame              },
    { "rundemo"             , Notification::RunDemo              },
    { "endgame"             , Notification::EndGame              },
    { "enddemo"             , Notification::EndDemo              },
    { "sleep"               , Notification::Sleep                },
    { "wakeup"              , Notification::WakeUp               },
    { "themechanged"        , Notification::ThemeChanged         },
    { "configurationchanged", Notification::ConfigurationChanged },
  });

  if (!sStringToAction.contains(action))
    return Notification::None;

  return sStringToAction[action];
}

void NotificationManager::LoadScriptList()
{
  Path scriptsFolder(sScriptPath);
  Path::PathList scripts = scriptsFolder.GetDirectoryContent();
  for(const Path& path : scripts)
    if (path.IsFile())
      mScriptList.push_back(path);
}

Path::PathList NotificationManager::FilteredScriptList(Notification action)
{
  Path::PathList list;

  for(const Path& scriptPath : mScriptList)
  {
    // Extract events between [ and ] in filename
    const std::string& scriptName = scriptPath.FilenameWithoutExtension();
    unsigned long start = scriptName.find('[');
    unsigned long stop = scriptName.find(']');
    if (((start | stop) != std::string::npos) && (stop - start > 1))
    {
      // Split events
      Strings::Vector events = Strings::Split(scriptName.substr(start + 1, stop - start - 1), ',');
      // If one event match our current action, this script has to be launched
      for(const std::string& event : events)
        if (ActionFromString(event) == action)
        {
          list.push_back(scriptPath);
          break;
        }
    }
    else list.push_back(scriptPath); // No filtering, just add
  }

  return list;
}

void NotificationManager::RunScripts(Notification action, const std::string& param)
{
  Path::PathList scripts = FilteredScriptList(action);
  if (scripts.empty()) return; // Nothing to launch

  // Build parameter
  std::string actionString(" -action ");
  actionString.append(ActionToString(action))
              .append(" -statefile \"")
              .append(sStatusFilePath.ToString())
              .append(1, '"');
  if (!param.empty())
  {
    actionString.append(" -param \"")
                .append(param)
                .append(1, '"');
  }

  for(const Path& script : scripts)
  {
    // Get command to execute
    std::string command(script.MakeEscaped());
    command.append(actionString);

    // Async?
    if ((script.FilenameWithoutExtension().find("(sync)") == std::string::npos)) command.append(" &");

    // Run!
    LOG(LogDebug) << "Run UserScript: " << command;
    system(command.c_str());
  }
}

void NotificationManager::Notify(const SystemData* system, const FileData* game, Notification action, const std::string& actionParameters)
{
  if (VideoEngine::IsInstantiated())
    VideoEngine::Instance().StopVideo();

  // Check if it is the same event than in previous call
  ParamBag newBag(system, game, action, actionParameters);
  if (newBag != mPreviousParamBag)
  {
    // Build status file
    std::string output("Version=2.0\r\n");
    output.append("Action=").append(ActionToString(action)).append("\r\n");
    output.append("ActionData=").append(actionParameters).append("\r\n");
    output.append("System=").append((system != nullptr) ? system->getFullName() : "").append("\r\n");
    output.append("SystemId=").append((system != nullptr) ? system->getName() : "").append("\r\n");
    output.append("Game=").append((game != nullptr) ? game->getName() : "").append("\r\n");
    output.append("GamePath=").append((game != nullptr) ? game->getPath().ToString() : "").append("\r\n");
    output.append("ImagePath=").append((game != nullptr) ? game->Metadata().Image().ToString() : "").append("\r\n");
    if (game != nullptr)
    {
      output.append("IsFolder=").append(game->isFolder() ? "1" : "0").append("\r\n");
      output.append("ThumbnailPath=").append(game->Metadata().Thumbnail().ToString()).append("\r\n");
      output.append("VideoPath=").append(game->Metadata().Video().ToString()).append("\r\n");
      output.append("Developer=").append(game->Metadata().Developer()).append("\r\n");
      output.append("Publisher=").append(game->Metadata().Publisher()).append("\r\n");
      output.append("Players=").append(game->Metadata().PlayersAsString()).append("\r\n");
      output.append("Region=").append(game->Metadata().RegionAsString()).append("\r\n");
      output.append("Genre=").append(game->Metadata().Genre()).append("\r\n");
      output.append("GenreId=").append(game->Metadata().GenreIdAsString()).append("\r\n");
      output.append("Favorite=").append(game->Metadata().Favorite() ? "1" : "0").append("\r\n");
      output.append("Hidden=").append(game->Metadata().Hidden() ? "1" : "0").append("\r\n");
      output.append("Adult=").append(game->Metadata().Adult() ? "1" : "0").append("\r\n");

      std::string emulator;
      std::string core;
      if (game->getSystem()->Manager().Emulators().GetGameEmulator(*game, emulator, core))
      {
        output.append("Emulator=").append(emulator).append("\r\n");
        output.append("Core=").append(core).append("\r\n");
      }
    }
    else if (system != nullptr && !system->IsVirtual())
    {
      std::string emulator;
      std::string core;
      if (system->Manager().Emulators().GetSystemDefaultEmulator(*system, emulator, core))
      {
        output.append("Emulator=").append(emulator).append("\r\n");
        output.append("Core=").append(core).append("\r\n");
      }
    }
    // Mimic old behavior of "State"
    if (action == Notification::SystemBrowsing ||
        action == Notification::GamelistBrowsing ||
        action == Notification::RunDemo ||
        action == Notification::RunGame)
      output.append("State=").append(action == Notification::RunGame ? "playing" : (action == Notification::RunDemo ? "demo" : "selected")).append("\r\n");
    // Save
    Files::SaveFile(Path(sStatusFilePath), output);

    // Run scripts
    RunScripts(action, actionParameters);
  }
  mPreviousParamBag = newBag;
}
