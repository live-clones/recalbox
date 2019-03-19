#include "scrapers/ScreenscraperScraper2.h"

#include "datetime/DateTime.h"
#include "utils/StringUtil.h"
#include "FileData.h"
#include "Log.h"
#include "PlatformId.h"
#include "Settings.h"
#include "SystemData.h"
#include "pugixml/pugixml.hpp"
#include <cstring>

using namespace PlatformIds;

/**
	List of systems and their IDs from
	https://www.screenscraper.fr/api/systemesListe.php?devid=xxx&devpassword=yyy&softname=zzz&output=XML
**/
static const std::map<PlatformId, unsigned short>& getPlatformIDs()
{
	static const std::map<PlatformId, unsigned short> screenscraperPlatformidMap
	{
		{AMSTRAD_CPC,               65},
		{APPLE_II,                  86},
		{APPLE_MACOS,               146},
		{ARCADE,                    75},
		{ATARI_8BITS,               43},
		{ATARI_2600,                26},
		{ATARI_5200,                40},
		{ATARI_7800,                41},
		{ATARI_JAGUAR,              27},
		{ATARI_JAGUAR_CD,           171},
		{ATARI_LYNX,                28},
		{ATARI_ST,                  42},
		{BANDAI_WONDERSWAN,         45},
		{BANDAI_WONDERSWAN_COLOR,   46},
		{COLECOVISION,              48},
		{COMMODORE_64,              66},
		{COMMODORE_AMIGA,           64},
		{FAIRCHILD_CHANNELF,        80},
		{GAMEENGINE_DAPHNE,         49},
		{GAMEENGINE_LUTRO,          206},
		{GAMEENGINE_OPENBOR,        214},
		{GAMEENGINE_SCUMMVM,        123},
		{GCE_VECTREX,               102},
		{IBM_PC,                    135},
		{MAGNAVOX_ODYSSEY2,         104},
		{MATTEL_INTELLIVISION,      115},
		{MGT_SAMCOUPE,              213},
		{MICROSOFT_MSX,             113},
		{MICROSOFT_MSX1,            113},
		{MICROSOFT_MSX2,            113},
		{MICROSOFT_XBOX,            32},
		{MICROSOFT_XBOX_360,        33},
		{NEC_PCENGINE,              31},
		{NEC_PCENGINE_CD,           114},
		{NEC_SUPERGRAFX,            31},
		{NEOGEO,                    142},
		{NEOGEO_POCKET,             25},
		{NEOGEO_POCKET_COLOR,       82},
		{NINTENDO_3DS,              17},
		{NINTENDO_64,               14},
		{NINTENDO_DS,               15},
		{NINTENDO_FDS,              106},
		{NINTENDO_GAME_AND_WATCH,   52},
		{NINTENDO_GAMEBOY,          9},
		{NINTENDO_GAMEBOY_ADVANCE,  12},
		{NINTENDO_GAMEBOY_COLOR,    10},
		{NINTENDO_GAMECUBE,         13},
		{NINTENDO_NES,              3},
		{NINTENDO_POKEMINI,         211},
		{NINTENDO_SATELLAVIEW,      107},
		{NINTENDO_SNES,             4},
		{NINTENDO_SUFAMITURBO,      108},
		{NINTENDO_WII,              16},
		{NINTENDO_WII_U,            18},
		{NINTENDO_VIRTUAL_BOY,      11},
		{OSH_UZEBOX,                216},
		{PANASONIC_3DO,             29},
		{SEGA_32X,                  19},
		{SEGA_CD,                   20},
		{SEGA_DREAMCAST,            23},
		{SEGA_GAME_GEAR,            21},
		{SEGA_GENESIS,              1},
		{SEGA_MASTER_SYSTEM,        2},
		{SEGA_MEGA_DRIVE,           1},
		{SEGA_SATURN,               22},
		{SEGA_SG1000,               109},
		{SHARP_X68000,              79},
		{SINCLAIR_ZX_SPECTRUM,      76},
		{SINCLAIR_ZX_81,            77},
		{SONY_PLAYSTATION,          57},
		{SONY_PLAYSTATION_2,        58},
		{SONY_PLAYSTATION_3,        59},
		{SONY_PLAYSTATION_VITA,     62},
		{SONY_PLAYSTATION_PORTABLE, 61},
		{TANDERINE_ORICATMOS,       131},
		{THOMSON_MOTO,              141},
	};

	return screenscraperPlatformidMap;
}

// Helper XML parsing method, finding a node-by-name recursively.
/*pugi::xml_node find_node_by_name_re(const pugi::xml_node& node, const std::vector<std::string> node_names) {

	for (const std::string& _val : node_names)
	{
		pugi::xpath_query query_node_name((static_cast<std::string>("//") + _val).c_str());
		pugi::xpath_node_set results = node.select_nodes(query_node_name);

		if (!results.empty())
			return results.first().node();
	}

	pugi::xml_node emptyNode;
	return emptyNode;
}*/

// Help XML parsing method, finding an direct child XML node starting from the parent and filtering by an attribute value list.
pugi::xml_node find_child_by_attribute_list(const pugi::xml_node& node_parent, const std::string& node_name, const std::string& attribute_name, const std::vector<std::string> attribute_values)
{
	for (const auto& _val : attribute_values)
	{
		for (pugi::xml_node node : node_parent.children(node_name.c_str()))
		{

			if (strcmp(node.attribute(attribute_name.c_str()).value(), _val.c_str()) == 0)
				return node;
		}
	}

	return pugi::xml_node(nullptr);

}

void screenscraper_generate_scraper_requests(const ScraperSearchParams& params,
	std::queue< std::unique_ptr<ScraperRequest> >& requests,
	std::vector<ScraperSearchResult>& results)
{
	std::string path;

	ScreenScraperRequest::ScreenScraperConfig ssConfig;

	path = ssConfig.getGameSearchUrl(params.game->getPath().filename().generic_string());
	auto& platforms = params.system->getPlatformIds();

	for (auto platform : platforms)
	{
		auto mapIt = getPlatformIDs().find(platform);
		if (mapIt != getPlatformIDs().cend())
		{
			path += "&systemeid=";
			path += HttpReq::urlEncode(std::to_string(mapIt->second));
		}else{
			LOG(LogWarning) << "ScreenScraper: no support for platform " << getPlatformName(platform);
		}

		requests.push(std::unique_ptr<ScraperRequest>(new ScreenScraperRequest(requests, results, path)));
	}

}

void ScreenScraperRequest::process(const std::unique_ptr<HttpReq>& req, std::vector<ScraperSearchResult>& results)
{
	assert(req->status() == HttpReq::REQ_SUCCESS);

	pugi::xml_document doc;
	pugi::xml_parse_result parseResult = doc.load_string(req->getContent().c_str());

	if (!parseResult)
	{
		std::stringstream ss;
		ss << "ScreenScraperRequest - Error parsing XML." << std::endl << parseResult.description() << "";

		std::string err = ss.str();
		setError(err);
		LOG(LogError) << err;

		return;
	}

	processGame(doc, results);
}

void ScreenScraperRequest::processGame(const pugi::xml_document& xmldoc, std::vector<ScraperSearchResult>& out_results)
{
	pugi::xml_node data = xmldoc.child("Data");
	pugi::xml_node game = data.child("jeu");

	if (game)
	{
		ScraperSearchResult result;
		ScreenScraperRequest::ScreenScraperConfig ssConfig;

		std::string region = StringUtil::toLower(ssConfig.region);
		std::string language = StringUtil::toLower(ssConfig.language);
    std::vector<std::string> regions = { region, "us", "eu", "jp", "wor", "ss" };

		// Name fallback: US, WOR(LD). ( Xpath: Data/jeu[0]/noms/nom[*] ). 
		result.mdl.SetName(find_child_by_attribute_list(game.child("noms"), "nom", "region", regions).text().get());

		// Description fallback language: EN, WOR(LD)
		std::string description = find_child_by_attribute_list(game.child("synopsis"), "synopsis", "langue", { language, "en", "wor" }).text().get();

		if (!description.empty()) {
			result.mdl.SetDescription(StringUtil::replace(description, "&nbsp;", " "));
		}

		// Genre fallback language: EN. ( Xpath: Data/jeu[0]/genres/genre[*] )
		result.mdl.SetGenre(find_child_by_attribute_list(game.child("genres"), "genre", "langue", { language, "en", "wor" }).text().get());
		LOG(LogDebug) << "Genre: " << result.mdl.Genre();

		// Get the date proper. The API returns multiple 'date' children nodes to the 'dates' main child of 'jeu'.
		// Date fallback: WOR(LD), US, SS, JP, EU
		std::string _date = find_child_by_attribute_list(game.child("dates"), "date", "region", regions).text().get();
		LOG(LogDebug) << "Release Date (unparsed): " << _date;

		// Date can be YYYY-MM-DD or just YYYY.
		DateTime dt(false);
		if (DateTime::ParseFromString("%yyyy-%MM-%dd", _date, dt)) result.mdl.SetReleaseDate(dt);
		else if (DateTime::ParseFromString("%yyyy", _date, dt)) result.mdl.SetReleaseDate(dt);

		LOG(LogDebug) << "Release Date (parsed): " << result.mdl.ReleaseDateAsString();

		/// Developer for the game( Xpath: Data/jeu[0]/developpeur )
		std::string developer = game.child("developpeur").text().get();
		if (!developer.empty())
			result.mdl.SetDeveloper(StringUtil::replace(developer, "&nbsp;", " "));

		// Publisher for the game ( Xpath: Data/jeu[0]/editeur )
		std::string publisher = game.child("editeur").text().get();
		if (!publisher.empty())
			result.mdl.SetPublisher(StringUtil::replace(publisher, "&nbsp;", " "));

		// Players
		result.mdl.SetPlayCountAsString(game.child("joueurs").text().get());

		if (Settings::getInstance()->getBool("ScrapeRatings") && game.child("note"))
		{
			float ratingVal = (game.child("note").text().as_int() / 20.0f);
			result.mdl.SetRating(ratingVal);
		}

		// Media super-node
		pugi::xml_node media_list = game.child("medias");

		if (media_list)
		{
			pugi::xml_node art = pugi::xml_node(nullptr);

			// Do an XPath query for media[type='$media_type'], then filter by region
			// We need to do this because any child of 'medias' has the form
			// <media type="..." region="..." format="..."> 
			// and we need to find the right media for the region.
			pugi::xpath_node_set results = media_list.select_nodes((static_cast<std::string>("media[@type='") + ssConfig.media_name + "']").c_str());
			if (!results.empty())
			{
				// Region fallback: WOR(LD), US, CUS(TOM?), JP, EU
				for (const auto& _region : regions)
				{
					if (art) break;
					for (auto node : results)
					{
						if (node.node().attribute("region").value() == _region)
						{
							art = node.node();
							break;
						}
					}
				}
			} // results

			if (art)
			{
				// Sending a 'softname' containing space will make the image URLs returned by the API also contain the space. 
				//  Escape any spaces in the URL here
				result.imageUrl = StringUtil::replace(art.text().get(), " ", "%20");
        // Ask for the same image, but with a smaller size, for the thumbnail displayed during scraping
        result.thumbnailUrl = result.imageUrl + "&maxheight=250";

				// Get the media type returned by ScreenScraper
				std::string media_type = StringUtil::toLower(art.attribute("format").value());
				ScraperImageType type = ScraperImageType::Jpeg; // default value
				if (media_type == "png")
          type = ScraperImageType::Png; // default value

        result.imageType = result.thumbnailType = type;
			}
			else LOG(LogDebug) << "Failed to find media XML node with name=" << ssConfig.media_name;
		}

		out_results.push_back(result);
	} // game
}

// Currently not used in this module
/*void ScreenScraperRequest::processList(const pugi::xml_document& xmldoc, std::vector<ScraperSearchResult>& results)
{
	assert(mRequestQueue != nullptr);

	LOG(LogDebug) << "Processing a list of results";

	pugi::xml_node data = xmldoc.child("Data");
	pugi::xml_node game = data.child("jeu");

	if (!game)
		LOG(LogDebug) << "Found nothing";

	ScreenScraperRequest::ScreenScraperConfig ssConfig;

	// limit the number of results per platform, not in total.
	// otherwise if the first platform returns >= 7 games
	// but the second platform contains the relevant game,
	// the relevant result would not be shown.
	for (int i = 0; game && i < MAX_SCRAPER_RESULTS; i++)
	{
		std::string id = game.child("id").text().get();
		std::string name = game.child("nom").text().get();
		std::string platformId = game.child("systemeid").text().get();
		std::string path = ssConfig.getGameSearchUrl(name).append("&systemeid=").append(platformId).append("&gameid=").append(id);

		mRequestQueue->push(std::unique_ptr<ScraperRequest>(new ScreenScraperRequest(results, path)));

		game = game.next_sibling("jeu");
	}
}*/

// Scrambling
std::string ScreenScraperRequest::ScreenScraperConfig::scramble(const std::string& _input, const std::string& key) const
{
	std::string buffer = _input;

	for (int i = (int)_input.size(); --i >= 0; )
		buffer[i] = (char)(_input[i] ^ (key[i % key.size()] + (i * 17)));

	return buffer;
}

std::string ScreenScraperRequest::ScreenScraperConfig::getGameSearchUrl(const std::string& gameName) const
{
	return API_URL_BASE
		+ "/jeuInfos.php?devid=" + scramble(API_DEV_U, API_DEV_KEY)
		+ "&devpassword=" + scramble(API_DEV_P, API_DEV_KEY)
		+ "&softname=" + HttpReq::urlEncode(API_SOFT_NAME)
		+ "&output=xml"
		+ "&romnom=" + HttpReq::urlEncode(gameName);

}
