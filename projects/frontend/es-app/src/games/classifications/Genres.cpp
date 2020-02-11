//
// Created by bkg2k on 05/02/2020.
//

#include "Genres.h"
#include <utils/Log.h>

const Path& Genres::GetResourcePath(GameGenres genre)
{
  static GenrePathMap sNames
  ({
    { GameGenres::None                          , Path::Empty },
    { GameGenres::Action                        , Path(":/genre/action.svg") },
    { GameGenres::  ActionPlatformer            , Path(":/genre/actionplatformer.svg") },
    { GameGenres::  ActionPlatformShooter       , Path(":/genre/actionplatformshooter.svg") },
    { GameGenres::  ActionFirstPersonShooter    , Path(":/genre/actionfirstpersonshooter.svg") },
    { GameGenres::  ActionShootEmUp             , Path(":/genre/actionshootemup.svg") },
    { GameGenres::  ActionShootWithGun          , Path(":/genre/actionshootwithgun.svg") },
    { GameGenres::  ActionFighting              , Path(":/genre/actionfighting.svg") },
    { GameGenres::  ActionBeatEmUp              , Path(":/genre/actionbeatemup.svg") },
    { GameGenres::  ActionStealth               , Path(":/genre/actionstealth.svg") },
    { GameGenres::  ActionBattleRoyale          , Path(":/genre/actionbattleroyale.svg") },
    { GameGenres::  ActionRythm                 , Path(":/genre/actionrythm.svg") },
    { GameGenres::Adventure                     , Path(":/genre/adventure.svg") },
    { GameGenres::  AdventureText               , Path(":/genre/adventuretext.svg") },
    { GameGenres::  AdventureGraphics           , Path(":/genre/adventuregraphical.svg") },
    { GameGenres::  AdventureVisualNovels       , Path(":/genre/adventurevisualnovel.svg") },
    { GameGenres::  AdventureInteractiveMovie   , Path(":/genre/adventureinteractivemovie.svg") },
    { GameGenres::  AdventureRealTime3D         , Path(":/genre/adventurerealtime3d.svg") },
    { GameGenres::  AdventureSurvivalHorror     , Path(":/genre/adventuresurvivalhorror.svg") },
    { GameGenres::RPG                           , Path(":/genre/rpg.svg") },
    { GameGenres::  RPGAction                   , Path(":/genre/rpgaction.svg") },
    { GameGenres::  RPGMMO                      , Path(":/genre/rpgmmo.svg") },
    { GameGenres::  RPGDungeonCrawler           , Path(":/genre/rpgdungeoncrawler.svg") },
    { GameGenres::  RPGTactical                 , Path(":/genre/rpgtactical.svg") },
    { GameGenres::  RPGJapanese                 , Path(":/genre/rpgjapanese.svg") },
    { GameGenres::  RPGFirstPersonPartyBased    , Path(":/genre/rpgfirstpersonpartybased.svg") },
    { GameGenres::Simulation                    , Path(":/genre/simulation.svg") },
    { GameGenres::  SimulationBuildAndManagement, Path(":/genre/simulationbuildandmanagement.svg") },
    { GameGenres::  SimulationLife              , Path(":/genre/simulationlife.svg") },
    { GameGenres::  SimulationFishAndHunt       , Path(":/genre/simulationfishandhunt.svg") },
    { GameGenres::  SimulationVehicle           , Path(":/genre/simulationvehicle.svg") },
    { GameGenres::  SimulationSciFi             , Path(":/genre/simulationscifi.svg") },
    { GameGenres::Strategy                      , Path(":/genre/strategy.svg") },
    { GameGenres::  Strategy4X                  , Path(":/genre/strategy4x.svg") },
    { GameGenres::  StrategyArtillery           , Path(":/genre/strategyartillery.svg") },
    { GameGenres::  StrategyAutoBattler         , Path(":/genre/strategyautobattler.svg") },
    { GameGenres::  StrategyMOBA                , Path(":/genre/strategymoba.svg") },
    { GameGenres::  StrategyRTS                 , Path(":/genre/strategyrts.svg") },
    { GameGenres::  StrategyTBS                 , Path(":/genre/strategytbs.svg") },
    { GameGenres::  StrategyTowerDefense        , Path(":/genre/strategytowerdefense.svg") },
    { GameGenres::  StrategyWargame             , Path(":/genre/strategywargame.svg") },
    { GameGenres::Sports                        , Path(":/genre/sports.svg") },
    { GameGenres::  SportRacing                 , Path(":/genre/sportracing.svg") },
    { GameGenres::  SportSimulation             , Path(":/genre/sportsimulation.svg") },
    { GameGenres::  SportCompetitive            , Path(":/genre/sportcompetitive.svg") },
    { GameGenres::  SportFight                  , Path(":/genre/sportfight.svg") },
    { GameGenres::Pinball                       , Path(":/genre/flipper.svg") },
    { GameGenres::Board                         , Path(":/genre/board.svg") },
    { GameGenres::Casual                        , Path(":/genre/casual.svg") },
    { GameGenres::DigitalCard                   , Path(":/genre/digitalcard.svg") },
    { GameGenres::PuzzleAndLogic                , Path(":/genre/puzzleandlogic.svg") },
    { GameGenres::Party                         , Path(":/genre/party.svg") },
    { GameGenres::Trivia                        , Path(":/genre/trivia.svg") },
    { GameGenres::Casino                        , Path(":/genre/casino.svg") },
    { GameGenres::Compilation                   , Path(":/genre/compilation.svg") },
    { GameGenres::DemoScene                     , Path(":/genre/demoscene.svg") },
    { GameGenres::Educative                     , Path(":/genre/educative.svg") }
  });

  Path* found = sNames.try_get(genre);
  if (found != nullptr)
    return *found;

  LOG(LogError) << "[Resource path] Unknown GameGenre " << (int)genre;
  return Path::Empty;
}

std::string Genres::GetName(GameGenres genre)
{
  static GenreMap sNames
  ({
    { GameGenres::None                          , "None"                                   },
    { GameGenres::Action                        , "Action (All)"                           },
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
    { GameGenres::Adventure                     , "Adventure (All)"                        },
    { GameGenres::  AdventureText               , "Textual Adventure"                      },
    { GameGenres::  AdventureGraphics           , "Graphical Adventure"                    },
    { GameGenres::  AdventureVisualNovels       , "Visual Novel"                           },
    { GameGenres::  AdventureInteractiveMovie   , "Interactive Movie"                      },
    { GameGenres::  AdventureRealTime3D         , "Real Time 3D Adventure"                 },
    { GameGenres::  AdventureSurvivalHorror     , "Survival"                               },
    { GameGenres::RPG                           , "RPG (All)"                              },
    { GameGenres::  RPGAction                   , "Action RPG"                             },
    { GameGenres::  RPGMMO                      , "MMORPG"                                 },
    { GameGenres::  RPGDungeonCrawler           , "Dungeon Crawler"                        },
    { GameGenres::  RPGTactical                 , "Tactical RPG"                           },
    { GameGenres::  RPGJapanese                 , "JRPG"                                   },
    { GameGenres::  RPGFirstPersonPartyBased    , "Party based RPG"                        },
    { GameGenres::Simulation                    , "Simulation (All)"                       },
    { GameGenres::  SimulationBuildAndManagement, "Build & Management"                     },
    { GameGenres::  SimulationLife              , "Life Simulation"                        },
    { GameGenres::  SimulationFishAndHunt       , "Fishing & Hunting"                      },
    { GameGenres::  SimulationVehicle           , "Vehicle Simulation"                     },
    { GameGenres::  SimulationSciFi             , "Science Fiction Simulation"             },
    { GameGenres::Strategy                      , "Strategy (All)"                         },
    { GameGenres::  Strategy4X                  , "eXplore, eXpand, eXploit & eXterminate" },
    { GameGenres::  StrategyArtillery           , "Artillery"                              },
    { GameGenres::  StrategyAutoBattler         , "Auto-battler"                           },
    { GameGenres::  StrategyMOBA                , "Mulpiplayer Online Battle Arena"        },
    { GameGenres::  StrategyRTS                 , "Real Time Stratégy"                     },
    { GameGenres::  StrategyTBS                 , "Turn Based Strategy"                    },
    { GameGenres::  StrategyTowerDefense        , "Tower Defense"                          },
    { GameGenres::  StrategyWargame             , "Wargame"                                },
    { GameGenres::Sports                        , "Sports (All)"                           },
    { GameGenres::  SportRacing                 , "Racing"                                 },
    { GameGenres::  SportSimulation             , "Sport Simulation"                       },
    { GameGenres::  SportCompetitive            , "Competition Sport"                      },
    { GameGenres::  SportFight                  , "Fighting/Violent Sport"                 },
    { GameGenres::Pinball                       , "Pinball"                                },
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

  const char** found = sNames.try_get(genre);
  if (found != nullptr)
    return *found;

  LOG(LogError) << "[Name] Unknown GameGenre " << (int)genre;
  return "Uknown";
}

const Genres::GenreMap& Genres::GetShortNameMap()
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
     { GameGenres::Pinball                       , "pinball"                      },
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
