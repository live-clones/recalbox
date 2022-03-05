//
// Created by pre2010-02 on 02/03/2022.
//

#include "Versions.h"

std::string& Versions::Serialize(Versions::GameVersions versions)
{
  static HashMap<GameVersions, std::string> sVesionTagToVersion
    ({

       {GameVersions::Sample, "sample"},
       {GameVersions::Sample1, "sample 1"},
       {GameVersions::Sample2, "sample 2"},

       {GameVersions::Proto, "proto"},
       {GameVersions::Proto1, "proto 1"},
       {GameVersions::Proto2, "proto 2"},

       {GameVersions::Beta, "beta"},
       {GameVersions::Beta1, "beta 1"},
       {GameVersions::Beta2, "beta 2"},
       {GameVersions::Beta3, "beta 3"},
       {GameVersions::Beta4, "beta 4"},
       {GameVersions::Beta5, "beta 5"},

       {GameVersions::Rev, "rev"},
       {GameVersions::Rev1, "rev 1"},
       {GameVersions::Rev2, "rev 2"},
       {GameVersions::Rev3, "rev 3"},
       {GameVersions::Rev4, "rev 4"},
       {GameVersions::Rev5, "rev 5"},
       {GameVersions::VirtualConsole, "virtual console"}
       });

  std::string* found = sVesionTagToVersion.try_get(versions);
  if (found != nullptr)
    return *found;

  static std::string sEmpty("");
  return sEmpty;
}

Versions::GameVersions Versions::Deserialize(const std::string& tag)
{
  static HashMap<std::string, GameVersions> sVesionTagToVersion
    ({

      {"sample", GameVersions::Sample},
      {"sample 1", GameVersions::Sample1},
      {"sample 2", GameVersions::Sample2},

      {"proto",  GameVersions::Proto},
      {"proto 1",  GameVersions::Proto1},
      {"proto 2",  GameVersions::Proto2},

      {"beta",   GameVersions::Beta},
      {"beta 1", GameVersions::Beta1},
      {"beta 2",  GameVersions::Beta2},
      {"beta 3",  GameVersions::Beta3},
      {"beta 4",  GameVersions::Beta4},
      {"beta 5",  GameVersions::Beta5},

      {"rev",    GameVersions::Rev},
      {"rev 1",  GameVersions::Rev1},
      {"rev 2",   GameVersions::Rev2},
      {"rev 3",   GameVersions::Rev3},
      {"rev 4",   GameVersions::Rev4},
      {"rev 5",   GameVersions::Rev5}
    });

  GameVersions* found = sVesionTagToVersion.try_get(Strings::ToLowerASCII(tag));
  if (found != nullptr)
    return *found;

  if( Strings::Contains(Strings::ToLowerASCII(tag), "virtual console") ||
    Strings::Contains(Strings::ToLowerASCII(tag), "switch online") ||
    Strings::Contains(Strings::ToLowerASCII(tag), "classic mini"))
  {
    return GameVersions::VirtualConsole;
  }

  return GameVersions::Unknown;
}

Versions::GameVersions Versions::ExtractGameVersionNoIntro(std::string filename)
{
  std::string name = filename;
  for(;;)
  {
    if(!Strings::Contains(name, "(") || !Strings::Contains(name, ")") )
      break;

    const size_t strBegin = name.find_first_of('(');
    const size_t strEnd = name.find_first_of(')');
    std::string tag = Strings::ToLowerASCII(name.substr(strBegin +1, strEnd - strBegin - 1));
    if(strEnd == name.size() -1 || tag.empty())
      break;

    GameVersions gameVersions = Deserialize(tag);
    if(gameVersions != GameVersions::Unknown)
    {
      return gameVersions;
    }

    name = name.substr(strEnd + 1, name.size() -1);
  }
  return GameVersions::None;
}
