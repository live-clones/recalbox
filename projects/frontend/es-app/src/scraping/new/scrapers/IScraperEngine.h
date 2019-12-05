//
// Created by bkg2k on 04/12/2019.
//
#pragma once

#include <utils/cplusplus/INoCopy.h>
#include <systems/SystemManager.h>
#include <scraping/new/INotifyScrapeResult.h>

enum class ScrappingMethod
{
  All,                 //<! Scrape all
  IncompleteKeep,      //<! Scrape only missing information, keeping information already here
  IncompleteOverwrite, //<! Scrape only missing information, and update information already here
};

class IScraperEngine : private INoCopy
{
  public:
    /*!
     * @brief Called from the factory to reset the current instance to its initial state
     * Implementation is responsible for reseting all its internal structures as if it were just created.
     */
    virtual void Initialize() = 0;

    /*!
     * @brief Run the scraper using the given methods, on the given system list and report progress using notifyTarget
     * @param method Scrapping method
     * @param systemList System list
     * @param notifyTarget Interface for reporting scrapping progression
     * @return True if everything has been successful. False if cancelled, quota reached or fatal error occurred
     */
    virtual bool RunOn(ScrappingMethod method, const SystemManager::SystemList& systemList, INotifyScrapeResult* notifyTarget) = 0;

    /*!
     * @brief Abort the current engine
     * @return True
     */
    virtual bool Abort() = 0;

    /*!
     * @brief Virtual destructor
     */
    virtual ~IScraperEngine() = default;
};
