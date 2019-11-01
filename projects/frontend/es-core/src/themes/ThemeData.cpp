#include "ThemeData.h"
#include "Renderer.h"
#include "Sound.h"
#include "resources/TextureResource.h"
#include "utils/Log.h"
#include "Settings.h"
#include "pugixml/pugixml.hpp"
#include <memory>
#include <boost/algorithm/string/replace.hpp>
#include <components/VideoComponent.h>

#include "components/ImageComponent.h"
#include "components/TextComponent.h"
#include "RootFolders.h"

std::vector<std::string>& ThemeData::SupportedViews()
{
  static std::vector<std::string> _SupportedViews =
  {
    { "system"  },
    { "basic"   },
    { "detailed"},
    { "menu"    },
  };
  return _SupportedViews;
}

std::vector<std::string>& ThemeData::SupportedFeatures()
{
  static std::vector<std::string> _SupportedFeatures =
  {
    {"carousel" },
    {"z-index"  },
  };
  return _SupportedFeatures;
}

std::map< std::string, std::map<std::string, ThemeData::ElementProperty> >& ThemeData::ElementMap()
{
  static std::map< std::string, std::map<std::string, ThemeData::ElementProperty> > _ElementMap =
  {
    { "image",
      {
        { "pos", ElementProperty::NormalizedPair },
        { "size", ElementProperty::NormalizedPair },
        { "maxSize", ElementProperty::NormalizedPair },
        { "origin", ElementProperty::NormalizedPair },
        { "rotation", ElementProperty::Float },
        { "rotationOrigin", ElementProperty::NormalizedPair },
        { "path", ElementProperty::Path },
        { "tile", ElementProperty::Boolean },
        { "color", ElementProperty::Color },
        { "zIndex", ElementProperty::Float },
      },
    },
    { "video",
      {
        { "pos", ElementProperty::NormalizedPair },
        { "size", ElementProperty::NormalizedPair },
        { "maxSize", ElementProperty::NormalizedPair },
        { "origin", ElementProperty::NormalizedPair },
        { "rotation", ElementProperty::Float },
        { "rotationOrigin", ElementProperty::NormalizedPair },
        { "path", ElementProperty::Path },
        { "zIndex", ElementProperty::Float },
      },
    },
    { "text",
      {
        { "pos", ElementProperty::NormalizedPair },
        { "size", ElementProperty::NormalizedPair },
        { "origin", ElementProperty::NormalizedPair },
        { "rotation", ElementProperty::Float },
        { "rotationOrigin", ElementProperty::NormalizedPair },
        { "text", ElementProperty::String },
        { "backgroundColor", ElementProperty::Color },
        { "fontPath", ElementProperty::Path },
        { "fontSize", ElementProperty::Float },
        { "color", ElementProperty::Color },
        { "alignment", ElementProperty::String },
        { "forceUppercase", ElementProperty::Boolean },
        { "lineSpacing", ElementProperty::Float },
        { "value", ElementProperty::String },
        { "zIndex", ElementProperty::Float },
      },
    },
    { "textlist",
      {
        { "pos", ElementProperty::NormalizedPair },
        { "size", ElementProperty::NormalizedPair },
        { "origin", ElementProperty::NormalizedPair },
        { "selectorHeight", ElementProperty::Float },
        { "selectorOffsetY", ElementProperty::Float },
        { "selectorColor", ElementProperty::Color },
        { "selectorImagePath", ElementProperty::Path },
        { "selectorImageTile", ElementProperty::Boolean },
        { "selectedColor", ElementProperty::Color },
        { "primaryColor", ElementProperty::Color },
        { "secondaryColor", ElementProperty::Color },
        { "fontPath", ElementProperty::Path },
        { "fontSize", ElementProperty::Float },
        { "scrollSound", ElementProperty::Path },
        { "alignment", ElementProperty::String },
        { "horizontalMargin", ElementProperty::Float },
        { "forceUppercase", ElementProperty::Boolean },
        { "lineSpacing", ElementProperty::Float },
        { "zIndex", ElementProperty::Float },
      },
    },
    { "container",
      {
        { "pos", ElementProperty::NormalizedPair },
        { "size", ElementProperty::NormalizedPair },
        { "origin", ElementProperty::NormalizedPair },
        { "zIndex", ElementProperty::Float },
      },
    },
    { "ninepatch",
      {
        { "pos", ElementProperty::NormalizedPair },
        { "size", ElementProperty::NormalizedPair },
        { "path", ElementProperty::Path },
        { "zIndex", ElementProperty::Float },
      },
    },
    { "datetime",
      {
        { "pos", ElementProperty::NormalizedPair },
        { "size", ElementProperty::NormalizedPair },
        { "color", ElementProperty::Color },
        { "fontPath", ElementProperty::Path },
        { "fontSize", ElementProperty::Float },
        { "alignment", ElementProperty::String },
        { "forceUppercase", ElementProperty::Boolean },
        { "zIndex", ElementProperty::Float },
      },
    },
    { "rating",
      {
        { "pos", ElementProperty::NormalizedPair },
        { "size", ElementProperty::NormalizedPair },
        { "origin", ElementProperty::NormalizedPair },
        { "rotation", ElementProperty::Float },
        { "rotationOrigin", ElementProperty::NormalizedPair },
        { "filledPath", ElementProperty::Path },
        { "unfilledPath", ElementProperty::Path },
        { "zIndex", ElementProperty::Float },
      },
    },
    { "sound",
      {
        { "path", ElementProperty::Path },
      },
    },
    { "helpsystem",
      {
        { "pos", ElementProperty::NormalizedPair },
        { "textColor", ElementProperty::Color },
        { "iconColor", ElementProperty::Color },
        { "fontPath", ElementProperty::Path },
        { "fontSize", ElementProperty::Float },
        { "iconUpDown", ElementProperty::Path },
        { "iconLeftRight", ElementProperty::Path },
        { "iconUpDownLeftRight", ElementProperty::Path },
        { "iconA", ElementProperty::Path },
        { "iconB", ElementProperty::Path },
        { "iconX", ElementProperty::Path },
        { "iconY", ElementProperty::Path },
        { "iconL", ElementProperty::Path },
        { "iconR", ElementProperty::Path },
        { "iconStart", ElementProperty::Path },
        { "iconSelect", ElementProperty::Path },
      },
    },
    { "carousel",
      {
        { "type", ElementProperty::String },
        { "size", ElementProperty::NormalizedPair },
        { "pos", ElementProperty::NormalizedPair },
        { "origin", ElementProperty::NormalizedPair },
        { "color", ElementProperty::Color },
        { "logoScale", ElementProperty::Float },
        { "logoRotation", ElementProperty::Float },
        { "logoRotationOrigin", ElementProperty::NormalizedPair },
        { "logoSize", ElementProperty::NormalizedPair },
        { "logoAlignment", ElementProperty::String },
        { "maxLogoCount", ElementProperty::Float },
        { "defaultTransition", ElementProperty::String },
        { "zIndex", ElementProperty::Float },
      },
    },
    { "menuBackground",
      {
        { "color", ElementProperty::Color },
        { "path", ElementProperty::Path },
        { "fadePath", ElementProperty::Path },
      },
    },
    { "menuIcons",
      {
        { "iconKodi", ElementProperty::Path },
        { "iconSystem", ElementProperty::Path },
        { "iconSystem", ElementProperty::Path },
        { "iconUpdates", ElementProperty::Path },
        { "iconControllers", ElementProperty::Path },
        { "iconGames", ElementProperty::Path },
        { "iconUI", ElementProperty::Path },
        { "iconSound", ElementProperty::Path },
        { "iconNetwork", ElementProperty::Path },
        { "iconScraper", ElementProperty::Path },
        { "iconAdvanced", ElementProperty::Path },
        { "iconQuit", ElementProperty::Path },
        { "iconRestart", ElementProperty::Path },
        { "iconShutdown", ElementProperty::Path },
        { "iconFastShutdown", ElementProperty::Path },
      },
    },
    { "menuSwitch",
      {
        { "pathOn", ElementProperty::Path },
        { "pathOff", ElementProperty::Path },
      },
    },
    { "menuSlider",
      {
        { "path", ElementProperty::Path },
      },
    },
    { "menuButton",
      {
        { "path", ElementProperty::Path },
        { "filledPath", ElementProperty::Path },
      },
    },
    { "menuText",
      {
        { "fontPath", ElementProperty::Path },
        { "fontSize", ElementProperty::Float },
        { "color", ElementProperty::Color },
        { "separatorColor", ElementProperty::Color },
        { "selectedColor", ElementProperty::Color },
        { "selectorColor", ElementProperty::Color },
      },
    },
    { "menuTextSmall",
      {
        { "fontPath", ElementProperty::Path },
        { "fontSize", ElementProperty::Float },
        { "color", ElementProperty::Color },
        { "selectedColor", ElementProperty::Color },
        { "selectorColor", ElementProperty::Color },
      },
    }
  };
  return _ElementMap;
}

namespace fs = boost::filesystem;

// helper
unsigned int getHexColor(const char* str)
{
	ThemeException error;
	if(str == nullptr)
		throw error << "Empty color";

	size_t len = strlen(str);
	if(len != 6 && len != 8)
		throw error << "Invalid color (bad length, \"" << str << "\" - must be 6 or 8)";

	unsigned int val;
	std::stringstream ss;
	ss << str;
	ss >> std::hex >> val;

	if(len == 6)
		val = (val << 8) | 0xFF;

	return val;
}

// helper
std::string resolvePath(const char* in, const fs::path& relative)
{
	if((in == nullptr) || in[0] == '\0')
		return in;

	fs::path relPath = relative.parent_path();
	
	boost::filesystem::path path(in);
	
	// we use boost filesystem here instead of just string checks because 
	// some directories could theoretically start with ~ or .
	if(*path.begin() == "~")
	{
		path = getHomePath() + (in + 1);
	}else if(*path.begin() == ".")
	{
		path = relPath / (in + 1);
	}

	return path.generic_string();
}

std::string ThemeData::resolveSystemVariable(const std::string& systemThemeFolder, const std::string& path)
{
	std::string result = path;
	boost::algorithm::replace_first(result, "$system", systemThemeFolder);
	return result;
}

ThemeData::ThemeData()
{
	mVersion = 0;
	mColorset = Settings::getInstance()->getString("ThemeColorSet");
	mIconset = Settings::getInstance()->getString("ThemeIconSet");
	mMenu = Settings::getInstance()->getString("ThemeMenu");
	mSystemview = Settings::getInstance()->getString("ThemeSystemView");
	mGamelistview = Settings::getInstance()->getString("ThemeGamelistView");
	Settings::getInstance()->setBool("ThemeHasMenuView", false);
	mSystemThemeFolder.clear();
}

void ThemeData::loadFile(const std::string& systemThemeFolder, const std::string& path)
{
	mPaths.push_back(path);

	ThemeException error;
	error.setFiles(mPaths);

	if(!fs::exists(path))
		throw error << "File does not exist!";

	mVersion = 0;
	mViews.clear();
	
	mSystemThemeFolder = systemThemeFolder;
	


	pugi::xml_document doc;
	pugi::xml_parse_result res = doc.load_file(path.c_str());
	if(!res)
		throw error << "XML parsing error: \n    " << res.description();

	pugi::xml_node root = doc.child("theme");
	if(!root)
		throw error << "Missing <theme> tag!";

	// parse version
	mVersion = root.child("formatVersion").text().as_float(-404);
	if(mVersion == -404)
		throw error << "<formatVersion> tag missing!\n   It's either out of date or you need to add <formatVersion>" << CURRENT_THEME_FORMAT_VERSION << "</formatVersion> inside your <theme> tag.";

	if(mVersion < MINIMUM_THEME_FORMAT_VERSION)
		throw error << "Theme uses format version " << mVersion << ". Minimum supported version is " << MINIMUM_THEME_FORMAT_VERSION << '.';
	
	parseIncludes(root);
	parseViews(root);
	parseFeatures(root);
	mPaths.pop_back();
}


void ThemeData::parseIncludes(const pugi::xml_node& root)
{
	ThemeException error;
	error.setFiles(mPaths);

	for (pugi::xml_node node = root.child("include"); node != nullptr; node = node.next_sibling("include"))
	{
		if (parseSubset(node))
		{
					
			std::string str = resolveSystemVariable(mSystemThemeFolder, node.text().get());
			
			//workaround for an issue in parseincludes introduced by variable implementation
			if (str.find("//") == std::string::npos)
			{
			
				const char* relPath = str.c_str();
				std::string path = resolvePath(relPath, mPaths.back());
				if(!ResourceManager::getInstance()->fileExists(path))
					throw error << "Included file \"" << relPath << "\" not found! (resolved to \"" << path << "\")";

				error << "    from included file \"" << relPath << "\":\n    ";

				mPaths.push_back(path);

				pugi::xml_document includeDoc;
				pugi::xml_parse_result result = includeDoc.load_file(path.c_str());
				if(!result)
					throw error << "Error parsing file: \n    " << result.description();

				pugi::xml_node root = includeDoc.child("theme");
				if(!root)
					throw error << "Missing <theme> tag!";
				parseIncludes(root);
				parseViews(root);
				parseFeatures(root);
			
				mPaths.pop_back();
			}			
		}		
	}
}

bool ThemeData::parseSubset(const pugi::xml_node& node)
{
	bool parse = true;
	
	if (node.attribute("subset") != nullptr)
	{
		parse = false;
		const std::string subsetAttr = node.attribute("subset").as_string();
		const std::string nameAttr = node.attribute("name").as_string();
		
		if (subsetAttr == "colorset" && nameAttr == mColorset)
		{	
			parse = true;
			return parse;
		}
		if (subsetAttr == "iconset" && nameAttr == mIconset)
		{	
			parse = true;
			return parse;
		}
		if (subsetAttr == "menu" && nameAttr == mMenu)
		{	
			parse = true;
			return parse;
		}
		if (subsetAttr == "systemview" && nameAttr == mSystemview)
		{	
			parse = true;
			return parse;
		}
		if (subsetAttr == "gamelistview" && nameAttr == mGamelistview)
		{	
			parse = true;
			return parse;
		}
	}
			
	
	return parse;
	
}

void ThemeData::parseFeatures(const pugi::xml_node& root)
{
	ThemeException error;
	error.setFiles(mPaths);

	for (pugi::xml_node node = root.child("feature"); node != nullptr; node = node.next_sibling("feature"))
	{
		if(!node.attribute("supported"))
			throw error << "Feature missing \"supported\" attribute!";

		const std::string supportedAttr = node.attribute("supported").as_string();

		if (std::find(SupportedFeatures().begin(), SupportedFeatures().end(), supportedAttr) != SupportedFeatures().end())
		{
			parseViews(node);
		}
	}
}

void ThemeData::parseViews(const pugi::xml_node& root)
{
	ThemeException error;
	error.setFiles(mPaths);

	// parse views
	for (pugi::xml_node node = root.child("view"); node != nullptr; node = node.next_sibling("view"))
	{
		if(!node.attribute("name"))
			throw error << "View missing \"name\" attribute!";

		const char* delim = " \t\r\n,";
		const std::string nameAttr = node.attribute("name").as_string();
		size_t prevOff = nameAttr.find_first_not_of(delim, 0);
		size_t off = nameAttr.find_first_of(delim, prevOff);
		std::string viewKey;
		while(off != std::string::npos || prevOff != std::string::npos)
		{
			viewKey = nameAttr.substr(prevOff, off - prevOff);
			if (viewKey == "menu")
				Settings::getInstance()->setBool("ThemeHasMenuView", true);
			prevOff = nameAttr.find_first_not_of(delim, off);
			off = nameAttr.find_first_of(delim, prevOff);
			
			if (std::find(SupportedViews().begin(), SupportedViews().end(), viewKey) != SupportedViews().end())
			{
				ThemeView& view = mViews.insert(std::pair<std::string, ThemeView>(viewKey, ThemeView())).first->second;
				parseView(node, view);
			}
		}
	}
}

void ThemeData::parseView(const pugi::xml_node& root, ThemeView& view)
{
	ThemeException error;
	error.setFiles(mPaths);

	for (pugi::xml_node node = root.first_child(); node != nullptr; node = node.next_sibling())
	{
		if(!node.attribute("name"))
			throw error << "Element of type \"" << node.name() << "\" missing \"name\" attribute!";
		if (std::string(node.name()) == "helpsystem")
			Settings::getInstance()->setBool("ThemeHasHelpSystem", true);

    const std::map< std::string, std::map<std::string, ThemeData::ElementProperty> >& elementMap = ElementMap();

		auto elemTypeIt = elementMap.find(node.name());
		if(elemTypeIt == elementMap.end())
			throw error << "Unknown element of type \"" << node.name() << "\"!";
    auto& subElementMap = elemTypeIt->second;

		if (parseRegion(node))
		{
			const char* delim = " \t\r\n,";
			const std::string nameAttr = node.attribute("name").as_string();
			size_t prevOff = nameAttr.find_first_not_of(delim, 0);
			size_t off =  nameAttr.find_first_of(delim, prevOff);
			while(off != std::string::npos || prevOff != std::string::npos)
			{
				std::string elemKey = nameAttr.substr(prevOff, off - prevOff);
				prevOff = nameAttr.find_first_not_of(delim, off);
				off = nameAttr.find_first_of(delim, prevOff);

				parseElement(node, subElementMap,
					view.elements.insert(std::pair<std::string, ThemeElement>(elemKey, ThemeElement())).first->second);

				if(std::find(view.orderedKeys.begin(), view.orderedKeys.end(), elemKey) == view.orderedKeys.end())
					view.orderedKeys.push_back(elemKey);
			}
		}
	}
}

bool ThemeData::parseRegion(const pugi::xml_node& node)
{
	bool parse = true;
	
	if (node.attribute("region") != nullptr)
	{
		std::string regionsetting = Settings::getInstance()->getString("ThemeRegionName");
		parse = false;
		const char* delim = " \t\r\n,";
		const std::string nameAttr = node.attribute("region").as_string();
		size_t prevOff = nameAttr.find_first_not_of(delim, 0);
		size_t off =  nameAttr.find_first_of(delim, prevOff);
		while(off != std::string::npos || prevOff != std::string::npos)
		{
			std::string elemKey = nameAttr.substr(prevOff, off - prevOff);
			prevOff = nameAttr.find_first_not_of(delim, off);
			off = nameAttr.find_first_of(delim, prevOff);
			if (elemKey == regionsetting)
			{	
				parse = true;
				return parse;
			}
		}
			
	}
	return parse;
	
}


void ThemeData::parseElement(const pugi::xml_node& root, const std::map<std::string, ElementProperty>& typeMap, ThemeElement& element)
{
	ThemeException error;
	error.setFiles(mPaths);

	element.type = root.name();
	element.extra = root.attribute("extra").as_bool(false);
	
	for (pugi::xml_node node = root.first_child(); node != nullptr; node = node.next_sibling())
	{
		auto typeIt = typeMap.find(node.name());
		if(typeIt == typeMap.end())
			throw error << "Unknown property type \"" << node.name() << "\" (for element of type " << root.name() << ").";
		
		std::string str = resolveSystemVariable(mSystemThemeFolder, node.text().as_string());

		switch(typeIt->second)
		{
		case ElementProperty::NormalizedPair:
		{

			size_t divider = str.find(' ');
			if(divider == std::string::npos) 
				throw error << "invalid normalized pair (property \"" << node.name() << "\", value \"" << str.c_str() << "\")";

			std::string first = str.substr(0, divider);
			std::string second = str.substr(divider, std::string::npos);

			Vector2f val(atof(first.c_str()), atof(second.c_str()));

			element.properties[node.name()] = val;
			break;
		}
		case ElementProperty::String:
			element.properties[node.name()] = str;
			break;
		case ElementProperty::Path:
		{
			std::string path = resolvePath(str.c_str(), mPaths.back().string());
			std::string variable = node.text().get();
			if(!ResourceManager::getInstance()->fileExists(path))
			{
				//too many warnings with region and system variable surcharge in themes
				if (!root.attribute("region") && variable.find("$system") == std::string::npos)
				{
					std::stringstream ss;
					ss << "  Warning " << error.msg; // "from theme yadda yadda, included file yadda yadda
					ss << "could not find file \"" << node.text().get() << "\" ";
					if(node.text().get() != path)
						ss << "(which resolved to \"" << path << "\") ";
					LOG(LogWarning) << ss.str();
				}
				break;
			}
			element.properties[node.name()] = path;
			break;
		}
		case ElementProperty::Color:
			element.properties[node.name()] = getHexColor(str.c_str());
			break;
		case ElementProperty::Float:
			{
			float floatVal = (float)(strtod(str.c_str(), nullptr));
			element.properties[node.name()] = floatVal;
  			break;
			}
		case ElementProperty::Boolean:
			{
			// only look at first char
			char first = str[0];
			// 1*, t* (true), T* (True), y* (yes), Y* (YES)
			bool boolVal = (first == '1' || first == 't' || first == 'T' || first == 'y' || first == 'Y');
			element.properties[node.name()] = boolVal;
  			break;
			}
		default:
			throw error << "Unknown ElementPropertyType for \"" << root.attribute("name").as_string() << "\", property " << node.name();
		}
	}
}


const ThemeData::ThemeElement* ThemeData::getElement(const std::string& view, const std::string& element, const std::string& expectedType) const
{
	auto viewIt = mViews.find(view);
	if(viewIt == mViews.end())
		return nullptr; // not found

	auto elemIt = viewIt->second.elements.find(element);
	if(elemIt == viewIt->second.elements.end()) return nullptr;

	if(elemIt->second.type != expectedType && !expectedType.empty())
	{
		LOG(LogWarning) << " requested mismatched theme type for [" << view << "." << element << "] - expected \"" 
			<< expectedType << "\", got \"" << elemIt->second.type << "\"";
		return nullptr;
	}

	return &elemIt->second;
}

const std::shared_ptr<ThemeData>& ThemeData::getDefault()
{
	static std::shared_ptr<ThemeData> theme = nullptr;
	if(theme == nullptr)
	{
		theme = std::make_shared<ThemeData>();

		const std::string path = RootFolders::DataRootFolder + "/system/.emulationstation/es_theme_default.xml";
		if(fs::exists(path))
		{
			try
			{
				std::string empty;
				theme->loadFile(empty, path);
			} catch(ThemeException& e)
			{
				LOG(LogError) << e.what();
				theme = std::make_shared<ThemeData>(); //reset to empty
			}
		}
	}

	return theme;
}

const std::shared_ptr<ThemeData>& ThemeData::getCurrent()
{
	static std::shared_ptr<ThemeData> theme = nullptr;
	if(theme == nullptr || Settings::getInstance()->getBool("ThemeChanged"))
	{
		theme = std::make_shared<ThemeData>();
		fs::path path;
		std::string currentTheme = Settings::getInstance()->getString("ThemeSet");
		static const size_t pathCount = 3;
		fs::path paths[pathCount] =
		{
			RootFolders::TemplateRootFolder + "/system/.emulationstation/themes/" + currentTheme,
			RootFolders::DataRootFolder     + "/themes/" + currentTheme,
			RootFolders::DataRootFolder     + "/system/.emulationstation/themes/" + currentTheme
		};
	

		fs::directory_iterator end;

		for (const auto& i : paths)
		{
			if(!fs::is_directory(i))
				continue;

			for (fs::directory_iterator it(i); it != end; ++it)
			{
				if(fs::is_directory(*it))
				{
					path = *it / "theme.xml";
					if(fs::exists(path))
					{
						try
						{
							std::string empty;
							theme->loadFile(empty, path.string());
							return theme;
						} catch(ThemeException& e)
						{
							LOG(LogError) << e.what();
							theme = std::make_shared<ThemeData>(); //reset to empty
						}
					}
				
				}
			}
			path = i / "theme.xml";
			if(fs::exists(path))
					{
						try
						{
							std::string empty;
							theme->loadFile(empty, path.string());
							return theme;
						} catch(ThemeException& e)
						{
							LOG(LogError) << e.what();
							theme = std::make_shared<ThemeData>(); //reset to empty
						}
					}
		}
	}
	return theme;
}

std::vector<GuiComponent*> ThemeData::makeExtras(const std::shared_ptr<ThemeData>& theme, const std::string& view, Window* window)
{
	std::vector<GuiComponent*> comps;

	auto viewIt = theme->mViews.find(view);
	if(viewIt == theme->mViews.end())
		return comps;
	
	for (auto& key : viewIt->second.orderedKeys)
	{
		ThemeElement& elem = viewIt->second.elements.at(key);
		if(elem.extra)
		{
			GuiComponent* comp = nullptr;
			const std::string& t = elem.type;
			if(t == "image")
				comp = new ImageComponent(window);

			else if (t == "video")
				comp = new VideoComponent(window);

			else if(t == "text")
				comp = new TextComponent(window);

			comp->setDefaultZIndex(10);
			comp->applyTheme(theme, view, key, ThemeProperties::All);
			comps.push_back(comp);
		}
	}

	return comps;
}

std::map<std::string, ThemeSet> ThemeData::getThemeSets()
{
	std::map<std::string, ThemeSet> sets;

	static const size_t pathCount = 3;
	fs::path paths[pathCount] =
	{
		RootFolders::TemplateRootFolder + "/system/.emulationstation/themes",
		RootFolders::DataRootFolder     + "/themes",
		RootFolders::DataRootFolder     + "/system/.emulationstation/themes"
	};

	fs::directory_iterator end;

	for (const auto& path : paths)
	{
		if(!fs::is_directory(path))
			continue;

		for (fs::directory_iterator it(path); it != end; ++it)
		{
			if(fs::is_directory(*it))
			{
				ThemeSet set = {*it};
				sets[set.getName()] = set;
			}
		}
	}

	return sets;
}

std::map<std::string, std::string> ThemeData::getThemeSubSets(const std::string& theme)
{
	std::map<std::string, std::string> sets;
	fs::path path;
	std::deque<boost::filesystem::path> dequepath;
	static const size_t pathCount = 3;
	fs::path paths[pathCount] =
	{
		RootFolders::TemplateRootFolder + "/system/.emulationstation/themes/" + theme,
		RootFolders::DataRootFolder     + "/themes/" + theme,
		RootFolders::DataRootFolder     + "/system/.emulationstation/themes/" + theme
	};

	fs::directory_iterator end;

	for (const auto& i : paths)
	{
		if(!fs::is_directory(i))
			continue;

		for (fs::directory_iterator it(i); it != end; ++it)
		{
			if(fs::is_directory(*it))
			{
				path = *it / "theme.xml";
				dequepath.push_back(path);
				pugi::xml_document doc;
				/*pugi::xml_parse_result res =*/ doc.load_file(path.c_str());
				pugi::xml_node root = doc.child("theme");
				crawlIncludes(root, sets, dequepath);
				dequepath.pop_back();
			}
		}
		path = i / "theme.xml";
			if(fs::exists(path))
			{
				dequepath.push_back(path);
				pugi::xml_document doc;
				/*pugi::xml_parse_result res =*/ doc.load_file(path.c_str());
				pugi::xml_node root = doc.child("theme");			
				crawlIncludes(root, sets, dequepath);
				findRegion(doc, sets);
				dequepath.pop_back();
			}
	}

	return sets;
}

void ThemeData::crawlIncludes(const pugi::xml_node& root, std::map<std::string, std::string>& sets, std::deque<boost::filesystem::path>& dequepath)
{
	for (pugi::xml_node node = root.child("include"); node != nullptr; node = node.next_sibling("include"))
	{
		
		sets[node.attribute("name").as_string()] = node.attribute("subset").as_string();
		
		const char* relPath = node.text().get();
		std::string path = resolvePath(relPath, dequepath.back());
		dequepath.push_back(path);
		pugi::xml_document includeDoc;
		/*pugi::xml_parse_result result =*/ includeDoc.load_file(path.c_str());
		pugi::xml_node root = includeDoc.child("theme");
		crawlIncludes(root, sets, dequepath);
		findRegion(includeDoc, sets);
		dequepath.pop_back();
	}
}

void ThemeData::findRegion(const pugi::xml_document& doc, std::map<std::string, std::string>& sets)
{
	pugi::xpath_node_set regionattr = doc.select_nodes("//@region");
	for (auto xpath_node : regionattr)
	{
		if (xpath_node.attribute() != nullptr)
			sets[xpath_node.attribute().value()] = "region";
	}
}

// as the getThemeSubSets process is heavy, doing it 1 time for all subsets then sorting on demand
std::map<std::string, std::string> ThemeData::sortThemeSubSets(const std::map<std::string, std::string>& subsetmap, const std::string& subset)
{
	std::map<std::string, std::string> sortedsets;
	
	for (const auto& it : subsetmap)
	{
		if (it.second == subset)
			sortedsets[it.first] = it.first;
	}
	return sortedsets;
}


fs::path ThemeData::getThemeFromCurrentSet(const std::string& system)
{
	auto themeSets = ThemeData::getThemeSets();
	if(themeSets.empty())
	{
		// no theme sets available
		return "";
	}

	auto set = themeSets.find(Settings::getInstance()->getString("ThemeSet"));
	if(set == themeSets.end())
	{
		// currently selected theme set is missing, so just pick the first available set
		set = themeSets.begin();
		Settings::getInstance()->setString("ThemeSet", set->first);
	}

	return set->second.getThemePath(system);
}

std::string ThemeData::getTransition()
{
	std::string result;
	auto elem = getElement("system", "systemcarousel", "carousel");
	if (elem != nullptr) {
		if (elem->has("defaultTransition")) {
			if (elem->get<std::string>("defaultTransition") == "instant") {
				result = "instant";
				return result;
			}
			if (elem->get<std::string>("defaultTransition") == "fade") {
				result = "fade";
				return result;
			}
			if (elem->get<std::string>("defaultTransition") == "slide") {
				result = "slide";
				return result;
			}
		}
	}
	return result;
}

bool ThemeData::isFolderHandled() const {
	auto elem = getElement("detailed", "md_folder_name", "text");
	return elem != nullptr ? elem->has("pos") : false;
}