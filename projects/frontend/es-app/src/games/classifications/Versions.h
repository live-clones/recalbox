//
// Created by gugue_u on 02/03/2022.
//
#include <string>

class Versions
{
  public:
    enum class GameVersions : unsigned int
    {
      Unknown = 0,

      Sample = 1,
      Sample1 = 1,
      Sample2 = 2,

      Proto = 11,
      Proto1 = 11,
      Proto2 = 12,

      Beta = 20,
      Beta1 = 21,
      Beta2 = 22,
      Beta3 = 23,
      Beta4 = 24,
      Beta5 = 25,
      Beta6 = 26,
      Beta7 = 27,
      Beta8 = 28,

      None = 50,

      Rev = 100,
      Rev1 = 101,
      Rev2 = 102,
      Rev3 = 103,
      Rev4 = 104,
      Rev5 = 105,
      Rev6 = 106,
      Rev7 = 107,
      Rev8 = 108,

      RevA = 201,
      RevB = 202,
      RevC = 203,
      RevD = 204,
      RevE = 205,
      RevF = 206,
      RevG = 207,
      RevH = 208,

      V1_1 = 301,
      V1_2 = 302,
      V1_3 = 303,
      V1_4 = 304,
      V1_5 = 305,
      V1_6 = 306,
      V1_7 = 307,
      V1_8 = 308,
      V1_9 = 308,

      V2_1 = 401,
      V2_2 = 402,
      V2_3 = 403,
      V2_4 = 404,
      V2_5 = 405,
      V2_6 = 406,
      V2_7 = 407,
      V2_8 = 408,
      V2_9 = 408,

      VirtualConsole = 1000,
    };

    GameVersions mGameVersion;
    int mVersionNumber;

    /*!
     * @brief extract game version from no intro game
     * @param File name
     * @return game versions
     */
    static Versions::GameVersions ExtractGameVersionNoIntro(std::string fileName);

    /*!
     * @brief Deserialize game version
     * @param string tag
     * @return GameVersions
     */
    static Versions::GameVersions Deserialize(const std::string& tag);

    /*!
     * @brief Deserialize game version
     * @param game version
     * @return game version has string
     */
    static std::string& Serialize(GameVersions versions);
};


