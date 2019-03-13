#include "scrapers/GamesDBScraper.h"
#include "Log.h"
#include "pugixml/pugixml.hpp"
#include "Settings.h"
#include "Util.h"
#include <boost/assign.hpp>

using namespace PlatformIds;
const std::map<PlatformId, const char*> gamesdb_platformid_map = boost::assign::map_list_of
	(PANASONIC_3DO, "3DO")
	(COMMODORE_AMIGA, "Amiga")
	(AMSTRAD_CPC, "Amstrad CPC")
	(APPLE_II, "Apple II")
	(ARCADE, "Arcade")
	(ATARI_8BITS, "Atari 800")
	(ATARI_2600, "Atari 2600")
	(ATARI_5200, "Atari 5200")
	(ATARI_7800, "Atari 7800")
	(ATARI_LYNX, "Atari Lynx")
	(ATARI_ST, "Atari ST")
	(ATARI_JAGUAR, "Atari Jaguar")
	(ATARI_JAGUAR_CD, "Atari Jaguar CD")
	(COLECOVISION, "Colecovision")
	(COMMODORE_64, "Commodore 64")
	(NINTENDO_FDS, "Famicom Disk System")
	(MATTEL_INTELLIVISION, "Intellivision")
	(APPLE_MACOS, "Mac OS")
	(MICROSOFT_XBOX, "Microsoft Xbox")
	(MICROSOFT_XBOX_360, "Microsoft Xbox 360")
	(MICROSOFT_MSX, "MSX")
	(NEOGEO, "NeoGeo")
	(NEOGEO_POCKET, "Neo Geo Pocket")
	(NEOGEO_POCKET_COLOR, "Neo Geo Pocket Color")
	(NINTENDO_3DS, "Nintendo 3DS")
	(NINTENDO_64, "Nintendo 64")
	(NINTENDO_DS, "Nintendo DS")
	(NINTENDO_NES, "Nintendo Entertainment System (NES)")
	(NINTENDO_GAMEBOY, "Nintendo Game Boy")
	(NINTENDO_GAMEBOY_ADVANCE, "Nintendo Game Boy Advance")
	(NINTENDO_GAMEBOY_COLOR, "Nintendo Game Boy Color")
	(NINTENDO_GAMECUBE, "Nintendo GameCube")
	(NINTENDO_WII, "Nintendo Wii")
	(NINTENDO_WII_U, "Nintendo Wii U")
	(IBM_PC, "PC")
	(SEGA_32X, "Sega 32X")
	(SEGA_CD, "Sega CD")
	(SEGA_DREAMCAST, "Sega Dreamcast")
	(SEGA_GAME_GEAR, "Sega Game Gear")
	(SEGA_GENESIS, "Sega Genesis")
	(SEGA_MASTER_SYSTEM, "Sega Master System")
	(SEGA_MEGA_DRIVE, "Sega Mega Drive")
	(SEGA_SATURN, "Sega Saturn")
	(SONY_PLAYSTATION, "Sony Playstation")
	(SONY_PLAYSTATION_2, "Sony Playstation 2")
	(SONY_PLAYSTATION_3, "Sony Playstation 3")
	(SONY_PLAYSTATION_4, "Sony Playstation 4")
	(SONY_PLAYSTATION_VITA, "Sony Playstation Vita")
	(SONY_PLAYSTATION_PORTABLE, "Sony PSP")
	(NINTENDO_SNES, "Super Nintendo (SNES)")
	(NEC_PCENGINE, "TurboGrafx 16")
	(BANDAI_WONDERSWAN, "WonderSwan")
	(BANDAI_WONDERSWAN_COLOR, "WonderSwan Color")
	(SINCLAIR_ZX_SPECTRUM, "Sinclair ZX Spectrum")
	(NINTENDO_VIRTUAL_BOY, "Nintendo Virtual Boy")
	(NINTENDO_GAME_AND_WATCH, "game-and-watch")
	(NEC_PCENGINE_CD, "TurboGrafx CD")
	(NEC_SUPERGRAFX, "TurboGrafx 16")
	(PORT_PRBOOM, "PC")
	(GCE_VECTREX, "Vectrex")
	(GAMEENGINE_LUTRO, "PC")
	(PORT_CAVE_STORY, "PC")
	(MAGNAVOX_ODYSSEY2, "Magnavox Odyssey 2")
	(SINCLAIR_ZX_81, "Sinclair ZX Spectrum")
	(STREAM_MOONLIGHT,"PC");


void thegamesdb_generate_scraper_requests(const ScraperSearchParams& params, std::queue< std::unique_ptr<ScraperRequest> >& requests, 
	std::vector<ScraperSearchResult>& results)
{
	std::string path = "thegamesdb.net/api/GetGame.php?";

	std::string cleanName = params.nameOverride;
	if(cleanName.empty())
		cleanName = params.game->getCleanName();

	path += "name=" + HttpReq::urlEncode(cleanName);

	if(params.system->getPlatformIds().empty())
	{
		// no platform specified, we're done
		requests.push(std::unique_ptr<ScraperRequest>(new TheGamesDBRequest(results, path)));
	}else{
		// go through the list, we need to split this into multiple requests 
		// because TheGamesDB API either sucks or I don't know how to use it properly...
		std::string urlBase = path;
		auto& platforms = params.system->getPlatformIds();
		for (auto& platform : platforms)
		{
			path = urlBase;
			auto mapIt = gamesdb_platformid_map.find(platform);
			if(mapIt != gamesdb_platformid_map.end())
			{
				path += "&platform=";
				path += HttpReq::urlEncode(mapIt->second);
			}else{
				LOG(LogWarning) << "TheGamesDB scraper warning - no support for platform " << getPlatformName(platform);
			}

			requests.push(std::unique_ptr<ScraperRequest>(new TheGamesDBRequest(results, path)));
		}
	}
}

void TheGamesDBRequest::process(const std::unique_ptr<HttpReq>& req, std::vector<ScraperSearchResult>& results)
{
	assert(req->status() == HttpReq::REQ_SUCCESS);

	pugi::xml_document doc;
	pugi::xml_parse_result parseResult = doc.load_string(req->getContent().c_str());
	if(!parseResult)
	{
		std::stringstream ss;
		ss << "GamesDBRequest - Error parsing XML. \n\t" << parseResult.description() << "";
		std::string err = ss.str();
		setError(err);
		LOG(LogError) << err;
		return;
	}

	pugi::xml_node data = doc.child("Data");

	std::string baseImageUrl = data.child("baseImgUrl").text().get();

	pugi::xml_node game = data.child("Game");
	while(game && results.size() < MAX_SCRAPER_RESULTS)
	{
		ScraperSearchResult result;

		result.mdl.SetName(game.child("GameTitle").text().get());
		result.mdl.SetDescription(game.child("Overview").text().get());

		DateTime rd;
		DateTime::ParseFromString("%MM/%DD/%YYYY", game.child("ReleaseDate").text().get(), rd);
		result.mdl.SetReleaseDate(rd);

		result.mdl.SetDeveloper(game.child("Developer").text().get());
		result.mdl.SetPublisher(game.child("Publisher").text().get());
		result.mdl.SetGenre(game.child("Genres").first_child().text().get());
		result.mdl.SetPlayersAsString(game.child("Players").text().get());

		if(Settings::getInstance()->getBool("ScrapeRatings") && game.child("Rating"))
		{
			float ratingVal = (game.child("Rating").text().as_int() / 10.0f);
			result.mdl.SetRating(ratingVal);
		}

		pugi::xml_node images = game.child("Images");

		if(images)
		{
			pugi::xml_node art = images.find_child_by_attribute("boxart", "side", "front");

			if(art)
			{
				result.thumbnailUrl = baseImageUrl + art.attribute("thumb").as_string();
				result.imageUrl = baseImageUrl + art.text().get();
			}
		}

		results.push_back(result);
		game = game.next_sibling("Game");
	}
}
