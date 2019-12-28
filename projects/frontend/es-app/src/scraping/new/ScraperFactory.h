//
// Created by bkg2k on 04/12/2019.
//
#pragma once

#include <string>
#include <scraping/new/scrapers/IScraperEngine.h>

class ScraperFactory
{
  private:
    //! Available scraper type
    enum class ScraperType
    {
        ScreenScraper, //!< ScreenScraper database (default)
        TheGameDB,     //!< The GameDB database
    };

    //! Scraper instance holder, for automatic destruction
    class ScraperHolder
    {
      public:
        //! Hold scraper instances
        std::map<ScraperType, IScraperEngine*> mScrapers;

        /*!
         * @brief Get or create new scraper instance and store it in the map
         * @param type Scraper type to get/create
         * @return IScraperEngine instance
         */
        IScraperEngine* Get(ScraperType type);

        /*!
         * @brief Destructor. Cleanup instantiated scrapers
         */
        ~ScraperHolder();
    };

    static ScraperHolder mScrapers;

  public:
    static IScraperEngine* GetScraper(const std::string& scraperidentifier);

    static const std::vector<std::string>& GetScraperList();
};

