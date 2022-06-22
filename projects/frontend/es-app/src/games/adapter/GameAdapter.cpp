//
// Created by bkg2k on 22/06/22.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include "GameAdapter.h"
#include "GameNameMapManager.h"
#include <systems/SystemData.h>

const std::string GameAdapter::sEasyRPGSystemName(LEGACY_STRING("easyrpg"));
const std::string GameAdapter::sEasyRPGGameName(LEGACY_STRING("RPG_RT.INI"));

std::string GameAdapter::ScrapingName() const
{
  if (mGame.System().Name() == sEasyRPGSystemName)
  {
    if (mGame.FilePath().Filename() == sEasyRPGGameName)
    {
      IniFile ini(mGame.FilePath(), false);
      std::string gameName = Strings::RemoveParenthesis(ini.AsString("GameTitle"));
      if (!gameName.empty()) return gameName;
    }
  }

  return mGame.FilePath().Filename();
}

std::string GameAdapter::DisplayName() const
{
  if (!mGame.Name().empty()) return mGame.Name();

  return RawDisplayName(mGame.System(), mGame.FilePath());
}

std::string GameAdapter::RawDisplayName(SystemData& system, const Path& rompath)
{
  if (system.Name() == sEasyRPGSystemName)
  {
    if (rompath.Filename() == sEasyRPGGameName)
    {
      IniFile ini(rompath, false);
      std::string gameName = Strings::RemoveParenthesis(ini.AsString("GameTitle"));
      if (!gameName.empty()) return gameName;
    }
  }
  else if (GameNameMapManager::HasRenaming(system))
  {
    std::string gameName(GameNameMapManager::Rename(system, rompath.FilenameWithoutExtension()));
    if (!gameName.empty()) return gameName;
  }

  return rompath.FilenameWithoutExtension();
}

long long GameAdapter::RomSize()
{
  Path romPath(mGame.FilePath());
  return romPath.IsFile() ? romPath.Size() : 0L;
}
