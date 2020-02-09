#include "guis/GuiScraperSingleGameRun.h"
#include "guis/GuiTextEditPopup.h"
#include "guis/GuiTextEditPopupKeyboard.h"
#include "components/TextComponent.h"
#include "components/ButtonComponent.h"
#include "components/MenuComponent.h"
#include "Settings.h"
#include "utils/locale/LocaleHelper.h"
#include "MenuThemeData.h"
#include <scraping/new/ScraperFactory.h>
#include <recalbox/RecalboxSystem.h>
#include <scraping/new/ScraperTools.h>

GuiScraperSingleGameRun::GuiScraperSingleGameRun(Window&window, FileData& game, IScrappingComplete* notifier)
  : Gui(window),
    mGame(game),
    mNotifier(notifier),
  	mGrid(window, Vector2i(1, 7)),
	  mBox(window, Path(":/frame.png"))
{
	auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
	
	mBox.setImagePath(menuTheme->menuBackground.path);
	mBox.setCenterColor(menuTheme->menuBackground.color);
	mBox.setEdgeColor(menuTheme->menuBackground.color);
	addChild(&mBox);
	addChild(&mGrid);

	// row 0 is a spacer

	mGameName = std::make_shared<TextComponent>(mWindow, Strings::ToUpperASCII(game.getPath().Filename()), menuTheme->menuText.font, menuTheme->menuText.color, TextAlignment::Center);
	mGrid.setEntry(mGameName, Vector2i(0, 1), false, true);

	// row 2 is a spacer

	mSystemName = std::make_shared<TextComponent>(mWindow, Strings::ToUpperASCII(game.getSystem()->getFullName()), menuTheme->menuTextSmall.font, menuTheme->menuTextSmall.color, TextAlignment::Center);
	mGrid.setEntry(mSystemName, Vector2i(0, 3), false, true);

	// row 4 is a spacer

	// ScraperSearchComponent
	mSearch = std::make_shared<ScraperSearchComponent>(window);
	mGrid.setEntry(mSearch, Vector2i(0, 5), false);

	// buttons
	mButton = std::make_shared<ButtonComponent>(mWindow, _("CANCEL"), _("CANCEL"), [&] { Close(); });
	std::vector< std::shared_ptr<ButtonComponent> > buttons;
	buttons.push_back(mButton);
	mButtonGrid = makeButtonGrid(mWindow, buttons);

	mGrid.setEntry(mButtonGrid, Vector2i(0, 6), true, false);

	setSize(Renderer::getDisplayWidthAsFloat() * 0.95f, Renderer::getDisplayHeightAsFloat() * 0.747f);
	setPosition((Renderer::getDisplayWidthAsFloat() - mSize.x()) / 2, (Renderer::getDisplayHeightAsFloat() - mSize.y()) / 2);

  // Create scraper and run!
  // Don't use the notification interface since the use can close this gui at any time.
  mScraper = ScraperFactory::GetScraper(Settings::Instance().Scraper());
  mScraper->RunOn(ScrappingMethod::All, game, nullptr, RecalboxSystem::GetMinimumFreeSpaceOnSharePartition());
}

void GuiScraperSingleGameRun::onSizeChanged()
{
	mBox.fitTo(mSize, Vector3f::Zero(), Vector2f(-32, -32));

	mGrid.setRowHeightPerc(0, 0.04f, false);
	mGrid.setRowHeightPerc(1, mGameName->getFont()->getLetterHeight() / mSize.y(), false); // game name
	mGrid.setRowHeightPerc(2, 0.04f, false);
	mGrid.setRowHeightPerc(3, mSystemName->getFont()->getLetterHeight() / mSize.y(), false); // system name
	mGrid.setRowHeightPerc(4, 0.04f, false);
	mGrid.setRowHeightPerc(6, mButtonGrid->getSize().y() / mSize.y(), false); // buttons
	mGrid.setSize(mSize);
}

bool GuiScraperSingleGameRun::ProcessInput(const InputCompactEvent& event)
{
	if (event.APressed())
	{
		Close();
		return true;
	}

	return Component::ProcessInput(event);
}

void GuiScraperSingleGameRun::Update(int deltaTime)
{
  if (mScraper != nullptr)
    if (!mScraper->IsRunning())
    {
      mSearch->SetRunning(false);
      mSearch->UpdateInfoPane(&mGame);
      mButton->setText(_("CLOSE"), _("CLOSE"));
      mScraper = nullptr;
      if (mNotifier != nullptr)
        mNotifier->ScrappingComplete(mGame);
    }

  Component::Update(deltaTime);
}

bool GuiScraperSingleGameRun::getHelpPrompts(Help& help)
{
	return mGrid.getHelpPrompts(help);
}


void GuiScraperSingleGameRun::GameResult(int index, int total, FileData* result)
{
  (void)index;
  (void)total;

  // Extract region?
  if (RecalboxConf::Instance().AsBool("scraper.extractregionfromfilename"))
    ScraperFactory::ExtractRegionFromFilename(*result);
  // Overwrite name?
  switch(ScraperTools::Clamp(RecalboxConf::Instance().AsInt("scraper.getnamefrom")))
  {
    case ScraperNameOptions::GetFromScraper: break;
    case ScraperNameOptions::GetFromFilename:
    {
      result->Metadata().SetName(result->getPath().FilenameWithoutExtension());
      break;
    }
    case ScraperNameOptions::GetFromFilenameUndecorated:
    {
      ScraperFactory::ExtractFileNameUndecorated(*result);
      break;
    }
  }

  // Update game data
  mSearch->UpdateInfoPane(result);
}

void GuiScraperSingleGameRun::ScrapingComplete(ScrapeResult reason)
{
  (void)reason;
}