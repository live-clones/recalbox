#include "SystemData.h"
#include <systems/SystemManager.h>
#include "audio/AudioManager.h"
#include "VolumeControl.h"
#include "utils/Log.h"
#include "NetPlayData.h"
#include <RecalboxConf.h>
#include <RootFolders.h>
#include <recalbox/RecalboxSystem.h>
#include <usernotifications/NotificationManager.h>
#include <VideoEngine.h>
#include <utils/Strings.h>
#include <utils/Files.h>
#include <Settings.h>
#include <themes/ThemeException.h>
#include <padtokeyboard/PadToKeyboardManager.h>
#include <utils/Zip.h>

SystemData::SystemData(SystemManager& systemManager, const SystemDescriptor& descriptor, RootFolderData::Ownership childOwnership, Properties properties, FileSorts::Sorts fixedSort)
  : mSystemManager(systemManager),
    mDescriptor(descriptor),
    mRootFolder(childOwnership, descriptor.RomPath(), this),
    mSortId(RecalboxConf::Instance().AsInt(mDescriptor.Name() + ".sort")),
    mProperties(properties),
    mFixedSort(fixedSort)
{
  // Set name
  mRootFolder.Metadata().SetName(mDescriptor.FullName());
}

void SystemData::OverrideCommand(const Path& romPath, std::string& command)
{
  Path parent = romPath.Directory();
  if (parent != romPath)
    OverrideCommand(parent, command);
  
  // Subsystem override
  Path cfg = romPath.IsFile() ? romPath.ChangeExtension(romPath.Extension() + ".system.cfg") : romPath / ".system.cfg";
  if (cfg.Exists())
  {
    IniFile subSystem(cfg);
    command = subSystem.AsString("command", command);
  }
}

void SystemData::RunGame(Window& window,
                         SystemManager& systemManager,
                         FileData& game,
                         const EmulatorData& emulator,
                         const NetPlayData& netplay)
{
  LOG(LogInfo) << "Attempting to launch game...";

  OrderedDevices controllers = InputManager::Instance().GenerateConfiguration();
  std::string controlersConfig = InputManager::GenerateConfiggenConfiguration(controllers);
  LOG(LogInfo) << "Controllers config : " << controlersConfig;

  VideoEngine::Instance().StopVideo();
  AudioManager::Instance().Deactivate();
  VolumeControl::getInstance()->deinit();
  Window::Finalize();

  std::string command = mDescriptor.Command();

  const std::string rom = game.getPath().MakeEscaped();
  const std::string basename = game.getPath().FilenameWithoutExtension();
  const std::string rom_raw = game.getPath().ToString();

  Strings::ReplaceAllIn(command, "%ROM%", rom);
  Strings::ReplaceAllIn(command, "%CONTROLLERSCONFIG%", controlersConfig);
  Strings::ReplaceAllIn(command, "%SYSTEM%", game.getSystem()->getName());
  Strings::ReplaceAllIn(command, "%BASENAME%", basename);
  Strings::ReplaceAllIn(command, "%ROM_RAW%", rom_raw);
  Strings::ReplaceAllIn(command, "%EMULATOR%", emulator.Emulator());
  Strings::ReplaceAllIn(command, "%CORE%", emulator.Core());
  Strings::ReplaceAllIn(command, "%RATIO%", game.Metadata().RatioAsString());

  switch(netplay.NetplayMode())
  {
    case NetPlayData::Mode::None:
    {
      Strings::ReplaceAllIn(command, "%NETPLAY%", "");
      break;
    }
    case NetPlayData::Mode::Client:
    {
      std::string netplayLine("-netplay client -netplay_port ");
      netplayLine.append(Strings::ToString(netplay.Port())).append(" -netplay_ip ").append(netplay.Ip());
      if (game.Metadata().RomCrc32() != 0) netplayLine.append(" -hash ").append(game.Metadata().RomCrc32AsString());
      Strings::ReplaceAllIn(command, "%NETPLAY%", netplayLine);
      break;
    }
    case NetPlayData::Mode::Server:
    {
      std::string netplayLine("-netplay host -netplay_port ");
      netplayLine.append(Strings::ToString(netplay.Port()));
      if (game.Metadata().RomCrc32() != 0) netplayLine.append(" -hash ").append(game.Metadata().RomCrc32AsString());
      Strings::ReplaceAllIn(command, "%NETPLAY%", netplayLine);
      break;
    }
  }

  LOG(LogInfo) << "	" << command;
  {
    PadToKeyboardManager padToKeyboard(controllers, game.getPath());
    padToKeyboard.StartMapping();
    if (padToKeyboard.IsValid())
      command.append(" -nodefaultkeymap");
    NotificationManager::Instance().Notify(game, Notification::RunGame);

    bool debug = RecalboxConf::Instance().AsBool("emulationstation.debuglogs");
    if (debug)
      command.append(" -verbose");

    printf("==============================================\n");
    int exitCode = WEXITSTATUS(runSystemCommand(command, debug));
    printf("==============================================\n");
    if (exitCode != 0)
      LOG(LogWarning) << "...launch terminated with nonzero exit code " << exitCode << "!";

    NotificationManager::Instance().Notify(game, Notification::EndGame);
    padToKeyboard.StopMapping();
  }

  // Reinit
  window.Initialize();
  VolumeControl::getInstance()->init();
  AudioManager::Instance().Reactivate();
  window.normalizeNextUpdate();

  // Update number of times the game has been launched
  game.Metadata().IncPlaycount();

  // Update last played time
  systemManager.UpdateLastPlayedSystem(game);
  game.Metadata().SetLastplayedNow();
}

std::string SystemData::demoInitialize(Window&)
{
  LOG(LogInfo) << "Entering demo mode...";

  OrderedDevices controllers = InputManager::Instance().GenerateConfiguration();
  std::string controlersConfig = InputManager::GenerateConfiggenConfiguration(controllers);
  LOG(LogInfo) << "Controllers config : " << controlersConfig;

  VideoEngine::Instance().StopVideo();
  AudioManager::Instance().Deactivate();
  VolumeControl::getInstance()->deinit();

  Window::Finalize();

  return controlersConfig;
}

void SystemData::demoFinalize(Window& window)
{
  // Reinit
  window.Initialize();
  VolumeControl::getInstance()->init();
  AudioManager::Instance().Reactivate();
  window.normalizeNextUpdate();
}

bool
SystemData::DemoRunGame(const FileData& game, const EmulatorData& emulator, int duration, int infoscreenduration, const std::string& controlersConfig)
{
  std::string command = mDescriptor.Command();

  const std::string rom = game.getPath().MakeEscaped();
  const std::string basename = game.getPath().FilenameWithoutExtension();
  const std::string rom_raw = game.getPath().ToString();

  command = Strings::Replace(command, "%ROM%", rom);
  command = Strings::Replace(command, "%CONTROLLERSCONFIG%", controlersConfig);
  command = Strings::Replace(command, "%SYSTEM%", game.getSystem()->getName());
  command = Strings::Replace(command, "%BASENAME%", basename);
  command = Strings::Replace(command, "%ROM_RAW%", rom_raw);
  command = Strings::Replace(command, "%EMULATOR%", emulator.Emulator());
  command = Strings::Replace(command, "%CORE%", emulator.Core());
  command = Strings::Replace(command, "%RATIO%", game.Metadata().RatioAsString());
  command = Strings::Replace(command, "%NETPLAY%", "");

  // Add demo stuff
  command += " -demo 1";
  command += " -demoduration ";
  command += std::to_string(duration);
  command += " -demoinfoduration ";
  command += std::to_string(infoscreenduration);

  bool debug = RecalboxConf::Instance().AsBool("emulationstation.debuglogs");
  if (debug)
    command.append(" -verbose");

  LOG(LogInfo) << "Demo command: " << command;
  NotificationManager::Instance().Notify(game, Notification::RunDemo);
  int exitCode = WEXITSTATUS(runSystemCommand(command, debug));
  NotificationManager::Instance().Notify(game, Notification::EndDemo);
  LOG(LogInfo) << "Demo exit code :	" << exitCode;


  // Configgen returns an exitcode 0x33 when the user interact with any pad/mouse
  if (exitCode == 0x33)
  {
    LOG(LogInfo) << "Exiting demo upon user request";
    return true;
  }

  if (exitCode != 0)
    LOG(LogWarning) << "...launch terminated with nonzero exit code " << exitCode << "!";

  return false;
}

void SystemData::populateFolder(FolderData* folder, FileData::StringMap& doppelgangerWatcher)
{
  LOG(LogInfo) << folder->getSystem()->getFullName() << ": Searching games/roms in " << folder->getPath().ToString()
               << "...";

  try
  {
    folder->populateRecursiveFolder(Strings::ToLowerASCII(mDescriptor.Extension()), this, doppelgangerWatcher);
  }
  catch (std::exception& ex)
  {
    LOG(LogError) << "Reading folder \"" << folder->getPath().ToString() << "\" has raised an error!";
    LOG(LogError) << "Exception: " << ex.what();
  }
}

Path SystemData::getGamelistPath(bool forWrite) const
{
  bool zip = RecalboxConf::Instance().AsBool("emulationstation.zippedgamelist", false);
  Path filePath = mRootFolder.getPath() / (zip ? "gamelist.zip" : "gamelist.xml");

  if (forWrite) // Write mode, ensure folder exist
  {
    if (!filePath.Directory().Exists())
      filePath.Directory().CreatePath();
  }
  else if (!filePath.Exists()) // Read mode. Try selected mode first, the fallback to the other mode
  {
    Path otherFilePath = mRootFolder.getPath() / (zip ? "gamelist.xml" : "gamelist.zip");
    if (otherFilePath.Exists()) return otherFilePath;
  }

  return filePath;
}

Path SystemData::getThemePath() const
{
  // where we check for themes, in order:
  // 1. [SYSTEM_PATH]/theme.xml
  // 2. system theme from currently selected theme set [CURRENT_THEME_PATH]/[SYSTEM]/theme.xml
  // 3. default system theme from currently selected theme set [CURRENT_THEME_PATH]/theme.xml

  // first, check game folder
  Path localThemePath = mRootFolder.getPath() / "theme.xml";
  if (localThemePath.Exists()) return localThemePath;

  // not in game folder, try system theme in theme sets
  localThemePath = ThemeData::getThemeFromCurrentSet(mDescriptor.ThemeFolder());
  if (localThemePath.Exists()) return localThemePath;

  // not system theme, try default system theme in theme set
  localThemePath = localThemePath.Directory().Directory() / "theme.xml";
  if (localThemePath.Exists()) return localThemePath;

  // none of the above, try default
  localThemePath = localThemePath.Directory() / "default/theme.xml";
  if (localThemePath.Exists()) return localThemePath;

  // No luck...
  return Path::Empty;
}

void SystemData::loadTheme()
{
  Path path = getThemePath();
  if (!path.Exists()) // no theme available for this platform
    return;

  try
  {
    mTheme.loadFile(ThemeFolder(), path);
  }
  catch (ThemeException& e)
  {
    LOG(LogError) << e.what();
    mDescriptor.SetDefaultThemePath();
    mTheme.loadFile(ThemeFolder(), path);
  }
}

std::string SystemData::getLocalizedText(const std::string& source)
{
  // Extract prefered language/region
  std::string locale = Strings::ToLowerASCII(RecalboxConf::Instance().AsString("system.language", "en_US"));

  // Get start
  std::string key = "[";
  key += locale;
  key += "]";
  unsigned long start = source.find(key);
  if (start == std::string::npos)
  {
    std::string language = (locale.length() == 5) ? locale.substr(0, 2) : "en";
    key = "[";
    key += language;
    key += "]";
    start = source.find(key);
    if (start == std::string::npos)
    {
      key = "[en]";
      start = source.find(key);
      if (start == std::string::npos)
        return source;
    }
  }

  // Get end
  unsigned long stop = source.find('[', start + key.length());
  if (stop == std::string::npos) stop = source.length();

  // Trimming
  start = source.find_first_not_of(" \t\n\r", start + key.length());
  if (start == std::string::npos) return "";
  stop = source.find_last_not_of(" \t\n\r", stop);

  return source.substr(start, stop - start);
}

void SystemData::overrideFolderInformation(FileData* folderdata)
{
  // Override image
  bool imageOverriden = false;
  Path fullPath = folderdata->getPath() / ".folder.picture.svg";

  MetadataDescriptor& metadata = folderdata->Metadata();

  if (fullPath.Exists())
  {
    metadata.SetVolatileImagePath(fullPath);
    imageOverriden = true;
  }
  else
  {
    fullPath = folderdata->getPath() / ".folder.picture.png";
    if (fullPath.Exists())
    {
      metadata.SetVolatileImagePath(fullPath);
      imageOverriden = true;
    }
  }

  // Override description oly if the image has been overriden
  if (imageOverriden)
  {
    fullPath = folderdata->getPath() / ".folder.description.txt";
    std::string text = Files::LoadFile(fullPath);
    if (text.length() != 0)
    {
      text = getLocalizedText(text);
      if (text.length() != 0)
        metadata.SetVolatileDescription(text);
    }
  }
}

FileData* SystemData::LookupOrCreateGame(const Path& root, const Path& path, ItemType type, FileData::StringMap& doppelgangerWatcher)
{
  if (!path.StartWidth(root))
  {
    LOG(LogError) << "File path \"" << path.ToString() << "\" is outside system path \"" << getStartPath().ToString()
                  << "\"";
    return nullptr;
  }

  bool isVirtual = IsVirtual();

  int itemStart = root.ItemCount();
  int itemLast = path.ItemCount() - 1;
  FolderData* treeNode = &mRootFolder;
  for (int itemIndex = itemStart; itemIndex <= itemLast; ++itemIndex)
  {
    // Get the key for duplicate detection. MUST MATCH KEYS USED IN populateRecursiveFolder.populateRecursiveFolder - Always fullpath
    std::string key = path.UptoItem(itemIndex);
    FileData* item = (doppelgangerWatcher.find(key) != doppelgangerWatcher.end()) ? doppelgangerWatcher[key] : nullptr;

    // Some ScummVM folder/games may create inconsistent folders
    if (!treeNode->isFolder()) return nullptr;

    // Last path component?
    if (itemIndex == itemLast)
    {
      if (type == ItemType::Game) // Final file
      {
        FileData* game = item;
        if (game == nullptr && !isVirtual)
        {
          // Add final game
          game = new FileData(path, this);
          doppelgangerWatcher[key] = game;
          treeNode->addChild(game, true);
        }
        else if (game != nullptr && isVirtual)
        {
          // Add existing game & remove from doppleganger
          treeNode->addChild(game, false);
          doppelgangerWatcher.erase(key);
        }
        return game;
      }
      else // Final folder (scrapped obviously)
      {
        FolderData* folder = (FolderData*) item;
        if (folder == nullptr)
        {
          // Create missing folder in both case, virtual or not
          folder = new FolderData(Path(key), this);
          doppelgangerWatcher[key] = folder;
          treeNode->addChild(folder, true);
        }
        return folder;
      }
    }
    else // Intermediate path
    {
      FolderData* folder = (FolderData*) item;
      if (folder == nullptr)
      {
        // Create missing folder in both case, virtual or not
        folder = new FolderData(Path(key), this);
        doppelgangerWatcher[key] = folder;
        treeNode->addChild(folder, true);
      }
      treeNode = folder;
    }
  }

  return nullptr;
}

void SystemData::ParseGamelistXml(FileData::StringMap& doppelgangerWatcher, bool forceCheckFile)
{
  try
  {
    Path xmlpath = getGamelistPath(false);
    if (!xmlpath.Exists()) return;

    XmlDocument gameList;
    if (Strings::ToLowerASCII(xmlpath.Extension()) == ".zip")
    {
      Zip zip(xmlpath);
      if (zip.Count() != 1)
      {
        LOG(LogError) << "Invalid zipped gamelist: More than one file in the archive!";
        return;
      }
      Path gamelist(zip.FileName(0));
      if (gamelist.Filename() != "gamelist.xml")
      {
        LOG(LogError) << "Invalid zipped gamelist: No gamelist.xml found!";
        return;
      }
      std::string content = zip.Content(0);
      XmlResult result = gameList.load_string(content.data());
      if (!result)
      {
        LOG(LogError) << "Could not parse " << xmlpath.ToString() << " file!";
        return;
      }
    }
    else
    {
      XmlResult result = gameList.load_file(xmlpath.ToChars());
      if (!result)
      {
        LOG(LogError) << "Could not parse " << xmlpath.ToString() << " file!";
        return;
      }
    }

    Path relativeTo = getStartPath();
    XmlNode games = gameList.child("gameList");
    if (games != nullptr)
      for (const XmlNode fileNode : games.children())
      {
        ItemType type;
        std::string name = fileNode.name();
        if (name == "game") type = ItemType::Game;
        else if (name == "folder") type = ItemType::Folder;
        else continue; // Unknown node

        Path path = relativeTo / Xml::AsString(fileNode, "path", "");
        if (forceCheckFile)
          if (!path.Exists())
            continue;

        FileData* file = LookupOrCreateGame(mRootFolder.getPath(), path, type, doppelgangerWatcher);
        if (file == nullptr)
        {
          LOG(LogError) << "Error finding/creating FileData for \"" << path.ToString() << "\", skipping.";
          continue;
        }

        // load the metadata
        file->Metadata().Deserialize(fileNode, relativeTo);
      }
  }
  catch (std::exception& ex)
  {
    LOG(LogError) << "System \"" << getName() << "\" has raised an error while reading its gamelist.xml!";
    LOG(LogError) << "Exception: " << ex.what();
  }
}

void SystemData::UpdateGamelistXml()
{
  //We do this by reading the XML again, adding changes and then writing it back,
  //because there might be information missing in our systemdata which would then miss in the new XML.
  //We have the complete information for every game though, so we can simply remove a game
  //we already have in the system from the XML, and then add it back from its GameData information...
  if (Settings::Instance().IgnoreGamelist()) return;

  // Dirty?
  if (!getRootFolder().IsDirty()) return;

  try
  {
    /*
     * Get all folder & games in a flat storage
     */
    FileData::List fileList = mRootFolder.getAllItemsRecursively(true, true);
    FileData::List folderList = mRootFolder.getAllFolders();
    // Nothing to process?
    if (fileList.empty()) return;

    /*
     * Create gamelist
     */
    Path xmlReadPath = getGamelistPath(false);
    XmlDocument document;
    XmlNode gameList = document.append_child("gameList");

    /*
     * Serialize folder and game nodes
     */
    for (const FileData* folder : folderList)
      folder->Metadata().Serialize(gameList, folder->getPath(), getStartPath());
    for (const FileData* file : fileList)
      file->Metadata().Serialize(gameList, file->getPath(), getStartPath());

    /*
     * Custom thread-safe writer
     */
    struct XmlWriter : public pugi::xml_writer
    {
      std::string mOutput;
      void write(const void* data, size_t size) override { mOutput.append((const char*)data, size); }
    }
    Writer;

    /*
     * Write the list.
     * At this point, we're sure at least one node has been updated (or added and updated).
     */
    Path xmlWritePath(getGamelistPath(true));
    xmlWritePath.Directory().CreatePath();
    document.save(Writer);

    // Save
    if (Strings::ToLowerASCII(xmlWritePath.Extension()) == ".zip")
    {
      Zip zip(xmlWritePath, true);
      Path xmlTruePath = xmlWritePath.ChangeExtension(".xml");
      if (zip.Add(Writer.mOutput, xmlTruePath.Filename()))
      {
        xmlTruePath.Delete();
        LOG(LogInfo) << "Saved gamelist.zip for system " << getFullName() << ". Updated items: " << fileList.size()
                     << "/" << fileList.size();
      }
      else LOG(LogError) << "Failed to save " << xmlWritePath.ToString();
    }
    else
    {
      if (Files::SaveFile(xmlWritePath, Writer.mOutput))
      {
        xmlWritePath.ChangeExtension(".zip").Delete();
        LOG(LogInfo) << "Saved gamelist.xml for system " << getFullName() << ". Updated items: " << fileList.size()
                     << "/" << fileList.size();
      }
      else LOG(LogError) << "Failed to save " << xmlWritePath.ToString();
    }
  }
  catch (std::exception& e)
  {
    LOG(LogError) << "Something went wrong while saving " << getFullName() << " : " << e.what();
  }
}

bool SystemData::IsFavorite() const
{
  return (mProperties & Properties::Favorite) != 0;
}

bool SystemData::IsVirtual() const
{
  return (mProperties & Properties::Virtual) != 0;
}

bool SystemData::IsSelfSorted() const
{
  return (mProperties & Properties::FixedSort) != 0;
}

bool SystemData::IsAlwaysFlat() const
{
  return (mProperties & Properties::AlwaysFlat) != 0;
}

void SystemData::BuildDoppelgangerMap(FileData::StringMap& doppelganger, bool includefolder) const
{
  mRootFolder.BuildDoppelgangerMap(doppelganger, includefolder);
}

void SystemData::UpdateLastPlayedGame(FileData& updated)
{
  // Update game
  updated.Metadata().SetLastplayedNow();

  // Build the doppelganger map
  FileData::StringMap map;
  BuildDoppelgangerMap(map, false);

  // Add or not the updated game
  LookupOrCreateGame(updated.getSystem()->getRootFolder().getPath(), updated.getPath(), ItemType::Game, map);
}
