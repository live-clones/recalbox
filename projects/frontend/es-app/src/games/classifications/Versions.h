//
// Created by gugue_u on 02/03/2022.
//




#include <string>
#include "games/FileData.h"

class Versions
{
  public:
    enum class GameVersions : unsigned int
    {
      Unknown = 0,
      Sample = 1,
      //Proto = 2,
      None = 50,

      Beta = 20,
      Beta1 = 21,
      Beta2 = 22,
      Beta3 = 23,
      Beta4 = 24,
      Beta5 = 25,
      Beta6 = 26,
      Beta7 = 27,
      Beta8 = 28,

      Rev = 100,
      Rev1 = 101,
      Rev2 = 102,
      Rev3 = 103,
      Rev4 = 104,
      Rev5 = 105,
      Rev6 = 106,
      Rev7 = 107,
      Rev8 = 108,

      VirtualConsole = 200,
    };

    GameVersions mGameVersion;
    int mVersionNumber;

    static Versions::GameVersions ExtractGameVersionNoIntro(std::string fileName);

    static Versions::GameVersions Deserialize(const std::string& tag);

    static std::string& Serialize(GameVersions versions);
};


