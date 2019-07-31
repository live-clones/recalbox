#include "scrapers/ScreenscraperScraper.h"

#include "datetime/DateTime.h"
#include "utils/StringUtil.h"
#include "FileData.h"
#include "Log.h"
#include "PlatformId.h"
#include "Settings.h"
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
    { PlatformId::AMSTRAD_CPC,               65},
    { PlatformId::AMSTRAD_GX4000,            87},
    { PlatformId::APPLE_II,                  86},
    { PlatformId::APPLE_IIGS,                217},
    { PlatformId::APPLE_MACOS,               146},
    { PlatformId::ARCADE,                    75},
    { PlatformId::ATARI_8BITS,               43},
    { PlatformId::ATARI_2600,                26},
    { PlatformId::ATARI_5200,                40},
    { PlatformId::ATARI_7800,                41},
    { PlatformId::ATARI_JAGUAR,              27},
    { PlatformId::ATARI_JAGUAR_CD,           171},
    { PlatformId::ATARI_LYNX,                28},
    { PlatformId::ATARI_ST,                  42},
    { PlatformId::BANDAI_WONDERSWAN,         45},
    { PlatformId::BANDAI_WONDERSWAN_COLOR,   46},
    { PlatformId::COLECOVISION,              48},
    { PlatformId::COMMODORE_64,              66},
    { PlatformId::COMMODORE_AMIGA,           64},
    { PlatformId::COMMODORE_CD32,            130},
    { PlatformId::COMMODORE_CDTV,            129},
    { PlatformId::FAIRCHILD_CHANNELF,        80},
    { PlatformId::GAMEENGINE_DAPHNE,         49},
    { PlatformId::GAMEENGINE_LUTRO,          206},
    { PlatformId::GAMEENGINE_OPENBOR,        214},
    { PlatformId::GAMEENGINE_SCUMMVM,        123},
    { PlatformId::GAMEENGINE_SOLARUS,        223},
    { PlatformId::GAMEENGINE_TIC80,          222},
    { PlatformId::GCE_VECTREX,               102},
    { PlatformId::IBM_PC,                    135},
    { PlatformId::MAGNAVOX_ODYSSEY2,         104},
    { PlatformId::MATTEL_INTELLIVISION,      115},
    { PlatformId::MGT_SAMCOUPE,              213},
    { PlatformId::MICROSOFT_MSX,             113},
    { PlatformId::MICROSOFT_MSX1,            113},
    { PlatformId::MICROSOFT_MSX2,            116},
    { PlatformId::MICROSOFT_MSXTURBOR,       118},
    { PlatformId::MICROSOFT_XBOX,            32},
    { PlatformId::MICROSOFT_XBOX_360,        33},
    { PlatformId::NEC_PC88,                  221},
    { PlatformId::NEC_PC98,                  208},
    { PlatformId::NEC_PCENGINE,              31},
    { PlatformId::NEC_PCENGINE_CD,           114},
    { PlatformId::NEC_PCFX,                  72},
    { PlatformId::NEC_SUPERGRAFX,            31},
    { PlatformId::NEOGEO,                    142},
    { PlatformId::NEOGEO_CD,                 142},
    { PlatformId::NEOGEO_POCKET,             25},
    { PlatformId::NEOGEO_POCKET_COLOR,       82},
    { PlatformId::NINTENDO_3DS,              17},
    { PlatformId::NINTENDO_64,               14},
    { PlatformId::NINTENDO_DS,               15},
    { PlatformId::NINTENDO_FDS,              106},
    { PlatformId::NINTENDO_GAME_AND_WATCH,   52},
    { PlatformId::NINTENDO_GAMEBOY,          9},
    { PlatformId::NINTENDO_GAMEBOY_ADVANCE,  12},
    { PlatformId::NINTENDO_GAMEBOY_COLOR,    10},
    { PlatformId::NINTENDO_GAMECUBE,         13},
    { PlatformId::NINTENDO_NES,              3},
    { PlatformId::NINTENDO_POKEMINI,         211},
    { PlatformId::NINTENDO_SATELLAVIEW,      107},
    { PlatformId::NINTENDO_SNES,             4},
    { PlatformId::NINTENDO_SUFAMITURBO,      108},
    { PlatformId::NINTENDO_VIRTUAL_BOY,      11},
    { PlatformId::NINTENDO_WII,              16},
    { PlatformId::NINTENDO_WII_U,            18},
    { PlatformId::OSH_UZEBOX,                216},
    { PlatformId::PALM_PDA,                  219},
    { PlatformId::PANASONIC_3DO,             29},
    { PlatformId::PORT_CAVE_STORY,           135},
    { PlatformId::PORT_PRBOOM,               135},
    { PlatformId::SAMMY_ATOMISWAVE,          53},
    { PlatformId::SEGA_32X,                  19},
    { PlatformId::SEGA_CD,                   20},
    { PlatformId::SEGA_DREAMCAST,            23},
    { PlatformId::SEGA_GAME_GEAR,            21},
    { PlatformId::SEGA_GENESIS,              1},
    { PlatformId::SEGA_MASTER_SYSTEM,        2},
    { PlatformId::SEGA_MEGA_DRIVE,           1},
    { PlatformId::SEGA_NAOMI,                56},
    { PlatformId::SEGA_SATURN,               22},
    { PlatformId::SEGA_SG1000,               109},
    { PlatformId::SHARP_X1,                  220},
    { PlatformId::SHARP_X68000,              79},
    { PlatformId::SINCLAIR_ZX_SPECTRUM,      76},
    { PlatformId::SINCLAIR_ZX_81,            77},
    { PlatformId::SONY_PLAYSTATION,          57},
    { PlatformId::SONY_PLAYSTATION_2,        58},
    { PlatformId::SONY_PLAYSTATION_3,        59},
    { PlatformId::SONY_PLAYSTATION_VITA,     62},
    { PlatformId::SONY_PLAYSTATION_PORTABLE, 61},
    { PlatformId::SPECTRAVISION_SPECTRAVIDEO,218},
    { PlatformId::TANDERINE_ORICATMOS,       131},
    { PlatformId::THOMSON_MOTO,              141},
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
pugi::xml_node find_child_by_attribute_list(const pugi::xml_node& node_parent, const std::string& node_name,
                                            const std::string& attribute_name,
                                            const std::vector<std::string>& attribute_values)
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
                                             std::queue<std::unique_ptr<ScraperRequest> >& requests,
                                             std::vector<ScraperSearchResult>& results)
{
  std::string path;

  const ScreenScraperRequest::ScreenScraperConfiguration& ssConfig = ScreenScraperRequest::Configuration();

  path = ssConfig.getGameSearchUrl(params.game->getPath().filename().generic_string());
  auto& platforms = params.system->getPlatformIds();

  for (auto platform : platforms)
  {
    auto mapIt = getPlatformIDs().find(platform);
    if (mapIt != getPlatformIDs().cend())
    {
      path += "&systemeid=";
      path += HttpReq::urlEncode(std::to_string(mapIt->second));
    }
    else
    {
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

    std::string region = StringUtil::toLower(Configuration().region);
    std::string language = StringUtil::toLower(Configuration().language);
    std::vector<std::string> regions = { region, "us", "eu", "jp", "wor", "ss" };
    std::vector<std::string> languages = { language, "en", "jp", "wor", "ss" };

    // Name fallback: US, WOR(LD). ( Xpath: Data/jeu[0]/noms/nom[*] ).
    result.mdl.SetName(find_child_by_attribute_list(game.child("noms"), "nom", "region", regions).text().get());

    // Description fallback language: EN, WOR(LD)
    std::string description = find_child_by_attribute_list(game.child("synopsis"), "synopsis", "langue", languages).text().get();

    if (!description.empty())
    {
      result.mdl.SetDescription(StringUtil::replace(description, "&nbsp;", " "));
    }

    // Genre fallback language: EN. ( Xpath: Data/jeu[0]/genres/genre[*] )
    result.mdl.SetGenre(find_child_by_attribute_list(game.child("genres"), "genre", "langue", languages).text().get());
    LOG(LogDebug) << "Genre: " << result.mdl.Genre();

    // Get the date proper. The API returns multiple 'date' children nodes to the 'dates' main child of 'jeu'.
    // Date fallback: WOR(LD), US, SS, JP, EU
    std::string _date = find_child_by_attribute_list(game.child("dates"), "date", "region", regions).text().get();
    LOG(LogDebug) << "Release Date (unparsed): " << _date;

    // Date can be YYYY-MM-DD or just YYYY.
    DateTime dt(false);
    if (DateTime::ParseFromString("%yyyy-%MM-%dd", _date, dt))
      result.mdl.SetReleaseDate(dt);
    else if (DateTime::ParseFromString("%yyyy", _date, dt))
      result.mdl.SetReleaseDate(dt);

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
      float ratingVal = (game.child("note").text().as_float() / 20.0f);
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
      pugi::xpath_node_set results = media_list.select_nodes(
        (static_cast<std::string>("media[@type='") + Configuration().media_name + "']").c_str());
      if (!results.empty())
      {
        // Region fallback: WOR(LD), US, CUS(TOM?), JP, EU
        for (const auto& _region : regions)
        {
          if (art) break;
          for (auto node : results)
            if (node.node().attribute("region").value() == _region)
            {
              art = node.node();
              break;
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
      else LOG(LogDebug) << "Failed to find media XML node with name=" << Configuration().media_name;
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

