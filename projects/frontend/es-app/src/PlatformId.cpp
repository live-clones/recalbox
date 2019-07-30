#include <cstring>
#include "PlatformId.h"

extern const char* mameNameToRealName[];

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
      { "c64"             , PlatformId::COMMODORE_64              }, // commodore 64,
      { "amiga"           , PlatformId::COMMODORE_AMIGA           },
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
      { "pc88"            , PlatformId::NEC_PC88                  },
      { "pc98"            , PlatformId::NEC_PC98                  },
      { "pcengine"        , PlatformId::NEC_PCENGINE              }, // turbografx-16/pcengine
      { "pcenginecd"      , PlatformId::NEC_PCENGINE_CD           },
      { "pcfx"            , PlatformId::NEC_PCFX                  },
      { "supergrafx"      , PlatformId::NEC_SUPERGRAFX            },
      { "neogeo"          , PlatformId::NEOGEO                    },
      { "neogeocd"        , PlatformId::NEOGEO_CD                 },
      { "ngp"             , PlatformId::NEOGEO_POCKET             }, // neo geo pocket
      { "ngpc"            , PlatformId::NEOGEO_POCKET_COLOR       }, // neo geo pocket color
      { "n3ds"            , PlatformId::NINTENDO_3DS              }, // nintendo 3DS
      { "n64"             , PlatformId::NINTENDO_64               }, // Nintendo 64
      { "nds"             , PlatformId::NINTENDO_DS               }, // nintendo DS
      { "fds"             , PlatformId::NINTENDO_FDS              }, // Nintendo Family Computer Disk System
      { "gw"              , PlatformId::NINTENDO_GAME_AND_WATCH   },
      { "gb"              , PlatformId::NINTENDO_GAMEBOY          }, // game boy
      { "gba"             , PlatformId::NINTENDO_GAMEBOY_ADVANCE  }, // game boy advance
      { "gbc"             , PlatformId::NINTENDO_GAMEBOY_COLOR    }, // game boy color
      { "gc"              , PlatformId::NINTENDO_GAMECUBE         }, // gamecube
      { "nes"             , PlatformId::NINTENDO_NES              }, // nintendo entertainment system
      { "pokemini"        , PlatformId::NINTENDO_POKEMINI         },
      { "satellaview"     , PlatformId::NINTENDO_SATELLAVIEW      },
      { "snes"            , PlatformId::NINTENDO_SNES             }, // super nintendo entertainment system
      { "sufami"          , PlatformId::NINTENDO_SUFAMITURBO      },
      { "virtualboy"      , PlatformId::NINTENDO_VIRTUAL_BOY      },
      { "wii"             , PlatformId::NINTENDO_WII              },
      { "wiiu"            , PlatformId::NINTENDO_WII_U            },
      { "uzebox"          , PlatformId::OSH_UZEBOX                },
      { "3do"             , PlatformId::PANASONIC_3DO             },
      { "prboom"          , PlatformId::PORT_PRBOOM               },
      { "cavestory"       , PlatformId::PORT_CAVE_STORY           },
      { "sega32x"         , PlatformId::SEGA_32X                  },
      { "segacd"          , PlatformId::SEGA_CD                   },
      { "dreamcast"       , PlatformId::SEGA_DREAMCAST            },
      { "gamegear"        , PlatformId::SEGA_GAME_GEAR            },
      { "genesis"         , PlatformId::SEGA_GENESIS              }, // sega genesis
      { "mastersystem"    , PlatformId::SEGA_MASTER_SYSTEM        }, // sega master system
      { "megadrive"       , PlatformId::SEGA_MEGA_DRIVE           }, // sega megadrive
      { "saturn"          , PlatformId::SEGA_SATURN               }, // sega saturn
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
      { "moonlight"       , PlatformId::STREAM_MOONLIGHT          },
      { "oricatmos"       , PlatformId::TANDERINE_ORICATMOS       },
      { "thomsonmoto"     , PlatformId::THOMSON_MOTO              },
                                                        
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
    for(auto& it : getScrappingPlatforms())
      if (it.second == id)
        return it.first;

    return "Unknown";
  }

  const char* getCleanMameName(const char* from)
  {
    const char** mameNames = mameNameToRealName;

    while(*mameNames != nullptr && strcmp(from, *mameNames) != 0)
      mameNames += 2;

    if(*mameNames)
      return *(mameNames + 1);

    return from;
  }
}
