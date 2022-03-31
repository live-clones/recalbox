//
// Created by bkg2k on 26/03/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include "GuiInfoPopupScraper.h"
#include "scraping/ScrapeResult.h"
#include <themes/MenuThemeData.h>
#include <components/TextComponent.h>
#include <components/ProgressBarComponent.h>
#include <systems/SystemData.h>

std::string GuiInfoPopupScraper::mTextTemplate("Scraping game %VALUE%/%TOTAL%\n%SYSTEM%\n%NAME%");

GuiInfoPopupScraper::GuiInfoPopupScraper(WindowManager& window)
  : GuiInfoPopupBase(window, true, 0x7FFFFFF, PopupType::Scraper, 3, 2, 1.6f)
{
}

float GuiInfoPopupScraper::AddComponents(WindowManager& window, ComponentGrid& grid, float maxWidth, float maxHeight,
                                         int paddingX, int paddingY)
{
  (void)paddingY;
  std::string iconText = "\uF1e4";

  auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
  float hwSize = Math::min(Renderer::Instance().DisplayHeightAsFloat(), Renderer::Instance().DisplayWidthAsFloat());
  unsigned int FONT_SIZE_ICON = (unsigned int)((Renderer::Instance().IsSmallResolution() ? 0.08f : 0.04f) * hwSize);
  unsigned int FONT_SIZE_TEXT = (unsigned int)((Renderer::Instance().IsSmallResolution() ? 0.04f : 0.02f) * hwSize);

  mText        = std::make_shared<TextComponent>(window, mTextTemplate + '\n', Font::get((int)FONT_SIZE_TEXT), menuTheme->menuText.color, TextAlignment::Top);
  mIcon        = std::make_shared<TextComponent>(window, iconText, Font::get((int)FONT_SIZE_ICON), menuTheme->menuText.color, TextAlignment::Left);
  mProgressBar = std::make_shared<ProgressBarComponent>(window, 1);
  mImage       = std::make_shared<ImageComponent>(window);
  mNoImage     = std::make_shared<ImageComponent>(window);

  grid.setEntry(mIcon       , Vector2i(0, 0), false, false);
  grid.setEntry(mText       , Vector2i(1, 0), false, false);
  grid.setEntry(mImage      , Vector2i(2, 0), false, false);
  grid.setEntry(mNoImage    , Vector2i(2, 0), false, false);
  grid.setEntry(mProgressBar, Vector2i(0, 1), false, false, Vector2i(3, 1));

  float msgHeight = 0.0f;
  mText->setSize(maxWidth - mIcon->getSize().y(), 0);
  msgHeight = Math::min(maxHeight, Math::max(mText->getSize().y(), mIcon->getSize().y()));
  grid.setColWidthPerc(0, (float)(mIcon->getFont()->getSize() + paddingX) / maxWidth);
  grid.setColWidthPerc(2, (msgHeight * 1.5f) / maxWidth);
  grid.setRowHeightPerc(1, 0.16f);
  mProgressBar->setSize(maxWidth, 0);
  mImage->setResize(0.f, msgHeight * 0.9f);
  mNoImage->setResize(0.f, msgHeight * 0.9f);
  mNoImage->setImage(Path(":/no_image.png"));

  mText->setText("Scraper starting...");

  return msgHeight * 1.16f;
}

void GuiInfoPopupScraper::SetScrapedGame(const FileData& game, int value, int total)
{
  if (Initialized())
  {
    mProgressBar->setMaxValue(total);
    mProgressBar->setCurrentValue(value);
    std::string msg(mTextTemplate);
    Strings::ReplaceAllIn(msg, "%VALUE%", Strings::ToString(value));
    Strings::ReplaceAllIn(msg, "%TOTAL%", Strings::ToString(total));
    Strings::ReplaceAllIn(msg, "%NAME%", game.Metadata().Name());
    Strings::ReplaceAllIn(msg, "%SYSTEM%", game.System().FullName());
    mText->setVerticalAlignment(TextAlignment::Top);
    mText->setSize(Grid().getColWidth(1), Grid().getRowHeight(0));
    mText->setText(msg);
    mProgressBar->setSize(mProgressBar->getSize().x(), Grid().getSize().y() * 0.08f);
    mImage->setResize(0.f, Grid().getRowHeight(0)* 0.9f);
    mNoImage->setResize(0.f, Grid().getRowHeight(0)* 0.9f);
    const MetadataDescriptor& meta = game.Metadata();
    mImage->SetVisible(true);
    mNoImage->SetVisible(false);
    if      (!meta.Image().IsEmpty())     mImage->setImage(meta.Image());
    else if (!meta.Thumbnail().IsEmpty()) mImage->setImage(meta.Thumbnail());
    else
    {
      mImage->SetVisible(false);
      mNoImage->SetVisible(true);
    }
    Grid().onSizeChanged();
  }
}

void GuiInfoPopupScraper::ScrapingComplete(ScrapeResult result)
{
  if (Initialized())
  {
    std::string msg("Scraping completed!\n\n");
    switch(result)
    {
      case ScrapeResult::Ok: msg.append("All games scraped successfully."); break;
      case ScrapeResult::QuotaReached: msg.append("You reached your daily quota. Retry tomorrow."); break;
      case ScrapeResult::DiskFull: msg.append("Your disk is full Pleas make room before trying again."); break;
      case ScrapeResult::NotScraped:
      case ScrapeResult::NotFound:
      case ScrapeResult::FatalError: msg.append("Errors occurred and prevented some games to get their metadata. Retry later."); break;
      default: break;
    }
    mText->setVerticalAlignment(TextAlignment::Top);
    mText->setSize(Grid().getColWidth(1), Grid().getRowHeight(0));
    mText->setText(msg);
    Grid().onSizeChanged();
  }
}
