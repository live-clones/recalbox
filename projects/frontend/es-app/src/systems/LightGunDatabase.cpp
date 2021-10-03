//
// Created by bkg2k on 08/03/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include "LightGunDatabase.h"

LightGunDatabase::LightGunDatabase()
  : mCurrentList(nullptr)
{
  LoadDatabase();
}

bool LightGunDatabase::SetCurrentSystem(const SystemData& system)
{
  mCurrentList = mSystemLists.try_get(system.Name());
  { LOG(LogDebug) << "[LightGun] System " << system.Name() << " selected."; }
  return mCurrentList != nullptr;
}

bool LightGunDatabase::ApplyFilter(const FileData& file) const
{
  std::string name = GetSimplifiedName(file.Name().empty() ? file.FilePath().FilenameWithoutExtension() : file.Name());
  if (mCurrentList != nullptr)
    for(const std::string& gamename : *mCurrentList)
      if (Strings::Contains(name, gamename))
      {
        { LOG(LogDebug) << "[LightGun] Game " << file.Name() << " match database name " << gamename; }
        return true;
      }
  return false;
}

std::string LightGunDatabase::GetSimplifiedName(const std::string& name)
{
  std::string result(name);

  // Kill decorations
  size_t pos1 = result.find('(');
  size_t pos2 = result.find('[');
  if ((pos2 != std::string::npos) && (pos2 < pos1)) pos1 = pos2;
  if (pos1 != std::string::npos) result = result.substr(0, pos1);

  // Crunch non a-z,0-9,! characters
  int writeIndex = 0;
  for(int i = -1; ++i < (int)result.size(); )
  {
    unsigned int c = result[i] | 0x20; // upper => lowercase letters, no effect on digits
    if (((unsigned int)(c - 0x30) <= 9) || ((unsigned int)(c - 0x61) <= 25) || (c == ('!' | 0x20)))
      result[writeIndex++] = (char)c;
  }
  result.resize(writeIndex);

  //{ LOG(LogDebug) << "[LightGun] " << name << " = " << result; }

  return result;
}

void LightGunDatabase::LoadDatabase()
{
  XmlDocument document;
  XmlResult result = document.load_file(sXmlPath);
  if (!result)
  {
    { LOG(LogError) << "[LightGun] Could not parse " << sXmlPath << " file!"; }
    return;
  }

  XmlNode root = document.child("root");
  if (root != nullptr)
    for (const XmlNode& system : root.children("system"))
    {
      std::string systemName = Xml::AttributeAsString(system, "name", "<missing name>");
      { LOG(LogDebug) << "[LightGun] Load system: " << systemName; }

      // Create system list
      Strings::Vector& gameList = mSystemLists[systemName];

      // Run through games
      std::string gameName;
      for (const XmlNode& games: system.children("gameList"))
        for (const XmlNode& game: games.children("game"))
        {
          gameName = Xml::AttributeAsString(game, "name", "");
          if (!gameName.empty() && Xml::AttributeAsBool(game, "tested", false))
            gameList.push_back(gameName);
        }
      { LOG(LogDebug) << "[LightGun] " << gameList.size() << " games found in system " << systemName; }
    }
}
