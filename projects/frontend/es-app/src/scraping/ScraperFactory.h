//
// Created by bkg2k on 04/12/2019.
//
#pragma once

#include <string>
#include <scraping/scrapers/IScraperEngine.h>
#include <utils/cplusplus/StaticLifeCycleControler.h>

class ScraperFactory : public StaticLifeCycleControler<ScraperFactory>
{
  public:
    //! Available scraper type
    enum class ScraperType
    {
        ScreenScraper, //!< ScreenScraper database (default)
        TheGameDB,     //!< The GameDB database
    };

  private:
    //! Hold scraper instances
    std::map<ScraperType, IScraperEngine*> mScrapers;

    /*!
     * @brief Get or create new scraper instance and store it in the map
     * @param type Scraper type to get/create
     * @return IScraperEngine instance
     */
    IScraperEngine* Get(ScraperType type);

  public:
    /*!
     * @brief Default constructor
     */
    ScraperFactory()
      : StaticLifeCycleControler("ScraperFactory")
    {
    }

    /*!
     * @brief Default destructor
     */
    ~ScraperFactory();

    IScraperEngine* GetScraper(const std::string& scraperidentifier);

    static ScraperType GetScraperType(const std::string& scraperidentifier);

    static const std::vector<std::string>& GetScraperList();

    static void ExtractFileNameUndecorated(FileData& game);

    static void ExtractRegionFromFilename(FileData& game);
};

