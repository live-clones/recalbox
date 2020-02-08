//
// Created by bkg2k on 05/02/2020.
//

#include "Genres.h"
#include <utils/Log.h>

Path Genres::GetResourcePath(GameGenres genre)
{
  static GenreMap sNames
  ({
    { GameGenres::None                          , "" },
    { GameGenres::Action                        , ":/genre/action.svg" },
    { GameGenres::  ActionPlatformer            , ":/genre/actionplatformer.svg" },
    { GameGenres::  ActionPlatformShooter       , ":/genre/actionplatformshooter.svg" },
    { GameGenres::  ActionFirstPersonShooter    , ":/genre/actionfirstpersonshooter.svg" },
    { GameGenres::  ActionShootEmUp             , ":/genre/actionshootemup.svg" },
    { GameGenres::  ActionShootWithGun          , ":/genre/actionshootwithgun.svg" },
    { GameGenres::  ActionFighting              , ":/genre/actionfighting.svg" },
    { GameGenres::  ActionBeatEmUp              , ":/genre/actionbeatemup.svg" },
    { GameGenres::  ActionStealth               , ":/genre/actionstealth.svg" },
    { GameGenres::  ActionBattleRoyale          , ":/genre/actionbattleroyale.svg" },
    { GameGenres::  ActionRythm                 , ":/genre/actionrythm.svg" },
    { GameGenres::Adventure                     , ":/genre/adventure.svg" },
    { GameGenres::  AdventureText               , ":/genre/adventuretext.svg" },
    { GameGenres::  AdventureGraphics           , ":/genre/adventuregraphical.svg" },
    { GameGenres::  AdventureVisualNovels       , ":/genre/adventurevisualnovel.svg" },
    { GameGenres::  AdventureInteractiveMovie   , ":/genre/adventureinteractivemovie.svg" },
    { GameGenres::  AdventureRealTime3D         , ":/genre/adventurerealtime3d.svg" },
    { GameGenres::  AdventureSurvivalHorror     , ":/genre/adventuresurvivalhorror.svg" },
    { GameGenres::RPG                           , ":/genre/rpg.svg" },
    { GameGenres::  RPGAction                   , ":/genre/rpgaction.svg" },
    { GameGenres::  RPGMMO                      , ":/genre/rpgmmo.svg" },
    { GameGenres::  RPGDungeonCrawler           , ":/genre/rpgdungeoncrawler.svg" },
    { GameGenres::  RPGTactical                 , ":/genre/rpgtactical.svg" },
    { GameGenres::  RPGJapanese                 , ":/genre/rpgjapanese.svg" },
    { GameGenres::  RPGFirstPersonPartyBased    , ":/genre/rpgfirstpersonpartybased.svg" },
    { GameGenres::Simulation                    , ":/genre/simulation.svg" },
    { GameGenres::  SimulationBuildAndManagement, ":/genre/simulationbuildandmanagement.svg" },
    { GameGenres::  SimulationLife              , ":/genre/simulationlife.svg" },
    { GameGenres::  SimulationFishAndHunt       , ":/genre/simulationfishandhunt.svg" },
    { GameGenres::  SimulationVehicle           , ":/genre/simulationvehicle.svg" },
    { GameGenres::  SimulationSciFi             , ":/genre/simulationscifi.svg" },
    { GameGenres::Strategy                      , ":/genre/strategy.svg" },
    { GameGenres::  Strategy4X                  , ":/genre/strategy4x.svg" },
    { GameGenres::  StrategyArtillery           , ":/genre/strategyartillery.svg" },
    { GameGenres::  StrategyAutoBattler         , ":/genre/strategyautobattler.svg" },
    { GameGenres::  StrategyMOBA                , ":/genre/strategymoba.svg" },
    { GameGenres::  StrategyRTS                 , ":/genre/strategyrts.svg" },
    { GameGenres::  StrategyTBS                 , ":/genre/strategytbs.svg" },
    { GameGenres::  StrategyTowerDefense        , ":/genre/strategytowerdefense.svg" },
    { GameGenres::  StrategyWargame             , ":/genre/strategywargame.svg" },
    { GameGenres::Sports                        , ":/genre/sports.svg" },
    { GameGenres::  SportRacing                 , ":/genre/sportracing.svg" },
    { GameGenres::  SportSimulation             , ":/genre/sportsimulation.svg" },
    { GameGenres::  SportCompetitive            , ":/genre/sportcompetitive.svg" },
    { GameGenres::  SportFight                  , ":/genre/sportfight.svg" },
    { GameGenres::Flipper                       , ":/genre/flipper.svg" },
    { GameGenres::Board                         , ":/genre/board.svg" },
    { GameGenres::Casual                        , ":/genre/casual.svg" },
    { GameGenres::DigitalCard                   , ":/genre/digitalcard.svg" },
    { GameGenres::PuzzleAndLogic                , ":/genre/puzzleandlogic.svg" },
    { GameGenres::Party                         , ":/genre/party.svg" },
    { GameGenres::Trivia                        , ":/genre/trivia.svg" },
    { GameGenres::Casino                        , ":/genre/casino.svg" },
    { GameGenres::Compilation                   , ":/genre/compilation.svg" },
    { GameGenres::DemoScene                     , ":/genre/demoscene.svg" },
    { GameGenres::Educative                     , ":/genre/educative.svg"}
  });

  auto it = sNames.find(genre);
  if (it != sNames.end())
    return Path(it->second);

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
