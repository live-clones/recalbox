#pragma once

#include "utils/math/Vectors.h"
#include <memory>
#include <string>
#include <map>
#include <utils/os/fs/Path.h>

class ThemeData;
class Font;

struct HelpStyle
{
	Vector2f position;
	unsigned int iconColor;
	unsigned int textColor;
	std::shared_ptr<Font> font;
	std::map<std::string, Path> iconMap;

	HelpStyle(); // default values
	void applyTheme(const ThemeData& theme, const std::string& view);
};