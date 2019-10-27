#include "SystemData.h"
#include "AudioManager.h"
#include "VolumeControl.h"
#include "utils/Log.h"
#include "Util.h"
#include <RecalboxConf.h>
#include <RootFolders.h>
#include <recalbox/RecalboxSystem.h>
#include <VideoEngine.h>
#include <utils/StringUtil.h>
#include <utils/os/fs/FileSystemUtil.h>
#include <utils/FileUtil.h>
#include <Settings.h>
#include <boost/property_tree/xml_parser.hpp>

namespace fs = boost::filesystem;
namespace pt = boost::property_tree;

EmulatorDescriptor EmulatorList::sEmptyEmulator("NO EMULATOR");

SystemData::SystemData(const SystemDescriptor& descriptor,
                       bool childOwnership,
                       bool favorite)
  : mDescriptor(descriptor),
    mRootFolder(childOwnership, descriptor.RomPath().ToString(), this),
    mSortId(RecalboxConf::getInstance()->getUInt(mDescriptor.Name() + ".sort")),
    mIsFavorite(favorite)
{
  // Set name
  mRootFolder.Metadata().SetName(mDescriptor.FullName());
}

void SystemData::RunGame(Window& window, FileData& game, const std::string& netplay, const std::string& core,
                         const std::string& ip, const std::string& port)
{
  (void) core;

  LOG(LogInfo) << "Attempting to launch game...";

  VideoEngine::This().StopVideo();
  AudioManager::getInstance()->deinit();
  VolumeControl::getInstance()->deinit();

  std::string controlersConfig = InputManager::Instance().GenerateConfiggenConfiguration();
  LOG(LogInfo) << "Controllers config : " << controlersConfig;
  window.deinit();


  std::string command = mDescriptor.Command();

  const std::string rom = FileSystemUtil::getEscapedPath(game.getPath().generic_string());
  const std::string basename = game.getPath().stem().string();
  const std::string rom_raw = fs::path(game.getPath()).make_preferred().string();

  command = StringUtil::replace(command, "%ROM%", rom);
  command = StringUtil::replace(command, "%CONTROLLERSCONFIG%", controlersConfig);
  command = StringUtil::replace(command, "%SYSTEM%", game.getSystem()->getName());
  command = StringUtil::replace(command, "%BASENAME%", basename);
  command = StringUtil::replace(command, "%ROM_RAW%", rom_raw);
  command = StringUtil::replace(command, "%EMULATOR%", game.Metadata().Emulator());
  command = StringUtil::replace(command, "%CORE%", game.Metadata().Core());
  command = StringUtil::replace(command, "%RATIO%", game.Metadata().RatioAsString());

  if (netplay == "client")
  {
    command = StringUtil::replace(command, "%NETPLAY%", "-netplay client -netplay_port " + port + " -netplay_ip " + ip);
  }
  else if (netplay == "host")
  {
    std::string hash = game.Metadata().RomCrc32AsString();
    std::string hashcmd;
    if (!hash.empty()) hashcmd = " -hash " + hash;
    command = StringUtil::replace(command, "%NETPLAY%", "-netplay host -netplay_port " +
                                                        RecalboxConf::getInstance()->get("global.netplay.port") +
                                                        hashcmd);
  }
  else command = StringUtil::replace(command, "%NETPLAY%", "");

  LOG(LogInfo) << "	" << command;
  std::cout << "==============================================\n";
  RecalboxSystem::NotifyGame(game, true, false);
  int exitCode = runSystemCommand(command);
  RecalboxSystem::NotifyGame(game, false, false);
  std::cout << "==============================================\n";

  if (exitCode != 0)
    LOG(LogWarning) << "...launch terminated with nonzero exit code " << exitCode << "!";

  // Reinit
  window.init();
  VolumeControl::getInstance()->init();
  AudioManager::getInstance()->resumeMusic();
  window.normalizeNextUpdate();

  //update number of times the game has been launched
  game.Metadata().IncPlaycount();

  //update last played time
  game.Metadata().SetLastplayedNow();
}

std::string SystemData::demoInitialize(Window& window)
{
  LOG(LogInfo) << "Entering demo mode...";

  std::string controlersConfig = InputManager::Instance().GenerateConfiggenConfiguration();
  LOG(LogInfo) << "Controllers config : " << controlersConfig;

  VideoEngine::This().StopVideo();
  AudioManager::getInstance()->deinit();
  VolumeControl::getInstance()->deinit();

  window.deinit();

  return controlersConfig;
}

void SystemData::demoFinalize(Window& window)
{
  // Reinit
  window.init();
  VolumeControl::getInstance()->init();
  AudioManager::getInstance()->resumeMusic();
  window.normalizeNextUpdate();
}

bool SystemData::DemoRunGame(const FileData& game, int duration, int infoscreenduration, const std::string& controlersConfig)
{
  std::string command = mDescriptor.Command();

  const std::string rom = FileSystemUtil::getEscapedPath(game.getPath().generic_string());
  const std::string basename = game.getPath().stem().string();
  const std::string rom_raw = fs::path(game.getPath()).make_preferred().string();

  command = StringUtil::replace(command, "%ROM%", rom);
  command = StringUtil::replace(command, "%CONTROLLERSCONFIG%", controlersConfig);
  command = StringUtil::replace(command, "%SYSTEM%", game.getSystem()->getName());
  command = StringUtil::replace(command, "%BASENAME%", basename);
  command = StringUtil::replace(command, "%ROM_RAW%", rom_raw);
  command = StringUtil::replace(command, "%EMULATOR%", game.Metadata().Emulator());
  command = StringUtil::replace(command, "%CORE%", game.Metadata().Core());
  command = StringUtil::replace(command, "%RATIO%", game.Metadata().RatioAsString());
  command = StringUtil::replace(command, "%NETPLAY%", "");

  // Add demo stuff
  command += " -demo 1";
  command += " -demoduration ";
  command += std::to_string(duration);
  command += " -demoinfoduration ";
  command += std::to_string(infoscreenduration);

  LOG(LogInfo) << "Demo command: " << command;
  RecalboxSystem::NotifyGame(game, true, true);
  int exitCode = runSystemCommand(command);
  RecalboxSystem::NotifyGame(game, false, false);
  LOG(LogInfo) << "Demo exit code :	" << exitCode;


  // Configgen returns an exitcode 0x33 when the user interact with any pad/mouse
  // this exitcode returns here byte-swapped or shifted. Need further investigation
  if (exitCode == 0x3300)
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
  LOG(LogInfo) << folder->getSystem()->getFullName() << ": Searching games/roms in "
               << folder->getPath().generic_string() << "...";

  try
  {
    folder->populateRecursiveFolder(mDescriptor.Extension(), this, doppelgangerWatcher);
  }
  catch (std::exception& ex)
  {
    LOG(LogError) << "Reading folder \"" << folder->getPath().generic_string() << "\" has raised an error!";
    LOG(LogError) << "Exception: " << ex.what();
  }
}

std::string SystemData::getGamelistPath(bool forWrite) const
{
  fs::path filePath;
  filePath = mRootFolder.getPath() / "gamelist.xml";
  if (forWrite && !fs::exists(filePath))
    fs::create_directories(filePath.parent_path());
  return filePath.generic_string();
}

std::string SystemData::getThemePath() const
{
  // where we check for themes, in order:
  // 1. [SYSTEM_PATH]/theme.xml
  // 2. system theme from currently selected theme set [CURRENT_THEME_PATH]/[SYSTEM]/theme.xml
  // 3. default system theme from currently selected theme set [CURRENT_THEME_PATH]/theme.xml

  // first, check game folder
  fs::path localThemePath = mRootFolder.getPath() / "theme.xml";
  if (fs::exists(localThemePath)) return localThemePath.generic_string();

  // not in game folder, try system theme in theme sets
  localThemePath = ThemeData::getThemeFromCurrentSet(mDescriptor.ThemeFolder());
  if (fs::exists(localThemePath)) return localThemePath.generic_string();

  // not system theme, try default system theme in theme set
  localThemePath = localThemePath.parent_path().parent_path() / "theme.xml";
  if (fs::exists(localThemePath)) return localThemePath.generic_string();

  //none of the above, try default
  localThemePath = localThemePath.parent_path() / "default/theme.xml";
  if (fs::exists(localThemePath)) return localThemePath.generic_string();

  // No luck...
  return "";
}

void SystemData::loadTheme()
{
  std::string path = getThemePath();
  if (!fs::exists(path)) // no theme available for this platform
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
  std::string locale = StringUtil::toLower(RecalboxConf::Instance().get("system.language", "en_US"));

  // Get start
  std::string key = "["; key += locale; key += "]";
  unsigned long start = source.find(key);
  if (start == std::string::npos)
  {
    std::string language = (locale.length() == 5) ? locale.substr(0,2) : "en";
    key = "["; key += language; key += "]";
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
  std::string fullPath = folderdata->getPath().generic_string() + "/.folder.picture.svg";

  MetadataDescriptor& metadata = folderdata->Metadata();

  if (FileSystemUtil::exists(fullPath))
  {
    metadata.SetVolatileImagePath(fullPath);
    imageOverriden = true;
  }
  else
  {
    fullPath = folderdata->getPath().generic_string() + "/.folder.picture.png";
    if (FileSystemUtil::exists(fullPath))
    {
      metadata.SetVolatileImagePath(fullPath);
      imageOverriden = true;
    }
  }

  // Override description oly if the image has been overriden
  if (imageOverriden)
  {
    fullPath = folderdata->getPath().generic_string() + "/.folder.description.txt";
    std::string text = FileUtil::loadTextFile(fullPath);
    if (text.length() != 0)
    {
      text = getLocalizedText(text);
      if (text.length() != 0)
        metadata.SetVolatileDescription(text);
    }
  }
}

FileData* SystemData::LookupOrCreateGame(const boost::filesystem::path& path, ItemType type,
                                         FileData::StringMap& doppelgangerWatcher)
{
  // first, verify that path is within the system's root folder
  FolderData* root = getRootFolder();

  bool contains = false;
  fs::path relative = removeCommonPathUsingStrings(path, root->getPath(), contains);
  if (!contains)
  {
    LOG(LogError) << "File path \"" << path << "\" is outside system path \"" << getStartPath().ToString() << "\"";
    return nullptr;
  }

  auto path_it = relative.begin();
  FolderData* treeNode = root;
  std::string key;
  while(path_it != relative.end())
  {
    // Get the key for duplicate detection. MUST MATCH KEYS USED IN populateRecursiveFolder.populateRecursiveFolder - Always fullpath
    key = (treeNode->getPath() / *path_it).string();
    FileData* item = (doppelgangerWatcher.find(key) != doppelgangerWatcher.end()) ? doppelgangerWatcher[key] : nullptr;

    // Last path component?
    if (path_it == --relative.end())
    {
      if (type == ItemType::Game) // Final file
      {
        FileData* game = item;
        if (game == nullptr)
        {
          // Add final game
          game = new FileData(path, this);
          doppelgangerWatcher[key] = game;
          treeNode->addChild(game, true);
        }
        return game;
      }
      else // Final folder (scrapped obviously)
      {
        FolderData* folder = (FolderData*)item;
        if (folder == nullptr)
        {
          // create missing folder
          folder = new FolderData(key, this);
          doppelgangerWatcher[key] = folder;
          treeNode->addChild(folder, true);
        }
        return folder;
      }
    }
    else // Intermediate path
    {
      FolderData* folder = (FolderData*)item;
      if (folder == nullptr)
      {
        // create missing folder
        folder = new FolderData(key, this);
        doppelgangerWatcher[key] = folder;
        treeNode->addChild(folder, true);
      }
      treeNode = folder;
    }
    path_it++;
  }

  return nullptr;
}

void SystemData::ParseGamelistXml(FileData::StringMap& doppelgangerWatcher)
{
  LOG(LogInfo) << getFullName() << ": Parsing XML file " << getGamelistPath(false) << "...";

  try
  {
    std::string xmlpath = getGamelistPath(false);

    if(!boost::filesystem::exists(xmlpath))
      return;

    MetadataDescriptor::Tree gameList;
    try
    {
      pt::read_xml(xmlpath, gameList, 0, std::locale("en_US.UTF8"));
    }
    catch(std::exception& e)
    {
      LOG(LogError) << "Could not parse " << xmlpath <<" file!";
      LOG(LogError) << e.what();
      return;
    }

    fs::path relativeTo = getStartPath().ToString();
    fs::path path;
    for (const auto& fileNode : gameList.get_child("gameList"))
    {
      ItemType type;
      if (fileNode.first == "game") type = ItemType::Game;
      else if (fileNode.first == "folder") type = ItemType::Folder;
      else continue; // Unknown node

      const MetadataDescriptor::Tree& children = fileNode.second;
      path = resolvePath(children.get("path", ""), relativeTo, false);

      FileData* file = LookupOrCreateGame(path, type, doppelgangerWatcher);
      if(file == nullptr)
      {
        LOG(LogError) << "Error finding/creating FileData for \"" << path << "\", skipping.";
        continue;
      }

      // load the metadata
      file->Metadata().Deserialize(fileNode, relativeTo.generic_string());
    }
  }
  catch(std::exception& ex)
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
  if (Settings::getInstance()->getBool("IgnoreGamelist")) return;

  try
  {
    /*
     * Get all folder & games in a flat storage
     */
    FolderData *rootFolder = getRootFolder();
    if (rootFolder == nullptr) return;

    FileData::List fileData = rootFolder->getAllItemsRecursively(true);
    // Nothing to process?
    if (fileData.empty()) return;

    /*
     * Create game/folder map for fast seeking using relative path as key
     */
    std::unordered_map<std::string, const FileData *> fileLinks;
    for (const FileData *file : fileData)                                                                    // For each File
      if (file->Metadata().IsDirty())                                                                        // with updated metadata
        fileLinks[makeRelativePath(file->getPath(), getStartPath().ToString(), false).generic_string()] = file; // store the relative path
    // Nothing changed?
    if (fileLinks.empty()) return;

    /*
     * Load or create gamelist node
     */
    std::string xmlReadPath = getGamelistPath(false);
    MetadataDescriptor::Tree document;
    if (boost::filesystem::exists(xmlReadPath))
    {
      try
      {
        pt::read_xml(xmlReadPath, document, pt::xml_parser::trim_whitespace, std::locale("en_US.UTF8"));
      }
      catch (std::exception &e)
      {
        LOG(LogError) << "Could not parse " << xmlReadPath << " file!";
        LOG(LogError) << e.what();
      }
    } else
    {
      // Create empty game list node
      document.add_child("gameList", MetadataDescriptor::Tree());
    }
    MetadataDescriptor::Tree &gameList = document.get_child("gameList");

    /*
     * Update pass #1 : Remove node from the gamelist where corresponding metadata have changed
     */
    for (auto it = gameList.begin(); it != gameList.end();)                         // For each gamelist entry
      if (fileLinks.find((*it).second.get("path", "")) != fileLinks.end())          // corresponding to an updated file
        it = gameList.erase(it);                                                    // delete the entry from the gamelist
      else
        ++it;

    /*
     * Update pass #2 - Insert new/updated game/folder nodes into the gamelist node
     */
    for (const FileData *file : fileData)                                 // For each file
      if (file->Metadata().IsDirty())                                     // If metadata have changed
        file->Metadata().Serialize(gameList,                              // Insert updated node
                                   file->getPath().generic_string(),
                                   getStartPath().ToString());

    /*
     * Write the list.
     * At this point, we're sure at least one node has been updated (or added and updated).
     */
    boost::filesystem::path xmlWritePath(getGamelistPath(true));
    boost::filesystem::create_directories(xmlWritePath.parent_path());
    try
    {
      pt::xml_writer_settings<std::string> settings(' ', 2);
      pt::write_xml(xmlWritePath.generic_string(), document, std::locale("en_US.UTF8"), settings);
      LOG(LogInfo) << "Saved gamelist.xml for system " << getFullName() << ". Updated items: " << fileLinks.size() << "/" << fileData.size();
    }
    catch (std::exception &e)
    {
      LOG(LogError) << "Failed to save " << xmlWritePath.generic_string() << " : " << e.what();
    }

  }
  catch(std::exception& e)
  {
    LOG(LogError) << "Something went wrong while saving " << getFullName() << " : " << e.what();
  }
}
