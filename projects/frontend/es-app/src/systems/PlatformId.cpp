#include "PlatformId.h"
#include <map>

namespace PlatformIds
{
  static const std::map<std::string, PlatformId>& getScrappingPlatforms()
  {
    static std::map<std::string, PlatformId> _ScrappingPlatforms =
    {
      { "unknown"         , PlatformId::PLATFORM_UNKNOWN          }, // nothing set
                                                           
      { "amstradcpc"      , PlatformId::AMSTRAD_CPC               },
      { "gx4000"          , PlatformId::AMSTRAD_GX4000            },
      { "apple2"          , PlatformId::APPLE_II                  },
      { "apple2gs"        , PlatformId::APPLE_IIGS                },
      { "macintosh"       , PlatformId::APPLE_MACOS               },
      { "arcade"          , PlatformId::ARCADE                    },
      { "atari800"        , PlatformId::ATARI_8BITS               },
      { "atari2600"       , PlatformId::ATARI_2600                },
      { "atari5200"       , PlatformId::ATARI_5200                },
      { "atari7800"       , PlatformId::ATARI_7800                },
      { "atarilynx"       , PlatformId::ATARI_LYNX                },
      { "atarist"         , PlatformId::ATARI_ST                  },
      { "atarijaguar"     , PlatformId::ATARI_JAGUAR              },
      { "atarijaguarcd"   , PlatformId::ATARI_JAGUAR_CD           },
      { "wonderswan"      , PlatformId::BANDAI_WONDERSWAN         },
      { "wonderswancolor" , PlatformId::BANDAI_WONDERSWAN_COLOR   },
      { "colecovision"    , PlatformId::COLECOVISION              },
      { "vic20"           , PlatformId::COMMODORE_VIC20           }, // Commodode VIC20,
      { "c64"             , PlatformId::COMMODORE_64              }, // Commodore 64,
      { "amiga"           , PlatformId::COMMODORE_AMIGA           },
      { "amigacd32"       , PlatformId::COMMODORE_CD32            },
      { "amigacdtv"       , PlatformId::COMMODORE_CDTV            },
      { "bk"              , PlatformId::ELEKTRONIKA_BK            },
      { "scv"             , PlatformId::EPOCH_SUPERCASSETTEVISION },
      { "channelf"        , PlatformId::FAIRCHILD_CHANNELF        },
      { "daphne"          , PlatformId::GAMEENGINE_DAPHNE         },
      { "lutro"           , PlatformId::GAMEENGINE_LUTRO          },
      { "openbor"         , PlatformId::GAMEENGINE_OPENBOR        },
      { "scummvm"         , PlatformId::GAMEENGINE_SCUMMVM        },
      { "solarus"         , PlatformId::GAMEENGINE_SOLARUS        },
      { "tic80"           , PlatformId::GAMEENGINE_TIC80          },
      { "vectrex"         , PlatformId::GCE_VECTREX               },
      { "pc"              , PlatformId::IBM_PC                    },
      { "odyssey2"        , PlatformId::MAGNAVOX_ODYSSEY2         },
      { "intellivision"   , PlatformId::MATTEL_INTELLIVISION      },
      { "samcoupe"        , PlatformId::MGT_SAMCOUPE              },
      { "xbox"            , PlatformId::MICROSOFT_XBOX            },
      { "xbox360"         , PlatformId::MICROSOFT_XBOX_360        },
      { "msx"             , PlatformId::MICROSOFT_MSX             },
      { "msx1"            , PlatformId::MICROSOFT_MSX1            },
      { "msx2"            , PlatformId::MICROSOFT_MSX2            },
      { "msxturbor"       , PlatformId::MICROSOFT_MSXTURBOR       },
      { "pc88"            , PlatformId::NEC_PC88                  },
      { "pc98"            , PlatformId::NEC_PC98                  },
      { "pcengine"        , PlatformId::NEC_PCENGINE              }, // Turbografx-16/PCEngine
      { "pcenginecd"      , PlatformId::NEC_PCENGINE_CD           },
      { "pcfx"            , PlatformId::NEC_PCFX                  },
      { "supergrafx"      , PlatformId::NEC_SUPERGRAFX            },
      { "neogeo"          , PlatformId::NEOGEO                    },
      { "neogeocd"        , PlatformId::NEOGEO_CD                 },
      { "ngp"             , PlatformId::NEOGEO_POCKET             }, // Neo Geo Pocket
      { "ngpc"            , PlatformId::NEOGEO_POCKET_COLOR       }, // Neo geo Pocket Color
      { "n3ds"            , PlatformId::NINTENDO_3DS              }, // nintendo 3DS
      { "n64"             , PlatformId::NINTENDO_64               }, // Nintendo 64
      { "nds"             , PlatformId::NINTENDO_DS               }, // Nintendo DS
      { "fds"             , PlatformId::NINTENDO_FDS              }, // Nintendo Family Computer Disk System
      { "gw"              , PlatformId::NINTENDO_GAME_AND_WATCH   },
      { "gb"              , PlatformId::NINTENDO_GAMEBOY          }, // Game Boy
      { "gba"             , PlatformId::NINTENDO_GAMEBOY_ADVANCE  }, // Game Boy Advance
      { "gbc"             , PlatformId::NINTENDO_GAMEBOY_COLOR    }, // Game Boy Color
      { "gc"              , PlatformId::NINTENDO_GAMECUBE         }, // Gamecube
      { "nes"             , PlatformId::NINTENDO_NES              }, // Nintendo Entertainment System
      { "pokemini"        , PlatformId::NINTENDO_POKEMINI         },
      { "satellaview"     , PlatformId::NINTENDO_SATELLAVIEW      },
      { "snes"            , PlatformId::NINTENDO_SNES             }, // Super Nintendo Entertainment System
      { "sufami"          , PlatformId::NINTENDO_SUFAMITURBO      },
      { "virtualboy"      , PlatformId::NINTENDO_VIRTUAL_BOY      },
      { "wii"             , PlatformId::NINTENDO_WII              },
      { "wiiu"            , PlatformId::NINTENDO_WII_U            },
      { "uzebox"          , PlatformId::OSH_UZEBOX                },
      { "palm"            , PlatformId::PALM_PDA                  },
      { "3do"             , PlatformId::PANASONIC_3DO             },
      { "videopacplus"    , PlatformId::PHILIPS_VIDEOPACPLUS      },
      { "pcv2"            , PlatformId::POCKET_CHALLENGE_V2       },
      { "atomiswave"      , PlatformId::SAMMY_ATOMISWAVE          },
      { "sega32x"         , PlatformId::SEGA_32X                  },
      { "segacd"          , PlatformId::SEGA_CD                   },
      { "dreamcast"       , PlatformId::SEGA_DREAMCAST            },
      { "gamegear"        , PlatformId::SEGA_GAME_GEAR            },
      { "genesis"         , PlatformId::SEGA_GENESIS              }, // Sega Genesis
      { "mastersystem"    , PlatformId::SEGA_MASTER_SYSTEM        }, // Sega Master system
      { "megadrive"       , PlatformId::SEGA_MEGA_DRIVE           }, // Sega Megadrive
      { "model3"          , PlatformId::SEGA_MODEL3               },
      { "naomi"           , PlatformId::SEGA_NAOMI                }, // Sega Arcade
      { "naomigd"         , PlatformId::SEGA_NAOMI                }, // Sega Arcade
      { "saturn"          , PlatformId::SEGA_SATURN               }, // Sega Saturn
      { "sg1000"          , PlatformId::SEGA_SG1000               }, // Sega SG1000
      { "x1"              , PlatformId::SHARP_X1                  },
      { "x68000"          , PlatformId::SHARP_X68000              },
      { "psx"             , PlatformId::SONY_PLAYSTATION          },
      { "ps2"             , PlatformId::SONY_PLAYSTATION_2        },
      { "ps3"             , PlatformId::SONY_PLAYSTATION_3        },
      { "ps4"             , PlatformId::SONY_PLAYSTATION_4        },
      { "psvita"          , PlatformId::SONY_PLAYSTATION_VITA     },
      { "psp"             , PlatformId::SONY_PLAYSTATION_PORTABLE }, // Playstation portable
      { "zxspectrum"      , PlatformId::SINCLAIR_ZX_SPECTRUM      },
      { "zx81"            , PlatformId::SINCLAIR_ZX_81            },
      { "spectravideo"    , PlatformId::SPECTRAVISION_SPECTRAVIDEO},
      { "moonlight"       , PlatformId::STREAM_MOONLIGHT          },
      { "oricatmos"       , PlatformId::TANDERINE_ORICATMOS       },
      { "thomsonmoto"     , PlatformId::THOMSON_MOTO              },
      { "supervision"     , PlatformId::WATARA_SUPERVISION        },
      { "2048"            , PlatformId::PORT_2048                 },
      { "cavestory"       , PlatformId::PORT_CAVE_STORY           },
      { "dinothawr"       , PlatformId::PORT_DINOTHAWR            },
      { "doom"            , PlatformId::PORT_PRBOOM               },
      { "doom3"           , PlatformId::PORT_BOOM3                },
      { "dungeoncrawlstonesoup" , PlatformId::PORT_CRAWL          },
      { "flashback"       , PlatformId::PORT_REMINISCENCE         },
      { "minecraft"       , PlatformId::PORT_CRAFT                },
      { "mrboom"          , PlatformId::PORT_MRBOOM               },
      { "outrun"          , PlatformId::PORT_CANNONBALL           },
      { "quake"           , PlatformId::PORT_TYRQUAKE             },
      { "quake2"          , PlatformId::PORT_VITAQUAKE2           },
      { "quake3"          , PlatformId::PORT_VITAQUAKE3           },
      { "rickdangerous"   , PlatformId::PORT_XRICK                },
      { "startrekvoyager" , PlatformId::PORT_VITAVOYAGER          },
      { "supermariowar"   , PlatformId::PORT_SMW                  },
      { "thepowdertoy"    , PlatformId::PORT_THEPOWDERTOY         },
      { "tombraider"      , PlatformId::PORT_OPENLARA             },
      { "wolfenstein3d"   , PlatformId::PORT_ECWOLF               },
                                                        
      { "ignore"          , PlatformId::PLATFORM_IGNORE           }, // Do not allow scraping for this system
      { "invalid"         , PlatformId::PLATFORM_COUNT            }, // Invalid
    };

    return _ScrappingPlatforms;
  }

  PlatformId getPlatformId(const std::string& str)
  {
    const std::map<std::string, PlatformId>& platforms = getScrappingPlatforms();

    auto it = platforms.find(str);
    if (it != platforms.end())
      return (*it).second;

    return PlatformId::PLATFORM_UNKNOWN;
  }

  std::string getPlatformName(PlatformId id)
  {
    for(const auto& it : getScrappingPlatforms())
      if (it.second == id)
        return it.first;

    return "Unknown";
  }
}
