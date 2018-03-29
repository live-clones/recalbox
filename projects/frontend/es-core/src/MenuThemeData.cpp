#include "MenuThemeData.h"
#include "Settings.h"
#include "ThemeData.h"
#include "Renderer.h"
#include "resources/Font.h"

std::shared_ptr<MenuThemeData> MenuThemeData::sInstance = NULL;

std::shared_ptr<MenuThemeData> MenuThemeData::getInstance() {
	if (sInstance == NULL || Settings::getInstance()->getBool("ThemeChanged"))
		sInstance = std::shared_ptr<MenuThemeData>(new MenuThemeData());
	return sInstance;
}

MenuThemeData::MenuThemeData(){
	
	mCurrent = std::shared_ptr<MenuTheme>(new MenuTheme);
	
	auto elem = ThemeData::getCurrent()->getElement("menu", "menubg", "menuBackground");
	
	if (elem)
	{
		if (elem->has("path"))
			mCurrent->menuBackground.path = elem->get<std::string>("path");
		if (elem->has("fadePath"))
			mCurrent->menuBackground.fadePath = elem->get<std::string>("fadePath");
		if (elem->has("color"))
			mCurrent->menuBackground.color = elem->get<unsigned int>("color");
	}
	
	elem = ThemeData::getCurrent()->getElement("menu", "menutitle", "menuText");
	
	if (elem)
	{
		if(elem->has("fontPath") || elem->has("fontSize"))
			mCurrent->menuTitle.font = Font::getFromTheme(elem, ThemeFlags::ALL, Font::get(FONT_SIZE_LARGE));
		if(elem->has("color"))
			mCurrent->menuTitle.color = elem->get<unsigned int>("color");
	}
	
	elem = ThemeData::getCurrent()->getElement("menu", "menufooter", "menuText");
	
	if (elem)
	{
		if(elem->has("fontPath") || elem->has("fontSize"))
			mCurrent->menuFooter.font = Font::getFromTheme(elem, ThemeFlags::ALL, Font::get(FONT_SIZE_SMALL));
		if(elem->has("color"))
			mCurrent->menuFooter.color = elem->get<unsigned int>("color");
	}
	
	elem = ThemeData::getCurrent()->getElement("menu", "menutext", "menuText");
	
	if (elem)
	{
		if(elem->has("fontPath") || elem->has("fontSize"))
		{
			mCurrent->menuText.font = Font::getFromTheme(elem, ThemeFlags::ALL, Font::get(FONT_SIZE_MEDIUM));
		}
			
		if(elem->has("color"))
			mCurrent->menuText.color = elem->get<unsigned int>("color");
		if(elem->has("separatorColor"))
			mCurrent->menuText.separatorColor = elem->get<unsigned int>("separatorColor");
		if(elem->has("selectedColor"))
			mCurrent->menuText.selectedColor = elem->get<unsigned int>("selectedColor");
		if(elem->has("selectorColor"))
			mCurrent->menuText.selectorColor = elem->get<unsigned int>("selectorColor");
	}
	
	elem = ThemeData::getCurrent()->getElement("menu", "menutextsmall", "menuTextSmall");
	
	if (elem)
	{
		if(elem->has("fontPath") || elem->has("fontSize"))
		{
			mCurrent->menuTextSmall.font = Font::getFromTheme(elem, ThemeFlags::ALL, Font::get(FONT_SIZE_SMALL));
		}
			
		if(elem->has("color"))
			mCurrent->menuTextSmall.color = elem->get<unsigned int>("color");
		if(elem->has("selectedColor"))
			mCurrent->menuText.selectedColor = elem->get<unsigned int>("selectedColor");
		if(elem->has("selectorColor"))
			mCurrent->menuText.selectedColor = elem->get<unsigned int>("selectorColor");
	}
	
	elem = ThemeData::getCurrent()->getElement("menu", "menubutton", "menuButton");
	
	if (elem)
	{
		if(elem->has("path"))
			mCurrent->iconSet.button = elem->get<std::string>("path");
		if(elem->has("filledPath"))
			mCurrent->iconSet.button_filled = elem->get<std::string>("filledPath");
	}
	
	elem = ThemeData::getCurrent()->getElement("menu", "menuswitch", "menuSwitch");
	
	if (elem)
	{
		if(elem->has("pathOn"))
			mCurrent->iconSet.on = elem->get<std::string>("pathOn");
		if(elem->has("pathOff"))
			mCurrent->iconSet.off = elem->get<std::string>("pathOff");
	}
	
	elem = ThemeData::getCurrent()->getElement("menu", "menuslider", "menuSlider");
	
	if (elem)
		if(elem->has("path"))
			mCurrent->iconSet.knob = elem->get<std::string>("path");

	elem = ThemeData::getCurrent()->getElement("menu", "menuicons", "menuIcons");

	if (elem) {
		if (elem->has("iconKodi"))
			mCurrent->menuIconSet.kodi = elem->get<std::string>("iconKodi");

		if (elem->has("iconSystem"))
			mCurrent->menuIconSet.system = elem->get<std::string>("iconSystem");

		if (elem->has("iconUpdates"))
			mCurrent->menuIconSet.updates = elem->get<std::string>("iconUpdates");

		if (elem->has("iconGames"))
			mCurrent->menuIconSet.games = elem->get<std::string>("iconGames");

		if (elem->has("iconControllers"))
			mCurrent->menuIconSet.controllers = elem->get<std::string>("iconControllers");

		if (elem->has("iconUI"))
			mCurrent->menuIconSet.ui = elem->get<std::string>("iconUI");

		if (elem->has("iconSound"))
			mCurrent->menuIconSet.sound = elem->get<std::string>("iconSound");

		if (elem->has("iconNetwork"))
			mCurrent->menuIconSet.network = elem->get<std::string>("iconNetwork");

		if (elem->has("iconScraper"))
			mCurrent->menuIconSet.scraper = elem->get<std::string>("iconScraper");

		if (elem->has("iconAdvanced"))
			mCurrent->menuIconSet.advanced = elem->get<std::string>("iconAdvanced");

		if (elem->has("iconQuit"))
            		mCurrent->menuIconSet.quit = elem->get<std::string>("iconQuit");

        	if (elem->has("iconRestart"))
            		mCurrent->menuIconSet.restart = elem->get<std::string>("iconRestart");

        	if (elem->has("iconShutdown"))
            		mCurrent->menuIconSet.shutdown = elem->get<std::string>("iconShutdown");

        	if (elem->has("iconFastShutdown"))
            		mCurrent->menuIconSet.fastshutdown = elem->get<std::string>("iconFastShutdown");
	}
		
	
}
