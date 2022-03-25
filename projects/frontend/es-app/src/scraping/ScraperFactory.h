//
// Created by bkg2k on 04/12/2019.
//
#pragma once

#include <string>
#include <scraping/scrapers/IScraperEngine.h>
#include <utils/cplusplus/StaticLifeCycleControler.h>
#include <scraping/ScraperType.h>

class ScraperFactory : public StaticLifeCycleControler<ScraperFactory>
{
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

    /*!
     * @brief Get scraper instance for the given type
     * @param scraper Scraper type
     * @return Scraper instance
     */
    IScraperEngine* GetScraper(ScraperType scraper);

    /*!
     * @brief Get scraper list type/name
     * @return HashMap type/name
     */
    static const HashMap<ScraperType, std::string>& GetScraperList();

    static void ExtractFileNameUndecorated(FileData& game);

    static void ExtractRegionFromFilename(FileData& game);
};

