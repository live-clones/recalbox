#include "Gamelist.h"
#include "SystemData.h"
#include <boost/filesystem.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <RecalboxConf.h>
#include <utils/os/fs/FileSystemUtil.h>
#include <utils/FileUtil.h>
#include <utils/StringUtil.h>
#include "Log.h"
#include "Settings.h"
#include "Util.h"

namespace fs = boost::filesystem;
namespace pt = boost::property_tree;

std::string getLocalizedText(const std::string& source)
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
      {
        return source;
      }
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

void overrideFolderInformation(FileData* folderdata)
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

FileData* findOrCreateFile(SystemData* system, const boost::filesystem::path& path, ItemType type, bool trustGamelist, FileData::StringMap& doppelgangerWatcher)
{
	// first, verify that path is within the system's root folder
	FolderData* root = system->getRootFolder();
	
	bool contains = false;
	fs::path relative = trustGamelist ? removeCommonPathUsingStrings(path, root->getPath(), contains) : removeCommonPath(path, root->getPath(), contains);
	if (!contains)
	{
		LOG(LogError) << "File path \"" << path << "\" is outside system path \"" << system->getStartPath() << "\"";
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
          game = new FileData(path, system);
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
          folder = new FolderData(key, system);
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
        folder = new FolderData(key, system);
        doppelgangerWatcher[key] = folder;
        treeNode->addChild(folder, true);
      }
      treeNode = folder;
    }
		path_it++;
	}

	return nullptr;
}

void parseGamelist(SystemData* system, FileData::StringMap& doppelgangerWatcher)
{
  try
  {
    bool trustGamelist = RecalboxConf::getInstance()->get("emulationstation.gamelistonly") == "1";
    std::string xmlpath = system->getGamelistPath(false);

    if(!boost::filesystem::exists(xmlpath))
      return;

    LOG(LogInfo) << "Parsing XML file \"" << xmlpath << "\"...";

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

    fs::path relativeTo = system->getStartPath();
    fs::path path;
    for (const auto& fileNode : gameList.get_child("gameList"))
    {
      ItemType type;
      if (fileNode.first == "game") type = ItemType::Game;
      else if (fileNode.first == "folder") type = ItemType::Folder;
      else continue; // Unknown node

      const MetadataDescriptor::Tree& children = fileNode.second;
      path = resolvePath(children.get("path", ""), relativeTo, false);

      if(!trustGamelist)
        if (!boost::filesystem::exists(path))
        {
          LOG(LogWarning) << "File \"" << path << "\" does not exist! Ignoring.";
          continue;
        }

      FileData* file = findOrCreateFile(system, path, type, trustGamelist, doppelgangerWatcher);
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
    LOG(LogError) << "System \"" << system->getName() << "\" has raised an error while reading its gamelist.xml!";
    LOG(LogError) << "Exception: " << ex.what();
  }
}

void updateGamelist(SystemData* system)
{
  //We do this by reading the XML again, adding changes and then writing it back,
  //because there might be information missing in our systemdata which would then miss in the new XML.
  //We have the complete information for every game though, so we can simply remove a game
  //we already have in the system from the XML, and then add it back from its GameData information...
  if(Settings::getInstance()->getBool("IgnoreGamelist")) return;

  try
  {
    /*
     * Get all folder & games in a flat storage
     */
    FolderData *rootFolder = system->getRootFolder();
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
        fileLinks[makeRelativePath(file->getPath(), system->getStartPath(), false).generic_string()] = file; // store the relative path
    // Nothing changed?
    if (fileLinks.empty()) return;

    /*
     * Load or create gamelist node
     */
    std::string xmlReadPath = system->getGamelistPath(false);
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
                                   system->getStartPath());

    /*
     * Write the list.
     * At this point, we're sure at least one node has been updated (or added and updated).
     */
    boost::filesystem::path xmlWritePath(system->getGamelistPath(true));
    boost::filesystem::create_directories(xmlWritePath.parent_path());
    try
    {
      pt::xml_writer_settings<std::string> settings(' ', 2);
      pt::write_xml(xmlWritePath.generic_string(), document, std::locale("en_US.UTF8"), settings);
      LOG(LogInfo) << "Saved gamelist.xml for system " << system->getFullName() << ". Updated items: " << fileLinks.size() << "/" << fileData.size();
    }
    catch (std::exception &e)
    {
      LOG(LogError) << "Failed to save " << xmlWritePath.generic_string() << " : " << e.what();
    }

  }
  catch(std::exception& e)
  {
    LOG(LogError) << "Something went wrong while saving " << system->getFullName() << " : " << e.what();
  }
}