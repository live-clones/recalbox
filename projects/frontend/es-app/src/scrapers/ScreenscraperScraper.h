#pragma once

#include <utils/StringUtil.h>
#include "scrapers/Scraper.h"
#include "EmulationStation.h"
#include "RecalboxConf.h"

namespace pugi
{
  class xml_document;
}

void screenscraper_generate_scraper_requests(const ScraperSearchParams& params,
                                             std::queue<std::unique_ptr<ScraperRequest> >& requests,
                                             std::vector<ScraperSearchResult>& results);

class ScreenScraperRequest : public ScraperHttpRequest
{
  public:
    // ctor for a GetGameList request
    ScreenScraperRequest(std::queue<std::unique_ptr<ScraperRequest> >& requestsWrite,
                         std::vector<ScraperSearchResult>& resultsWrite, const std::string& url) : ScraperHttpRequest(
      resultsWrite, url), mRequestQueue(&requestsWrite)
    {}

    // ctor for a GetGame request
    //ScreenScraperRequest(std::vector<ScraperSearchResult>& resultsWrite, const std::string& url) : ScraperHttpRequest(
    //  resultsWrite, url), mRequestQueue(nullptr)
    //{}

    // Settings for the scraper
    struct ScreenScraperConfiguration
    {
      // Access to the API
      const std::string API_DEV_U = "\xF1\x5A\xA8\x46\x25\xDE\x48\x2A";
      const std::string API_DEV_P = "\xC0\x0C\x80\x45\x30\xD6\x7F\x6A\x69\xB5\x02\x9D\xAD\x6B\xA3\x33\xE6\x7A\xE8\x4E";
      const std::string API_DEV_KEY = "\x83\x2E\xA9\xF4\x05\x67\xC1\xDB\xB1\x65\xC7\x0D\xFE\x29\xA3\x48";
      const std::string API_URL_BASE = "https://www.screenscraper.fr/api2";
      const std::string API_SOFT_NAME = "Emulationstation " + static_cast<std::string>(PROGRAM_VERSION_STRING);

      /** Which type of image artwork we need. Possible values (not a comprehensive list):
        - ss: in-game screenshot
        - box-3D: 3D boxart
        - box-2D: 2D boxart (default)
        - screenmarque : marquee
        - sstitle: in-game start screenshot
        - steamgrid: Steam artwork
        - wheel: spine
        - support-2D: media showing the 2d boxart on the cart
        - support-3D: media showing the 3d boxart on the cart

        Note that no all games contain values for these, so we default to "box-2D" since it's the most common.
      **/
      std::string media_name = "screenshot";

      // Which Region to use when selecting the artwork
      // Applies to: artwork, name of the game, date of release
      std::string region = "us";

      // Which Language to use when selecting the textual information
      // Applies to: description, genre
      std::string language = "en";

      // Screenscraper login
      std::string user = "";

      // Screenscraper password
      std::string password = "";

      // Scrambling
      std::string scramble(const std::string& _input, const std::string& key) const
      {
        std::string buffer = _input;

        for (int i = (int) _input.size(); --i >= 0;)
          buffer[i] = (char) ((unsigned char)_input[i] ^ (unsigned char)(key[i % key.size()] + (i * 17)));

        return buffer;
      }

      // Build request URL
      std::string getGameSearchUrl(const std::string& gameName) const
      {
        return API_URL_BASE +
               "/jeuInfos.php?devid=" + scramble(API_DEV_U, API_DEV_KEY) +
               "&devpassword=" + scramble(API_DEV_P, API_DEV_KEY) +
               "&softname=" + HttpReq::urlEncode(API_SOFT_NAME) +
               "&ssid=" + user +
               "&sspassword=" + password +
               "&output=xml" +
               "&romnom=" + HttpReq::urlEncode(gameName);
      }

      ScreenScraperConfiguration()
      {
        // Build all the stuff requited by the scraper, regarding use preferences
        RecalboxConf& conf = RecalboxConf::Instance();

        // Extract prefered language/region
        std::string locale = StringUtil::toLower(conf.get("system.language", "en_US"));
        region = (locale.length() == 5) ? locale.substr(3,2) : "us";
        language = (locale.length() == 5) ? locale.substr(0,2) : "en";
        // Override with conf values
        region = conf.get("scraper.screenscraper.region", region);
        language = conf.get("scraper.screenscraper.language", region);

        // Choose media
        media_name = StringUtil::toLower(conf.get("scraper.screenscraper.media", "mixv1"));
        if (media_name == "screenshot") media_name = "ss";
        else if (media_name == "title") media_name = "sstitle";
        else if (media_name == "box2d") media_name = "box-2D";
        else if (media_name == "box3d") media_name = "box-3D";
        else if (media_name == "mixv1") media_name = "mixrbv1";
        else if (media_name == "mixv2") media_name = "mixrbv2";
        else media_name = "mixrbv1";

        user = conf.get("scraper.screenscraper.user", "");
        password = conf.get("scraper.screenscraper.password", "");
      };
    };

    static const ScreenScraperConfiguration& Configuration()
    {
      static ScreenScraperConfiguration _Configuration;
      return _Configuration;
    }

  protected:
    void process(const std::unique_ptr<HttpReq>& req, std::vector<ScraperSearchResult>& results) override;

    //void processList(const pugi::xml_document& xmldoc, std::vector<ScraperSearchResult>& results);

    static void processGame(const pugi::xml_document& xmldoc, std::vector<ScraperSearchResult>& results);

    //bool isGameRequest() { return !mRequestQueue; }

    std::queue<std::unique_ptr<ScraperRequest> >* mRequestQueue;
};
