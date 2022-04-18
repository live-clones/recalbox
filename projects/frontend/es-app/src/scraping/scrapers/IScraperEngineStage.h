//
// Created by bkg2k on 19/03/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

// Forward declaration
class FileData;

class IScraperEngineStage
{
  public:
    //! Stages
    enum class Stage
    {
      Text,      //!< All text info have been downloaded
      Images,    //!< All images have been downloaded
      Video,     //!< All video have been downloaded
      Completed, //!< Everything downloaded
    };

    /*!
     * @brief Rezport scraping stage completion
     * @param game Target game
     * @param stage Last stage completed
     */
    virtual void StageCompleted(FileData* game, Stage stage) = 0;
};