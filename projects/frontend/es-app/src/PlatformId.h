#pragma once

#include <map>

namespace PlatformIds
{
	enum PlatformId : unsigned int
	{
		PLATFORM_UNKNOWN = 0,

		THREEDO, // name can't start with a constant
		AMIGA,
		AMSTRAD_CPC,
		APPLE_II,
		ARCADE,
		ATARI_800,
		ATARI_2600,
		ATARI_5200,
		ATARI_7800,
		ATARI_LYNX,
		ATARI_ST, // Atari ST/STE/Falcon
		ATARI_JAGUAR,
		ATARI_JAGUAR_CD,
		ATARI_XE,
		COLECOVISION,
		COMMODORE_64,
		FAMILY_COMPUTER_DISK_SYSTEM,
		INTELLIVISION,
        	MACOS,
		XBOX,
		XBOX_360,
		MSX,
		MSX1,
		MSX2,
		NEOGEO,
		NEOGEO_POCKET,
		NEOGEO_POCKET_COLOR,
		NINTENDO_3DS,
		NINTENDO_64,
		NINTENDO_DS,
		NINTENDO_ENTERTAINMENT_SYSTEM,
		GAME_BOY,
		GAME_BOY_ADVANCE,
		GAME_BOY_COLOR,
		NINTENDO_GAMECUBE,
		NINTENDO_WII,
		NINTENDO_WII_U,
		PC,
		SEGA_32X,
		SEGA_CD,
		SEGA_DREAMCAST,
		SEGA_GAME_GEAR,
		SEGA_GENESIS,
		SEGA_MASTER_SYSTEM,
		SEGA_MEGA_DRIVE,
		SEGA_SATURN,
		PLAYSTATION,
		PLAYSTATION_2,
		PLAYSTATION_3,
		PLAYSTATION_4,
		PLAYSTATION_VITA,
		PLAYSTATION_PORTABLE,
		SG_1000,
		SUPER_NINTENDO,
		TURBOGRAFX_16, // (also PC Engine)
		WONDERSWAN,
		WONDERSWAN_COLOR,
		ZX_SPECTRUM,
		VIRTUAL_BOY,
		GAME_AND_WATCH,
		PC_ENGINE_CD,
		SUPERGRAFX,
		PRBOOM,
		VECTREX,
		LUTRO,
		CAVE_STORY,
		ODYSSEY_2,
		ZX_81,
		MOONLIGHT,
		SCUMMVM,
		THOMSON_MO,
		THOMSON_TO,

		PLATFORM_IGNORE, // do not allow scraping for this system
		PLATFORM_COUNT
	};

	PlatformId getPlatformId(const char* str);
	const char* getPlatformName(PlatformId id);

	const char* getCleanMameName(const char* from);
}
