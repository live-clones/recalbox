#include <systems/SystemManager.h>
#include <scraping/new/ScraperFactory.h>
#include <RecalboxConf.h>
#include "guis/GuiScraperOptions.h"
#include "guis/GuiScraperRun.h"
#include "guis/GuiMsgBox.h"
#include "MenuMessages.h"

#include "components/OptionListComponent.h"
#include "components/SwitchComponent.h"
#include "utils/locale/LocaleHelper.h"
#include "Settings.h"

GuiScraperOptions::GuiScraperOptions(Window& window, SystemManager& systemManager)
  : Gui(window),
    mSystemManager(systemManager),
    mMenu(window, _("SCRAPE NOW"))
{
	addChild(&mMenu);

	// add filters (with first one selected)
  mScrapingMethod = std::make_shared< OptionListComponent<ScrappingMethod> >(mWindow, _("SCRAPE THESE GAMES"), false);
  mScrapingMethod->add(_("All Games"), ScrappingMethod::All, false);
  mScrapingMethod->add(_("Only missing image"), ScrappingMethod::IncompleteKeep, true);
	mMenu.addWithLabel(mScrapingMethod, _("FILTER"));

	// add systems (all with a platformid specified selected)
	mSystems = std::make_shared< OptionListComponent<SystemData*> >(mWindow, _("SCRAPE THESE SYSTEMS"), true);
	for (auto it : mSystemManager.GetVisibleSystemList())
		if(!it->hasPlatformId(PlatformIds::PlatformId::PLATFORM_IGNORE))
		  if (!it->IsVirtual() || it->IsFavorite()) // Allow scrapping favorites, but not virtual systems
			  mSystems->add(it->getFullName(), it, it->PlatformCount() != 0);
	mMenu.addWithLabel(mSystems, _("SYSTEMS"));

	// Select option regarding the selected scraper
	ScraperFactory::ScraperType type = ScraperFactory::Instance().GetScraperType(Settings::Instance().Scraper());

	switch(type)
  {
    case ScraperFactory::ScraperType::ScreenScraper:
    {
      std::string imageCode = Strings::ToLowerASCII(RecalboxConf::Instance().AsString("scraper.screenscraper.media", "mixv1"));
      if (std::string("screenshot|title|logo|marquee|box2d|box3d|mixv1|mixv2").find(imageCode) == std::string::npos)
        imageCode = "mixv1";

      mImage = std::make_shared< OptionListComponent<std::string> >(mWindow, _("SELECT IMAGE TYPE"), false);
      mImage->add(_("In-game screenshot")  , "screenshot", imageCode == "screenshot");
      mImage->add(_("Title screenshot")    , "title"     , imageCode == "title"     );
      mImage->add(_("Clear logo")          , "logo"      , imageCode == "logo"      );
      mImage->add(_("Marquee")             , "marquee"   , imageCode == "marquee"   );
      mImage->add(_("2D Case")             , "box2d"     , imageCode == "box2d"     );
      mImage->add(_("3D Case")             , "box3d"     , imageCode == "box3d"     );
      mImage->add(_("ScreenScraper Mix V1"), "mixv1"     , imageCode == "mixv1"     );
      mImage->add(_("ScreenScraper Mix V2"), "mixv2"     , imageCode == "mixv2"     );
      mMenu.addWithLabel(mImage, _("SCRAPE IMAGE"));

      std::string thumbnailCode = Strings::ToLowerASCII(RecalboxConf::Instance().AsString("scraper.screenscraper.thumbnail", "box3d"));
      if (std::string("screenshot|title|logo|marquee|box2d|box3d|mixv1|mixv2").find(thumbnailCode) == std::string::npos)
        thumbnailCode = "box3d";

      mThumbnail = std::make_shared< OptionListComponent<std::string> >(mWindow, _("SELECT THUMBNAIL TYPE"), false);
      mThumbnail->add(_("No thumbnail")        , "none"      , thumbnailCode == "none");
      mThumbnail->add(_("In-game screenshot")  , "screenshot", thumbnailCode == "screenshot");
      mThumbnail->add(_("Title screenshot")    , "title"     , thumbnailCode == "title"     );
      mThumbnail->add(_("Clear logo")          , "logo"      , thumbnailCode == "logo"      );
      mThumbnail->add(_("Marquee")             , "marquee"   , thumbnailCode == "marquee"   );
      mThumbnail->add(_("2D Case")             , "box2d"     , thumbnailCode == "box2d"     );
      mThumbnail->add(_("3D Case")             , "box3d"     , thumbnailCode == "box3d"     );
      mThumbnail->add(_("ScreenScraper Mix V1"), "mixv1"     , thumbnailCode == "mixv1"     );
      mThumbnail->add(_("ScreenScraper Mix V2"), "mixv2"     , thumbnailCode == "mixv2"     );
      mMenu.addWithLabel(mThumbnail, _("SCRAPE THUMBNAIL"));

      std::string videoCode = Strings::ToLowerASCII(RecalboxConf::Instance().AsString("scraper.screenscraper.video", "none"));
      if (std::string("none|normal|optimized").find(videoCode) == std::string::npos)
        videoCode = "none";

      mVideo = std::make_shared< OptionListComponent<std::string> >(mWindow, _("SELECT VIDEO TYPE"), false);
      mVideo->add(_("No video")                  , "none"     , videoCode == "none"     );
      mVideo->add(_("Original video")            , "normal"   , videoCode == "normal"   );
      mVideo->add(_("Optimized/Normalized video"), "optimized", videoCode == "optimized");
      mMenu.addWithLabel(mVideo, _("SCRAPE VIDEO"));

      std::string regionCode = Strings::ToLowerASCII(RecalboxConf::Instance().AsString("scraper.screenscraper.region", "wor"));
      if (std::string("eu|us|jp|wor").find(regionCode) == std::string::npos)
        regionCode = "wor";

      mRegion = std::make_shared< OptionListComponent<std::string> >(mWindow, _("SELECT FAVORITE REGION"), false);
      mRegion->add(_("Europe"), "eu" , regionCode == "eu" );
      mRegion->add(_("USA")   , "us" , regionCode == "us" );
      mRegion->add(_("Japan") , "jp" , regionCode == "jp" );
      mRegion->add(_("World") , "wor", regionCode == "wor");
      mMenu.addWithLabel(mRegion, _("FAVORITE REGION"));

      std::string languageCode = Strings::ToLowerASCII(RecalboxConf::Instance().AsString("scraper.screenscraper.language", "en"));
      if (std::string("en|es|pt|fr|de|it|nl|ja|zh|ko|ru|da|fi|sv|hu|no|pl|cz|sk|tr").find(languageCode) == std::string::npos)
        languageCode = "en";

      mLanguage = std::make_shared< OptionListComponent<std::string> >(mWindow, _("SELECT FAVORITE LANGUAGE"), false);
      mLanguage->add(_("English")   , "en", languageCode == "en" );
      mLanguage->add(_("Español")   , "es", languageCode == "es" );
      mLanguage->add(_("Português") , "pt", languageCode == "pt" );
      mLanguage->add(_("Français")  , "fr", languageCode == "fr");
      mLanguage->add(_("Deutsch")   , "de", languageCode == "de");
      mLanguage->add(_("Italiano")  , "it", languageCode == "it");
      mLanguage->add(_("Nederlands"), "nl", languageCode == "nl");
      mLanguage->add(_("日本語")     , "ja", languageCode == "ja");
      mLanguage->add(_("简体中文")   , "zh", languageCode == "zh");
      mLanguage->add(_("한국어")     , "ko", languageCode == "ko");
      mLanguage->add(_("Русский")   , "ru", languageCode == "ru");
      mLanguage->add(_("Dansk")     , "da", languageCode == "da");
      mLanguage->add(_("Suomi")     , "fi", languageCode == "fi");
      mLanguage->add(_("Svenska")   , "sv", languageCode == "sv");
      mLanguage->add(_("Magyar")    , "hu", languageCode == "hu");
      mLanguage->add(_("Norsk")     , "no", languageCode == "no");
      mLanguage->add(_("Polski")    , "pl", languageCode == "pl");
      mLanguage->add(_("Čeština")   , "cz", languageCode == "cz");
      mLanguage->add(_("Slovenčina"), "sk", languageCode == "sk");
      mLanguage->add(_("Türkçe")    , "tr", languageCode == "tr");
      mMenu.addWithLabel(mLanguage, _("FAVORITE LANGUAGE"));

      //mMixImages = std::make_shared<SwitchComponent>(mWindow);
      //mMixImages->setState(true);
      //mMenu.addWithLabel(mMixImages, _("USE COMPOSED VISUALS"));

      break;
    }
    case ScraperFactory::ScraperType::TheGameDB:
    {
      mApproveResults = std::make_shared<SwitchComponent>(mWindow);
      mApproveResults->setState(false);
      mMenu.addWithLabel(mApproveResults, _("USER DECIDES ON CONFLICTS"));

      break;
    }
  }

  // scrape ratings
  mScrapeRatings = std::make_shared<SwitchComponent>(mWindow);
  mScrapeRatings->setState(Settings::Instance().ScrapeRatings());
  mMenu.addWithLabel(mScrapeRatings, _("SCRAPE RATINGS"), _(MENUMESSAGE_SCRAPER_RATINGS_HELP_MSG));

	mMenu.addButton(_("START"), "start", std::bind(&GuiScraperOptions::pressedStart, this));
	mMenu.addButton(_("BACK"), "back", [&] { Close(); });

	mMenu.setPosition((Renderer::getDisplayWidthAsFloat() - mMenu.getSize().x()) / 2, (Renderer::getDisplayHeightAsFloat() - mMenu.getSize().y()) / 2);
}

void GuiScraperOptions::pressedStart()
{
	for (auto& system : mSystems->getSelectedObjects())
	{
		if(system->PlatformCount() == 0)
		{
			mWindow.pushGui(new GuiMsgBox(mWindow,
				_("WARNING: SOME OF YOUR SELECTED SYSTEMS DO NOT HAVE A PLATFORM SET. RESULTS MAY BE EVEN MORE INACCURATE THAN USUAL!\nCONTINUE ANYWAY?"), 
						       _("YES"), std::bind(&GuiScraperOptions::start, this),
						       _("NO"), nullptr));
			return;
		}
	}

	start();
}

void GuiScraperOptions::start()
{
  // Select option regarding the selected scraper
  ScraperFactory::ScraperType type = ScraperFactory::Instance().GetScraperType(Settings::Instance().Scraper());
  // Save options
  switch(type)
  {
    case ScraperFactory::ScraperType::ScreenScraper:
    {
      RecalboxConf::Instance().SetString("scraper.screenscraper.media", mImage->getSelected());
      RecalboxConf::Instance().SetString("scraper.screenscraper.thumbnail", mThumbnail->getSelected());
      RecalboxConf::Instance().SetString("scraper.screenscraper.video", mVideo->getSelected());
      RecalboxConf::Instance().SetString("scraper.screenscraper.region", mRegion->getSelected());
      RecalboxConf::Instance().SetString("scraper.screenscraper.language", mLanguage->getSelected());
      RecalboxConf::Instance().SaveRecalboxConf();
      break;
    }
    case ScraperFactory::ScraperType::TheGameDB:
    {
      break;
    }
  }
  Settings::Instance().SetScrapeRatings(mScrapeRatings->getState());
  /*if(Settings::Instance().Scraper() == "Screenscraper") {
		Settings::Instance().SetMixImages(mMixImages->getState());
	}*/

  GuiScraperRun* gsm = new GuiScraperRun(mWindow, mSystemManager, mSystems->getSelectedObjects(), mScrapingMethod->getSelected());
  mWindow.pushGui(gsm);
  Close();
}

bool GuiScraperOptions::ProcessInput(const InputCompactEvent& event)
{
	bool consumed = Component::ProcessInput(event);
	if(consumed)
		return true;
	
	if (event.APressed())
	{
		Close();
		return true;
	}

	if (event.StartPressed())
	{
	  mWindow.CloseAll();
    return true;
	}

	return false;
}

bool GuiScraperOptions::getHelpPrompts(Help& help)
{
	mMenu.getHelpPrompts(help);
	help.Set(HelpType::A, _("BACK"))
	    .Set(HelpType::Start, _("CLOSE"));
	return true;
}
