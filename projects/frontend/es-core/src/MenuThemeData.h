#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include "ThemeData.h"
#include "resources/Font.h"
#include "Renderer.h"

struct MenuElement {
	unsigned int color;
	unsigned int selectedColor;
	unsigned int selectorColor;
	unsigned int separatorColor;
	std::string path;
	std::string fadePath;
	std::shared_ptr<Font> font;
};

struct IconElement {
	std::string button;
	std::string button_filled;
	std::string on;
	std::string off;
	std::string option_arrow;
	std::string arrow;
	std::string knob;
};

struct MenuIconElement {
	std::string kodi;
	std::string system;
	std::string updates;
	std::string games;
	std::string controllers;
	std::string ui;
	std::string sound;
	std::string network;
	std::string scraper;
	std::string advanced;
	std::string quit;
	std::string restart;
	std::string shutdown;
	std::string fastshutdown;
};

struct MenuTheme {
	MenuElement menuBackground{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, ":/frame.png", ":/scroll_gradient.png", nullptr};
	MenuElement menuTitle{ 0x555555FF, 0x555555FF, 0x555555FF, 0xFFFFFFFF, "", "", Font::get(FONT_SIZE_LARGE)};
	MenuElement menuFooter{ 0xC6C6C6FF, 0xC6C6C6FF, 0xC6C6C6FF, 0xFFFFFFFF, "", "", Font::get(FONT_SIZE_SMALL)};
	MenuElement menuText{ 0x777777FF, 0xFFFFFFFF, 0x878787FF, 0xC6C7C6FF, "", "", Font::get(FONT_SIZE_MEDIUM)};
	MenuElement menuTextSmall{ 0x777777FF, 0xFFFFFFFF, 0x878787FF, 0xC6C7C6FF, "", "", Font::get(FONT_SIZE_SMALL)};
	MenuIconElement menuIconSet{"","","","","","","","","","",""};
	IconElement iconSet{ ":/button.png", ":/button_filled.png", ":/on.svg", ":/off.svg", ":/option_arrow.svg", ":/arrow.svg", ":/slider_knob.svg" };
};

class MenuThemeData
{
public:
	static std::shared_ptr<MenuThemeData> getInstance();
	inline std::shared_ptr<MenuTheme> getCurrentTheme() { return mCurrent; };
	
private:
	MenuThemeData();
	std::shared_ptr<MenuTheme> mCurrent;
	static std::shared_ptr<MenuThemeData> sInstance;
};
