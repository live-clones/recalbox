#include <themes/MenuThemeData.h>

std::shared_ptr<MenuThemeData> MenuThemeData::sInstance = nullptr;

std::shared_ptr<MenuThemeData> MenuThemeData::getInstance() {
	if (sInstance == nullptr)
		sInstance = std::shared_ptr<MenuThemeData>(new MenuThemeData());
	return sInstance;
}

void MenuThemeData::Reset()
{
  sInstance = nullptr;
  getInstance();
}

MenuThemeData::MenuThemeData()
{
	mCurrent = std::make_shared<MenuTheme>();
	const auto *elem = ThemeData::getCurrent().getElement("menu", "menuSize", "menuSize");

	if (elem != nullptr) {
		if (elem->HasProperty("height"))
			mCurrent->menuSize.height = elem->AsFloat("height");
	}

	elem = ThemeData::getCurrent().getElement("menu", "menubg", "menuBackground");
	
	if (elem != nullptr)
	{
		if (elem->HasProperty("path"))
			mCurrent->menuBackground.path = Path(elem->AsString("path"));
		if (elem->HasProperty("fadePath"))
			mCurrent->menuBackground.fadePath = Path(elem->AsString("fadePath"));
		if (elem->HasProperty("color"))
			mCurrent->menuBackground.color = (unsigned int)elem->AsInt("color");
	}
	
	elem = ThemeData::getCurrent().getElement("menu", "menutitle", "menuText");
	
	if (elem != nullptr)
	{
		if(elem->HasProperty("fontPath") || elem->HasProperty("fontSize"))
			mCurrent->menuTitle.font = Font::getFromTheme(elem, ThemeProperties::All, Font::get(FONT_SIZE_LARGE));
		if(elem->HasProperty("color"))
			mCurrent->menuTitle.color = (unsigned int)elem->AsInt("color");
	}
	
	elem = ThemeData::getCurrent().getElement("menu", "menufooter", "menuText");
	
	if (elem != nullptr)
	{
		if(elem->HasProperty("fontPath") || elem->HasProperty("fontSize"))
			mCurrent->menuFooter.font = Font::getFromTheme(elem, ThemeProperties::All, Font::get(FONT_SIZE_SMALL));
		if(elem->HasProperty("color"))
			mCurrent->menuFooter.color = (unsigned int)elem->AsInt("color");
	}
	
	elem = ThemeData::getCurrent().getElement("menu", "menutext", "menuText");
	
	if (elem != nullptr)
	{
		if(elem->HasProperty("fontPath") || elem->HasProperty("fontSize"))
		{
			mCurrent->menuText.font = Font::getFromTheme(elem, ThemeProperties::All, Font::get(FONT_SIZE_MEDIUM));
		}
			
		if(elem->HasProperty("color"))
			mCurrent->menuText.color = (unsigned int)elem->AsInt("color");
		if(elem->HasProperty("separatorColor"))
			mCurrent->menuText.separatorColor = (unsigned int)elem->AsInt("separatorColor");
		if(elem->HasProperty("selectedColor"))
			mCurrent->menuText.selectedColor = (unsigned int)elem->AsInt("selectedColor");
		if(elem->HasProperty("selectorColor"))
			mCurrent->menuText.selectorColor = (unsigned int)elem->AsInt("selectorColor");
	}
	
	elem = ThemeData::getCurrent().getElement("menu", "menutextsmall", "menuTextSmall");
	
	if (elem != nullptr)
	{
		if(elem->HasProperty("fontPath") || elem->HasProperty("fontSize"))
		{
			mCurrent->menuTextSmall.font = Font::getFromTheme(elem, ThemeProperties::All, Font::get(FONT_SIZE_SMALL));
		}
			
		if(elem->HasProperty("color"))
			mCurrent->menuTextSmall.color = (unsigned int)elem->AsInt("color");
		if(elem->HasProperty("selectedColor"))
			mCurrent->menuText.selectedColor = (unsigned int)elem->AsInt("selectedColor");
		if(elem->HasProperty("selectorColor"))
			mCurrent->menuText.selectedColor = (unsigned int)elem->AsInt("selectorColor");
	}
	
	elem = ThemeData::getCurrent().getElement("menu", "menubutton", "menuButton");
	
	if (elem != nullptr)
	{
		if(elem->HasProperty("path"))
			mCurrent->iconSet.button = elem->AsString("path");
		if(elem->HasProperty("filledPath"))
			mCurrent->iconSet.button_filled = elem->AsString("filledPath");
	}
	
	elem = ThemeData::getCurrent().getElement("menu", "menuswitch", "menuSwitch");
	
	if (elem != nullptr)
	{
		if(elem->HasProperty("pathOn"))
			mCurrent->iconSet.on = elem->AsString("pathOn");
		if(elem->HasProperty("pathOff"))
			mCurrent->iconSet.off = elem->AsString("pathOff");
	}
	
	elem = ThemeData::getCurrent().getElement("menu", "menuslider", "menuSlider");
	
	if (elem != nullptr)
		if(elem->HasProperty("path"))
			mCurrent->iconSet.knob = elem->AsString("path");

	elem = ThemeData::getCurrent().getElement("menu", "menuicons", "menuIcons");

	if (elem != nullptr) {
		if (elem->HasProperty("iconKodi"))
			mCurrent->menuIconSet.kodi = elem->AsString("iconKodi");

		if (elem->HasProperty("iconSystem"))
			mCurrent->menuIconSet.system = elem->AsString("iconSystem");

		if (elem->HasProperty("iconRecalboxRGBDual"))
			mCurrent->menuIconSet.recalboxrgbdual = elem->AsString("iconRecalboxRGBDual");

		if (elem->HasProperty("iconUpdates"))
			mCurrent->menuIconSet.updates = elem->AsString("iconUpdates");

		if (elem->HasProperty("iconGames"))
			mCurrent->menuIconSet.games = elem->AsString("iconGames");

		if (elem->HasProperty("iconControllers"))
			mCurrent->menuIconSet.controllers = elem->AsString("iconControllers");

		if (elem->HasProperty("iconUI"))
			mCurrent->menuIconSet.ui = elem->AsString("iconUI");

		if (elem->HasProperty("iconSound"))
			mCurrent->menuIconSet.sound = elem->AsString("iconSound");

		if (elem->HasProperty("iconNetwork"))
			mCurrent->menuIconSet.network = elem->AsString("iconNetwork");

		if (elem->HasProperty("iconScraper"))
			mCurrent->menuIconSet.scraper = elem->AsString("iconScraper");

		if (elem->HasProperty("iconAdvanced"))
			mCurrent->menuIconSet.advanced = elem->AsString("iconAdvanced");

		if (elem->HasProperty("iconQuit"))
            		mCurrent->menuIconSet.quit = elem->AsString("iconQuit");

    if (elem->HasProperty("iconRestart"))
          mCurrent->menuIconSet.restart = elem->AsString("iconRestart");

    if (elem->HasProperty("iconShutdown"))
          mCurrent->menuIconSet.shutdown = elem->AsString("iconShutdown");

    if (elem->HasProperty("iconFastShutdown"))
          mCurrent->menuIconSet.fastshutdown = elem->AsString("iconFastShutdown");

    if (elem->HasProperty("iconLicense"))
      mCurrent->menuIconSet.license = elem->AsString("iconLicense");
	}
}
