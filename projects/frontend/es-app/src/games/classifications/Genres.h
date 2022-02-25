//
// Created by bkg2k on 05/02/2020.
//
#pragma once

// References:
// https://www.idtech.com/blog/different-types-of-video-game-genres
// https://en.wikipedia.org/wiki/List_of_video_game_genres

#include <utils/os/fs/Path.h>
#include <utils/storage/HashMap.h>

enum class GameGenres : short // Must be an short
{
  None                            = 0x0000, //!< No genre
  Action                          = 0x0100, //!< Generic Action games
    ActionPlatformer              = 0x0101, //!< - Action platform game
    ActionPlatformShooter         = 0x0102, //!< - Action platform shooter (Turrican, ...)
    ActionFirstPersonShooter      = 0x0103, //!< - First person shooter (007, ...)
    ActionShootEmUp               = 0x0104, //!< - Shoot'em up/all (
    ActionShootWithGun            = 0x0105, //!< - On-screen shooters (Operation wolf, Duck Hunt, ...)
    ActionFighting                = 0x0106, //!< - Fighting games (mortal kombat, street fighters, ...)
    ActionBeatEmUp                = 0x0107, //!< - Beat'em up/all (Renegade, Double Dragon, ...)
    ActionStealth                 = 0x0108, //!< - Stealth combat (MGS, Dishonored, ...)
    ActionBattleRoyale            = 0x0109, //!< - Battle royale survivals (Fortnite, Apex legend, ...)
    ActionRythm                   = 0x010A, //!< - Music/Rythm games (Dance Dance Revolution, ...)
  Adventure                       = 0x0200, //!< Generic Adventure games
    AdventureText                 = 0x0201, //!< - Old-school text adventure (Zork, ...)
    AdventureGraphics             = 0x0202, //!< - Mainly Point-and-clicks
    AdventureVisualNovels         = 0x0203, //!< - Dating & legal simulation (Ace Attornay, ...)
    AdventureInteractiveMovie     = 0x0204, //!< - Interactive movies (Tex Murphy, Fahrenheit, RE4, ...)
    AdventureRealTime3D           = 0x0205, //!< - 3D adventures (Shenmue, Heavy rain, ...)
    AdventureSurvivalHorror       = 0x0206, //!< - Survivals/Horror Survivals (Lost in blue, Resident evil, ...)
  RPG                             = 0x0300, //!< Generic RPG (Role Playing Games)
    RPGAction                     = 0x0301, //!< - Action RPG (Diablo, ...)
    RPGMMO                        = 0x0302, //!< - Massive Multiplayer Online RPG (TESO, WoW, ...)
    RPGDungeonCrawler             = 0x0303, //!< - Dungeon Crawler (Dungeon Master, Eye of the beholder, ...)
    RPGTactical                   = 0x0304, //!< - Tactical RPG (Ogres Battle, FF Tactics, ...)
    RPGJapanese                   = 0x0305, //!< - Japaneese RPG, manga-like (Chrono Trigger, FF, ...)
    RPGFirstPersonPartyBased      = 0x0306, //!< - Team-as-one RPG (Ishar, Bard's tales, ...)
  Simulation                      = 0x0400, //!< Generic simulation
    SimulationBuildAndManagement  = 0x0401, //!< - Construction & Management simulations (Sim-city, ...)
    SimulationLife                = 0x0402, //!< - Life simulation (Nintendogs, Tamagoshi, Sims, ...)
    SimulationFishAndHunt         = 0x0403, //!< - Fighing and hunting (Deer hunting, Sega bass fishing, ...)
    SimulationVehicle             = 0x0404, //!< - Car/Planes/Tank/... simulations (Flight Simulator, Sherman M4, ...)
    SimulationSciFi               = 0x0405, //!< - Space Opera (Elite, Homeworld)
  Strategy                        = 0x0500, //!< Generic strategy games
    Strategy4X                    = 0x0501, //!< - eXplore, eXpand, eXploit, eXterminate (Civilization, ...)
    StrategyArtillery             = 0x0502, //!< - multiplayer artillery games, turn by turn (Scortched Tanks, Worms, ...)
    StrategyAutoBattler           = 0x0503, //!< - Auto-battle tacticals (Dota undergrounds, Heartstone Battlegrounds, ...)
    StrategyMOBA                  = 0x0504, //!< - Multiplayer Online Battle Arena (Dota 2, Smite, ...)
    StrategyRTS                   = 0x0505, //!< - Real Time Strategy (Warcrafs, Dune, C&C, ...)
    StrategyTBS                   = 0x0506, //!< - Turn based strategy (Might & Magic, Making History, ...)
    StrategyTowerDefense          = 0x0507, //!< - Tower defenses!
    StrategyWargame               = 0x0508, //!< - Military tactics
  Sports                          = 0x0600, //!< Generic sport games
    SportRacing                   = 0x0601, //!< - All racing games!
    SportSimulation               = 0x0602, //!< - All physical/simulation sports
    SportCompetitive              = 0x0603, //!< - High competitive factor (Ball Jack, ...)
    SportFight                    = 0x0604, //!< - Fighting sports/violent sports (SpeedBall, WWE 2K Fight Nights, ...)
  Pinball                         = 0x0700, //!< Pinball
  Board                           = 0x0800, //!< Board games (chess, backgammon, othello, ...)
  Casual                          = 0x0900, //!< Simple interaction games for casual gaming
  DigitalCard                     = 0x0A00, //!< Card Collection/games (Hearthstone, Magic the Gathering, ...)
  PuzzleAndLogic                  = 0x0B00, //!< Puzzle and logic games (Tetris, Sokoban, ...)
  Party                           = 0x0C00, //!< Multiplayer party games (Mario party, ...)
  Trivia                          = 0x0D00, //!< Answer/Quizz games (Family Feud, Are you smarter than a 5th grade, ...)
  Casino                          = 0x0E00, //!< Casino games
  Compilation                     = 0x0F00, //!< Multi games
  DemoScene                       = 0x1000, //!< Amiga/ST/PC Demo from demo scene
  Educative                       = 0x1100, //!< Educative games
};

class Genres
{
  public:
    typedef HashMap<GameGenres, const char*> GenreMap;
    typedef HashMap<GameGenres, Path> GenrePathMap;
    typedef std::vector<GameGenres> GenreList;

    static bool IsSubGenre(GameGenres genre) { return ((int)genre & 0xFF) != 0; }

    static bool TopGenreMatching(GameGenres sub, GameGenres top) { return ((int)sub >> 8) == ((int)top >> 8); }

    /*!
     * @brief Get genre resource path of the given genre
     * @param genre Game genre
     * @return Path
     */
    static const Path& GetResourcePath(GameGenres genre);

    /*!
     * @brief Get generic english name of the given genre
     * @param genre Game genre
     * @return Generic name (can be used as translation key)
     */
    static std::string GetName(GameGenres genre);

    /*!
     * @brief Lookup game genre from its name
     * @param name Name
     * @return GameGenre (None if not found)
     */
    static GameGenres LookupFromName(const std::string& name);

    /*!
     * @brief Get generic short name per genre
     * @return Short name (may be used in automatic configuration
     */
    static const GenreMap& GetShortNameMap();

    /*!
     * @brief Get generic english name per genre
     * @return Long name map
     */
    static const GenreMap& GetLongNameMap();

    /*!
     * @brief Get ordered genre list
     * @return Ordered genre list
     */
    static const GenreList& GetOrderedList();
};

