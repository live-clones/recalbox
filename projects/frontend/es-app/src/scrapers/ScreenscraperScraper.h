#pragma once

#include "scrapers/Scraper.h"
#include "EmulationStation.h"

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
    ScreenScraperRequest(std::vector<ScraperSearchResult>& resultsWrite, const std::string& url) : ScraperHttpRequest(
      resultsWrite, url), mRequestQueue(nullptr)
    {}

    // Settings for the scraper
    static const struct ScreenScraperConfig
    {
      std::string getGameSearchUrl(const std::string& gameName) const;

      // Access to the API
      const std::string API_DEV_U = "\xF1\x5A\xA8\x46\x25\xDE\x48\x2A";
      const std::string API_DEV_P = "\xC0\x0C\x80\x45\x30\xD6\x7F\x6A\x69\xB5\x02\x9D\xAD\x6B\xA3\x33\xE6\x7A\xE8\x4E";
      const std::string API_DEV_KEY = "\x83\x2E\xA9\xF4\x05\x67\xC1\xDB\xB1\x65\xC7\x0D\xFE\x29\xA3\x48";
      const std::string API_URL_BASE = "https://screenscraper.fr/api2";
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
      std::string media_name = "box-2D";

      // Which Region to use when selecting the artwork
      // Applies to: artwork, name of the game, date of release
      std::string region = "US";

      // Which Language to use when selecting the textual information
      // Applies to: description, genre
      std::string language = "EN";

      std::string scramble(const std::string& _input, const std::string& key) const;

      ScreenScraperConfig()
      {};
    } configuration;

  protected:
    void process(const std::unique_ptr<HttpReq>& req, std::vector<ScraperSearchResult>& results) override;

    void processList(const pugi::xml_document& xmldoc, std::vector<ScraperSearchResult>& results);

    void processGame(const pugi::xml_document& xmldoc, std::vector<ScraperSearchResult>& results);

    bool isGameRequest() { return !mRequestQueue; }

    std::queue<std::unique_ptr<ScraperRequest> >* mRequestQueue;
};
