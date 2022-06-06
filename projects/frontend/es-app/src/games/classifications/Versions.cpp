//
// Created by pre2010-02 on 02/03/2022.
//

#include "Versions.h"
#include <utils/storage/HashMap.h>
#include <utils/Strings.h>

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

       {GameVersions::RevA, "rev a"},
       {GameVersions::RevB, "rev b"},
       {GameVersions::RevC, "rev c"},
       {GameVersions::RevD, "rev d"},
       {GameVersions::RevE, "rev e"},

       {GameVersions::V1_1, "v1.1"},
       {GameVersions::V1_2, "v1.2"},
       {GameVersions::V1_3, "v1.3"},
       {GameVersions::V1_4, "v1.4"},
       {GameVersions::V1_5, "v1.5"},
       {GameVersions::V1_6, "v1.6"},
       {GameVersions::V1_7, "v1.7"},
       {GameVersions::V1_8, "v1.8"},
       {GameVersions::V1_9, "v1.9"},

       {GameVersions::V2_1, "v2.1"},
       {GameVersions::V2_2, "v2.2"},
       {GameVersions::V2_3, "v2.3"},
       {GameVersions::V2_4, "v2.4"},
       {GameVersions::V2_5, "v2.5"},
       {GameVersions::V2_6, "v2.6"},
       {GameVersions::V2_7, "v2.7"},
       {GameVersions::V2_8, "v2.8"},
       {GameVersions::V2_9, "v2.9"},

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
      {"rev 5",   GameVersions::Rev5},

      {"rev a",  GameVersions::RevA},
      {"rev b",   GameVersions::RevB},
      {"rev c",   GameVersions::RevC},
      {"rev d",   GameVersions::RevD},
      {"rev e",   GameVersions::RevE},

      {"v1.1",  GameVersions::V1_1},
      {"v1.2",  GameVersions::V1_2},
      {"v1.3",  GameVersions::V1_3},
      {"v1.4",  GameVersions::V1_4},
      {"v1.5",  GameVersions::V1_5},
      {"v1.6",  GameVersions::V1_6},
      {"v1.7",  GameVersions::V1_6},
      {"v1.8",  GameVersions::V1_8},
      {"v1.9",  GameVersions::V1_9},

      {"v2.1",  GameVersions::V1_1},
      {"v2.2",  GameVersions::V1_2},
      {"v2.3",  GameVersions::V1_3},
      {"v2.4",  GameVersions::V1_4},
      {"v2.5",  GameVersions::V1_5},
      {"v2.6",  GameVersions::V1_6},
      {"v2.7",  GameVersions::V1_6},
      {"v2.8",  GameVersions::V1_8},
      {"v2.9",  GameVersions::V1_9}

    });


  GameVersions* found = sVesionTagToVersion.try_get(tag);
  if (found != nullptr)
    return *found;

  if (Strings::Contains(tag, "virtual console") ||
      Strings::Contains(tag, "switch online") ||
      Strings::Contains(tag, "classic mini"))
    return GameVersions::VirtualConsole;

  return GameVersions::Unknown;
}

Versions::GameVersions Versions::ExtractGameVersionNoIntro(const std::string& filename)
{
  for(int end = 0;;)
  {
    int begin = (int)filename.find('(', end);
    if (begin == (int)std::string::npos) break;
    end = (int)filename.find(')', begin);
    if (end == (int)std::string::npos) break;

    std::string tag = Strings::ToLowerASCII(filename.substr(begin +1, end - begin - 1));
    if (tag.empty()) break;

    GameVersions gameVersions = Deserialize(tag);
    if(gameVersions != GameVersions::Unknown)
      return gameVersions;
  }
  return GameVersions::None;
}
