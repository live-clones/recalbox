//
// Created by bkg2k on 26/03/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <components/ComponentGrid.h>
#include <WindowManager.h>
#include <guis/GuiInfoPopupBase.h>
#include <utils/Log.h>
#include <scraping/ScrapeResult.h>

// Forward declaration
class TextComponent;
class ProgressBarComponent;
class ImageComponent;

class GuiInfoPopupScraper : public GuiInfoPopupBase
{
  public:
    /*!
     * @brief Constructor
     * @param window Window manager
     */
    explicit GuiInfoPopupScraper(WindowManager& window);

    /*!
     * @brief Refresh game informations
     * @param game FileData
     * @param value current game
     * @param total total game
     */
    void SetScrapedGame(const FileData& game, int value, int total);

    /*!
     * @brief Scraping completed
     * @param result Scraper Result
     */
    void ScrapingComplete(ScrapeResult result);

  protected:
    float AddComponents(WindowManager& window, ComponentGrid& grid, float maxWidth, float maxHeight, int paddingX, int paddingY) override;

  private:
    //! Text template
    static std::string mTextTemplate;

    //! Message component
    std::shared_ptr<TextComponent> mText;
    //! Icon component
    std::shared_ptr<TextComponent> mIcon;
    //! Image component
    std::shared_ptr<ImageComponent> mImage;
    //! No Image
    std::shared_ptr<ImageComponent> mNoImage;
    //! Progress bar
    std::shared_ptr<ProgressBarComponent> mProgressBar;
};



