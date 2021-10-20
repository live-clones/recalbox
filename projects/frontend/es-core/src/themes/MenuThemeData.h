#pragma once

#include <utils/math/Misc.h>
#include <themes/ThemeData.h>
#include <resources/Font.h>
#include <Renderer.h>

struct MenuElement {
	unsigned int color;
	unsigned int selectedColor;
	unsigned int selectorColor;
	unsigned int separatorColor;
	Path path;
	Path fadePath;
	std::shared_ptr<Font> font;
};

struct MenuSize {
  float height;
};

struct IconElement {
	Path button;
  Path button_filled;
  Path on;
  Path off;
  Path option_arrow;
  Path arrow;
  Path knob;
};

struct MenuIconElement {
  Path kodi;
  Path system;
  Path updates;
  Path games;
  Path controllers;
  Path ui;
  Path sound;
  Path network;
  Path scraper;
  Path advanced;
  Path quit;
  Path restart;
  Path shutdown;
  Path fastshutdown;
  Path license;
};

struct MenuTheme
{
	MenuElement menuBackground{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, Path(":/frame.png"), Path(":/scroll_gradient.png"), nullptr};
	MenuElement menuTitle{ 0x555555FF, 0x555555FF, 0x555555FF, 0xFFFFFFFF, Path::Empty, Path::Empty, Font::get(FONT_SIZE_LARGE)};
	MenuElement menuFooter{ 0xC6C6C6FF, 0xC6C6C6FF, 0xC6C6C6FF, 0xFFFFFFFF, Path::Empty, Path::Empty, Font::get(FONT_SIZE_SMALL)};
	MenuElement menuText{ 0x777777FF, 0xFFFFFFFF, 0x878787FF, 0xC6C7C6FF, Path::Empty, Path::Empty, Font::get(FONT_SIZE_MEDIUM)};
	MenuElement menuTextSmall{ 0x777777FF, 0xFFFFFFFF, 0x878787FF, 0xC6C7C6FF, Path::Empty, Path::Empty, Font::get(FONT_SIZE_SMALL)};
	MenuIconElement menuIconSet; //{Path::Empty,Path::Empty,Path::Empty,Path::Empty,Path::Empty,Path::Empty,Path::Empty,Path::Empty,Path::Empty,Path::Empty,Path::Empty,Path::Empty,Path::Empty,Path::Empty,Path::Empty};
	IconElement iconSet{ Path(":/button.png"), Path(":/button_filled.png"), Path(":/on.svg"), Path(":/off.svg"), Path(":/option_arrow.svg"), Path(":/arrow.svg"), Path(":/slider_knob.svg") };
	MenuSize menuSize {height: 0.85f};
};

class MenuThemeData
{
public:
	static std::shared_ptr<MenuThemeData> getInstance();
	inline std::shared_ptr<MenuTheme> getCurrentTheme() { return mCurrent; };

	/*!
	 * @brief Destroy the singleton and force menu theme reloading
	 */
	static void Reset();
	
private:
	MenuThemeData();
	std::shared_ptr<MenuTheme> mCurrent;
	static std::shared_ptr<MenuThemeData> sInstance;
};
