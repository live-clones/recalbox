//
// Created by bkg2k on 04/12/2019.
//
#pragma once

#include <scraping/new/scrapers/IScraperEngine.h>

class TheGameDBEngine : public IScraperEngine
{
  private:
    /*
     * IScraperEngine implemntation
     */

    /*!
     * @brief Called from the factory to reset the current instance to its initial state
     * Implementation is responsible for reseting all its internal structures as if it were just created.
     */
    void Initialize() override;

    /*!
     * @brief Run the scraper using the given methods, on the given system list and report progress using notifyTarget
     * @param method Scrapping method
     * @param systemList System list
     * @param notifyTarget Interface for reporting scrapping progression
     * @return True if everything has been successful. False if cancelled, quota reached or fatal error occurred
     */
    bool RunOn(ScrappingMethod method, const SystemManager::SystemList& systemList, INotifyScrapeResult* notifyTarget) override;

    /*!
     * @brief Abort the current engine
     * @return True
     */
    bool Abort() override;
};
