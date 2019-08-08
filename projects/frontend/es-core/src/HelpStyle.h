#pragma once

#include "utils/math/Vectors.h"
#include <memory>
#include <string>
#include <map>

class ThemeData;
class Font;

struct HelpStyle
{
	Vector2f position;
	unsigned int iconColor;
	unsigned int textColor;
	std::shared_ptr<Font> font;
	std::map<std::string, std::string> iconMap;

	HelpStyle(); // default values
	void applyTheme(const std::shared_ptr<ThemeData>& theme, const std::string& view);
};