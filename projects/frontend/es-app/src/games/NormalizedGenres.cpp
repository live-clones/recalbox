//
// Created by bkg2k on 05/02/2020.
//

#include "NormalizedGenres.h"
#include <utils/Log.h>

Path NormalizedGenres::GetResourcePath(GameGenres genre)
{
  static GenreMap sNames
  ({
    { GameGenres::None                          , "" },
    { GameGenres::Action                        , "" },
    { GameGenres::  ActionPlatformer            , "" },
    { GameGenres::  ActionPlatformShooter       , "" },
    { GameGenres::  ActionFirstPersonShooter    , "" },
    { GameGenres::  ActionShootEmUp             , "" },
    { GameGenres::  ActionShootWithGun          , "" },
    { GameGenres::  ActionFighting              , "" },
    { GameGenres::  ActionBeatEmUp              , "" },
    { GameGenres::  ActionStealth               , "" },
    { GameGenres::  ActionBattleRoyale          , "" },
    { GameGenres::  ActionRythm                 , "" },
    { GameGenres::Adventure                     , "" },
    { GameGenres::  AdventureText               , "" },
    { GameGenres::  AdventureGraphics           , "" },
    { GameGenres::  AdventureVisualNovels       , "" },
    { GameGenres::  AdventureInteractiveMovie   , "" },
    { GameGenres::  AdventureRealTime3D         , "" },
    { GameGenres::  AdventureSurvivalHorror     , "" },
    { GameGenres::RPG                           , "" },
    { GameGenres::  RPGAction                   , "" },
    { GameGenres::  RPGMMO                      , "" },
    { GameGenres::  RPGDungeonCrawler           , "" },
    { GameGenres::  RPGTactical                 , "" },
    { GameGenres::  RPGJapanese                 , "" },
    { GameGenres::  RPGFirstPersonPartyBased    , "" },
    { GameGenres::Simulation                    , "" },
    { GameGenres::  SimulationBuildAndManagement, "" },
    { GameGenres::  SimulationLife              , "" },
    { GameGenres::  SimulationFishAndHunt       , "" },
    { GameGenres::  SimulationVehicle           , "" },
    { GameGenres::  SimulationSciFi             , "" },
    { GameGenres::Strategy                      , "" },
    { GameGenres::  Strategy4X                  , "" },
    { GameGenres::  StrategyArtillery           , "" },
    { GameGenres::  StrategyAutoBattler         , "" },
    { GameGenres::  StrategyMOBA                , "" },
    { GameGenres::  StrategyRTS                 , "" },
    { GameGenres::  StrategyTBS                 , "" },
    { GameGenres::  StrategyTowerDefense        , "" },
    { GameGenres::  StrategyWargame             , "" },
    { GameGenres::Sports                        , "" },
    { GameGenres::  SportRacing                 , "" },
    { GameGenres::  SportSimulation             , "" },
    { GameGenres::  SportCompetitive            , "" },
    { GameGenres::  SportFight                  , "" },
    { GameGenres::Flipper                       , "" },
    { GameGenres::Board                         , "" },
    { GameGenres::Casual                        , "" },
    { GameGenres::DigitalCard                   , "" },
    { GameGenres::PuzzleAndLogic                , "" },
    { GameGenres::Party                         , "" },
    { GameGenres::Trivia                        , "" },
    { GameGenres::Casino                        , "" },
    { GameGenres::Compilation                   , "" },
    { GameGenres::DemoScene                     , "" },
  });

  auto it = sNames.find(genre);
  if (it != sNames.end())
    return Path(it->second);

  LOG(LogError) << "[Resource path] Unknown GameGenre " << (int)genre;
  return Path::Empty;
}

std::string NormalizedGenres::GetName(GameGenres genre)
{
  static GenreMap sNames
  ({
    { GameGenres::None                          , "None"                                   },
    { GameGenres::Action                        , "Action"                                 },
    { GameGenres::  ActionPlatformer            , "Platform"                               },
    { GameGenres::  ActionPlatformShooter       , "Platform Shooter"                       },
    { GameGenres::  ActionFirstPersonShooter    , "First Person Shooter"                   },
    { GameGenres::  ActionShootEmUp             , "Shoot'em Up"                            },
    { GameGenres::  ActionShootWithGun          , "Shoot with Gun"                         },
    { GameGenres::  ActionFighting              , "Fighting"                               },
    { GameGenres::  ActionBeatEmUp              , "Beat'em All"                            },
    { GameGenres::  ActionStealth               , "Infiltration"                           },
    { GameGenres::  ActionBattleRoyale          , "Battle Royale"                          },
    { GameGenres::  ActionRythm                 , "Rythm & Music"                          },
    { GameGenres::Adventure                     , "Adventure"                              },
    { GameGenres::  AdventureText               , "Textual Adventure"                      },
    { GameGenres::  AdventureGraphics           , "Graphical Adventure"                    },
    { GameGenres::  AdventureVisualNovels       , "Visual Novel"                           },
    { GameGenres::  AdventureInteractiveMovie   , "Interactive Movie"                      },
    { GameGenres::  AdventureRealTime3D         , "Real Time 3D Adventure"                 },
    { GameGenres::  AdventureSurvivalHorror     , "Survival"                               },
    { GameGenres::RPG                           , "RPG"                                    },
    { GameGenres::  RPGAction                   , "Action RPG"                             },
    { GameGenres::  RPGMMO                      , "MMORPG"                                 },
    { GameGenres::  RPGDungeonCrawler           , "Dungeon Crawler"                        },
    { GameGenres::  RPGTactical                 , "Tactical RPG"                           },
    { GameGenres::  RPGJapanese                 , "JRPG"                                   },
    { GameGenres::  RPGFirstPersonPartyBased    , "Party based RPG"                        },
    { GameGenres::Simulation                    , "Simulation"                             },
    { GameGenres::  SimulationBuildAndManagement, "Build & Management"                     },
    { GameGenres::  SimulationLife              , "Life Simulation"                        },
    { GameGenres::  SimulationFishAndHunt       , "Fishing & Hunting"                      },
    { GameGenres::  SimulationVehicle           , "Vehicle Simulation"                     },
    { GameGenres::  SimulationSciFi             , "Science Fiction Simulation"             },
    { GameGenres::Strategy                      , "Strategy"                               },
    { GameGenres::  Strategy4X                  , "eXplore, eXpand, eXploit & eXterminate" },
    { GameGenres::  StrategyArtillery           , "Artillery"                              },
    { GameGenres::  StrategyAutoBattler         , "Auto-battler"                           },
    { GameGenres::  StrategyMOBA                , "Mulpiplayer Online Battle Arena"        },
    { GameGenres::  StrategyRTS                 , "Real Time Stratégy"                     },
    { GameGenres::  StrategyTBS                 , "Turn Based Strategy"                    },
    { GameGenres::  StrategyTowerDefense        , "Tower Defense"                          },
    { GameGenres::  StrategyWargame             , "Wargame"                                },
    { GameGenres::Sports                        , "Sports"                                 },
    { GameGenres::  SportRacing                 , "Racing"                                 },
    { GameGenres::  SportSimulation             , "Sport Simulation"                       },
    { GameGenres::  SportCompetitive            , "Competition Sport"                      },
    { GameGenres::  SportFight                  , "Fighting/Violent Sport"                 },
    { GameGenres::Flipper                       , "Flipper"                                },
    { GameGenres::Board                         , "Board game"                             },
    { GameGenres::Casual                        , "Casual game"                            },
    { GameGenres::DigitalCard                   , "Digital Cards"                          },
    { GameGenres::PuzzleAndLogic                , "Puzzle & Logic"                         },
    { GameGenres::Party                         , "Multiplayuer Party Game"                },
    { GameGenres::Trivia                        , "Trivia"                                 },
    { GameGenres::Casino                        , "Casino"                                 },
    { GameGenres::Compilation                   , "Multi Game Compilation"                 },
    { GameGenres::DemoScene                     , "Demo from Demo Screne"                  },
    { GameGenres::Educative                     , "Educative"                              },
  });

  auto it = sNames.find(genre);
  if (it != sNames.end())
    return it->second;

  LOG(LogError) << "[Name] Unknown GameGenre " << (int)genre;
  return "Uknown";
}

const NormalizedGenres::GenreMap& NormalizedGenres::GetShortNameMap()
{
  static GenreMap sNames
  ({
     { GameGenres::Action                        , "action"                       },
     { GameGenres::  ActionPlatformer            , "actionplatformer"             },
     { GameGenres::  ActionPlatformShooter       , "actionplatformshooter"        },
     { GameGenres::  ActionFirstPersonShooter    , "actionfirstpersonshooter"     },
     { GameGenres::  ActionShootEmUp             , "actionshootemup"              },
     { GameGenres::  ActionShootWithGun          , "actionshootwithgun"           },
     { GameGenres::  ActionFighting              , "actionfighting"               },
     { GameGenres::  ActionBeatEmUp              , "actionbeatemup"               },
     { GameGenres::  ActionStealth               , "actionstealth"                },
     { GameGenres::  ActionBattleRoyale          , "actionbattleroyale"           },
     { GameGenres::  ActionRythm                 , "actionrythm"                  },
     { GameGenres::Adventure                     , "adventure"                    },
     { GameGenres::  AdventureText               , "adventuretext"                },
     { GameGenres::  AdventureGraphics           , "adventuregraphics"            },
     { GameGenres::  AdventureVisualNovels       , "adventurevisualnovels"        },
     { GameGenres::  AdventureInteractiveMovie   , "adventureinteractivemovie"    },
     { GameGenres::  AdventureRealTime3D         , "adventurerealtime3d"          },
     { GameGenres::  AdventureSurvivalHorror     , "adventuresurvivalhorror"      },
     { GameGenres::RPG                           , "rpg"                          },
     { GameGenres::  RPGAction                   , "rpgaction"                    },
     { GameGenres::  RPGMMO                      , "rpgmmo"                       },
     { GameGenres::  RPGDungeonCrawler           , "rpgdungeoncrawler"            },
     { GameGenres::  RPGTactical                 , "rpgtactical"                  },
     { GameGenres::  RPGJapanese                 , "rpgjapanese"                  },
     { GameGenres::  RPGFirstPersonPartyBased    , "rpgfirstpersonpartybased"     },
     { GameGenres::Simulation                    , "simulation"                   },
     { GameGenres::  SimulationBuildAndManagement, "simulationbuildandmanagement" },
     { GameGenres::  SimulationLife              , "simulationlife"               },
     { GameGenres::  SimulationFishAndHunt       , "simulationfishandhunt"        },
     { GameGenres::  SimulationVehicle           , "simulationvehicle"            },
     { GameGenres::  SimulationSciFi             , "simulationscifi"              },
     { GameGenres::Strategy                      , "strategy"                     },
     { GameGenres::  Strategy4X                  , "strategy4x"                   },
     { GameGenres::  StrategyArtillery           , "strategyartillery"            },
     { GameGenres::  StrategyAutoBattler         , "strategyautobattler"          },
     { GameGenres::  StrategyMOBA                , "strategymoba"                 },
     { GameGenres::  StrategyRTS                 , "strategyrts"                  },
     { GameGenres::  StrategyTBS                 , "strategytbs"                  },
     { GameGenres::  StrategyTowerDefense        , "strategytowerdefense"         },
     { GameGenres::  StrategyWargame             , "strategywargame"              },
     { GameGenres::Sports                        , "sports"                       },
     { GameGenres::  SportRacing                 , "sportracing"                  },
     { GameGenres::  SportSimulation             , "sportsimulation"              },
     { GameGenres::  SportCompetitive            , "sportcompetitive"             },
     { GameGenres::  SportFight                  , "sportfight"                   },
     { GameGenres::Flipper                       , "flipper"                      },
     { GameGenres::Board                         , "board"                        },
     { GameGenres::Casual                        , "casual"                       },
     { GameGenres::DigitalCard                   , "digitalcard"                  },
     { GameGenres::PuzzleAndLogic                , "puzzleandlogic"               },
     { GameGenres::Party                         , "party"                        },
     { GameGenres::Trivia                        , "trivia"                       },
     { GameGenres::Casino                        , "casino"                       },
     { GameGenres::Compilation                   , "compilation"                  },
     { GameGenres::DemoScene                     , "demoscene"                    },
     { GameGenres::Educative                     , "educative"                    },
   });

  return sNames;
}
