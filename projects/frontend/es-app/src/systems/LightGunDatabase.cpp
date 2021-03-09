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
  mCurrentList = mSystemLists.try_get(system.getName());
  return mCurrentList != nullptr;
}

bool LightGunDatabase::ApplyFilter(const FileData& file) const
{
  std::string name = GetSimplifiedName(file.getName().empty() ? file.getPath().FilenameWithoutExtension() : file.getName());
  if (mCurrentList != nullptr)
    for(const std::string& gamename : *mCurrentList)
      if (Strings::Contains(name, gamename))
        return true;
  return false;
}

std::string LightGunDatabase::GetSimplifiedName(const std::string& name)
{
  std::string result(name);

  int writeIndex = 0;
  for(int i = -1; ++i < (int)result.size(); )
  {
    unsigned int c = result[i] | 0x20; // upper => lowercase letters, no effect on digits
    if (((unsigned int)(c - 0x30) <= 9) || ((unsigned int)(c - 0x61) <= 25))
      result[writeIndex++] = (char)c;
  }
  result.resize(writeIndex);

  { LOG(LogDebug) << "[LightGun] " << name << " = " << result; }

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
    for (const XmlNode system : root.children("system"))
    {
      // Get system name
      std::string systemName = Xml::AsString(system, "platform", "");
      if (systemName.empty()) continue;
      { LOG(LogDebug) << "[LightGun] Load system: " << systemName; }

      // Create system list
      Strings::Vector& gameList = mSystemLists[systemName];

      // Run through games
      XmlNode games = system.child("games");
      if (games != nullptr)
        for(const XmlNode game : games.children("game"))
        {
          std::string gameName = Xml::AsString(game, "name", "");
          if (!gameName.empty())
            gameList.push_back(gameName);
        }
      { LOG(LogDebug) << "[LightGun] " << gameList.size() << " games found in system " << systemName; }
    }

}
