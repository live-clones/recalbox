#pragma once

#include <Eigen/Dense>
#include <memory>
#include <string>
#include <map>

class ThemeData;
class Font;

struct HelpStyle
{
	Eigen::Vector2f position;
	unsigned int iconColor;
	unsigned int textColor;
	std::shared_ptr<Font> font;
	std::map<std::string, std::string> iconMap;

	HelpStyle(); // default values
	void applyTheme(const std::shared_ptr<ThemeData>& theme, const std::string& view);
};