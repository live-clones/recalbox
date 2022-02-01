#include "ThemeData.h"
#include "audio/Sound.h"
#include "resources/TextureResource.h"
#include "utils/Log.h"
#include "pugixml/pugixml.hpp"
#include <components/VideoComponent.h>
#include <utils/Strings.h>
#include <algorithm>
#include <MainRunner.h>
#include "components/ImageComponent.h"
#include "components/TextComponent.h"
#include "RootFolders.h"
#include "ThemeException.h"
#include "MenuThemeData.h"

ThemeData* ThemeData::sCurrent = nullptr;
bool ThemeData::sThemeChanged = false;
bool ThemeData::sThemeHasMenuView = true;
bool ThemeData::sThemeHasHelpSystem = true;

std::vector<std::string>& ThemeData::SupportedViews()
{
  static std::vector<std::string> sSupportedViews =
  {
    { "system"  },
    { "basic"   },
    { "detailed"},
    { "menu"    },
    { "gameclip"},
  };
  return sSupportedViews;
}

std::vector<std::string>& ThemeData::SupportedFeatures()
{
  static std::vector<std::string> sSupportedFeatures =
  {
    {"carousel" },
    {"z-index"  },
  };
  return sSupportedFeatures;
}

std::map< std::string, std::map<std::string, ThemeData::ElementProperty> >& ThemeData::ElementMap()
{
  static std::map< std::string, std::map<std::string, ThemeData::ElementProperty> > sElementMap =
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
        { "disabled", ElementProperty::Boolean },
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
        { "disabled", ElementProperty::Boolean },
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
        { "disabled", ElementProperty::Boolean },
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
        { "disabled", ElementProperty::Boolean },
      },
    },
    { "ninepatch",
      {
        { "pos", ElementProperty::NormalizedPair },
        { "size", ElementProperty::NormalizedPair },
        { "path", ElementProperty::Path },
        { "zIndex", ElementProperty::Float },
        { "disabled", ElementProperty::Boolean },
      },
    },
    { "datetime",
      {
        { "pos", ElementProperty::NormalizedPair },
        { "size", ElementProperty::NormalizedPair },
        { "origin", ElementProperty::NormalizedPair },
        { "color", ElementProperty::Color },
        { "fontPath", ElementProperty::Path },
        { "fontSize", ElementProperty::Float },
        { "alignment", ElementProperty::String },
        { "forceUppercase", ElementProperty::Boolean },
        { "zIndex", ElementProperty::Float },
        { "disabled", ElementProperty::Boolean },
        { "display", ElementProperty::String },
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
        { "disabled", ElementProperty::Boolean },
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
        { "iconLicense", ElementProperty::Path },
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
    },
    { "menuSize",
      {
        { "height", ElementProperty::Float },
      },
    }
  };
  return sElementMap;
}

// helper
unsigned int getHexColor(const char* str)
{
	if(str == nullptr)
		throw ThemeException("Empty color");

	size_t len = strlen(str);
	int val = 0;
	if ((len != 6 && len != 8) || !Strings::HexToInt(str, val))
    throw ThemeException("Invalid color (bad length, \"" + std::string(str) + "\" - must be 6 or 8)");

	if(len == 6) val = (val << 8) | 0xFF;
	return (unsigned int)val;
}

ThemeData::ThemeData()
{
	mVersion = 0;
	SetThemeHasMenuView(false);
	mSystemThemeFolder.clear();
  mRandomPath.clear();
}

bool ThemeData::CheckThemeOption(std::string& selected, const std::map<std::string, std::string>& subsets, const std::string& subset)
{
  std::map<std::string, std::string> map = sortThemeSubSets(subsets, subset);
  // Empty subset?
  if (subsets.empty()) return false;
  if (map.empty()) return false;
  // Try to fix the value if not found
  auto selectedColorSet = map.find(selected);
  if (selectedColorSet == map.end())
  {
    selected = map.begin()->first;
    return true;
  }
  return false;
}

void ThemeData::loadFile(const std::string& systemThemeFolder, const Path& path)
{
	mPaths.push_back(path);

	if(!path.Exists())
		throw ThemeException("File does not exist!", mPaths);

	mVersion = 0;
	mViews.clear();
	
	mSystemThemeFolder = systemThemeFolder;

	std::string themeName = path.IsDirectory() ? path.Filename() : path.Directory().Filename();
	std::map<std::string, std::string> subSets = getThemeSubSets(themeName);

	bool main = systemThemeFolder.empty();
  bool needSave = false;
  mColorset = RecalboxConf::Instance().GetThemeColorSet(themeName);
  if (main && CheckThemeOption(mColorset, subSets, "colorset")) { RecalboxConf::Instance().SetThemeColorSet(themeName, mColorset); needSave = true; }
  mIconset = RecalboxConf::Instance().GetThemeIconSet(themeName);
  if (main && CheckThemeOption(mIconset, subSets, "iconset")) { RecalboxConf::Instance().SetThemeIconSet(themeName, mIconset); needSave = true; }
  mMenu = RecalboxConf::Instance().GetThemeMenuSet(themeName);
  if (main && CheckThemeOption(mMenu, subSets, "menu")) { RecalboxConf::Instance().SetThemeMenuSet(themeName, mMenu); needSave = true; }
  mSystemview = RecalboxConf::Instance().GetThemeSystemView(themeName);
  if (main && CheckThemeOption(mSystemview, subSets, "systemview")) { RecalboxConf::Instance().SetThemeSystemView(themeName, mSystemview); needSave = true; }
  mGamelistview = RecalboxConf::Instance().GetThemeGamelistView(themeName);
  if (main && CheckThemeOption(mGamelistview, subSets, "gamelistview")) { RecalboxConf::Instance().SetThemeGamelistView(themeName, mGamelistview); needSave = true; }
  mGameClipView = RecalboxConf::Instance().GetThemeGameClipView(themeName);
  if (main && CheckThemeOption(mGameClipView, subSets, "gameclipview")) { RecalboxConf::Instance().SetThemeGameClipView(themeName, mGameClipView); needSave = true; }
  mRegion = RecalboxConf::Instance().GetThemeRegion(themeName);
  if (main && mRegion != "us" && mRegion != "eu" && mRegion != "jp") { mRegion="us"; RecalboxConf::Instance().SetThemeRegion(themeName, mRegion); needSave = true; }
  if (needSave) RecalboxConf::Instance().Save();

  pugi::xml_document doc;
	pugi::xml_parse_result res = doc.load_file(path.ToChars());
	if(!res)
		throw ThemeException("XML parsing error: \n    " + std::string(res.description()), mPaths);

	pugi::xml_node root = doc.child("theme");
	if(!root)
		throw ThemeException("Missing <theme> tag!", mPaths);

	// parse version
	mVersion = root.child("formatVersion").text().as_float(-404);
	if(mVersion == -404)
		throw ThemeException("<formatVersion> tag missing!\n   It's either out of date or you need to add <formatVersion>" + Strings::ToString(CURRENT_THEME_FORMAT_VERSION) + "</formatVersion> inside your <theme> tag.", mPaths);

	if(mVersion < MINIMUM_THEME_FORMAT_VERSION)
		throw ThemeException("Theme uses format version " + Strings::ToString(mVersion, 2) + ". Minimum supported version is " + Strings::ToString(MINIMUM_THEME_FORMAT_VERSION) + '.', mPaths);
	
	parseIncludes(root);
	parseViews(root);
	parseFeatures(root);
	mPaths.pop_back();
}


void ThemeData::parseIncludes(const pugi::xml_node& root)
{
  std::string errorString;
	for (pugi::xml_node node = root.child("include"); node != nullptr; node = node.next_sibling("include"))
	{
		if (parseSubset(node))
		{
			std::string str = resolveSystemVariable(mSystemThemeFolder, node.text().get(), mRandomPath);
			
			//workaround for an issue in parseincludes introduced by variable implementation
			if (str.find("//") == std::string::npos)
			{
				Path path = Path(str).ToAbsolute(mPaths.back().Directory());
				if(!ResourceManager::fileExists(path))
        {
          { LOG(LogWarning) << "[ThemeData] Included file \"" << str << "\" not found! (resolved to \"" << path.ToString() << "\")"; }
          continue;
        }

				errorString += "    from included file \"" + str + "\":\n    ";

				mPaths.push_back(path);

				pugi::xml_document includeDoc;
				pugi::xml_parse_result result = includeDoc.load_file(path.ToChars());
				if(!result)
					throw ThemeException(errorString + "Error parsing file: \n    " + result.description(), mPaths);

				pugi::xml_node newRoot = includeDoc.child("theme");
				if(!newRoot)
					throw ThemeException(errorString + "Missing <theme> tag!", mPaths);
				parseIncludes(newRoot);
				parseViews(newRoot);
				parseFeatures(newRoot);
			
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
			return true;
		}
		if (subsetAttr == "iconset" && nameAttr == mIconset)
		{	
			return true;
		}
		if (subsetAttr == "menu" && nameAttr == mMenu)
		{	
			return true;
		}
		if (subsetAttr == "systemview" && nameAttr == mSystemview)
		{	
			return true;
		}
		if (subsetAttr == "gamelistview" && nameAttr == mGamelistview)
		{	
			return true;
		}
        if (subsetAttr == "gameclipview" && nameAttr == mGameClipView)
        {
            return true;
        }
	}

	return parse;
}

void ThemeData::parseFeatures(const pugi::xml_node& root)
{
	for (pugi::xml_node node = root.child("feature"); node != nullptr; node = node.next_sibling("feature"))
	{
		if(!node.attribute("supported"))
			throw ThemeException("Feature missing \"supported\" attribute!", mPaths);

		const std::string supportedAttr = node.attribute("supported").as_string();

		if (std::find(SupportedFeatures().begin(), SupportedFeatures().end(), supportedAttr) != SupportedFeatures().end())
		{
			parseViews(node);
		}
	}
}

void ThemeData::parseViews(const pugi::xml_node& root)
{
	// parse views
	for (pugi::xml_node node = root.child("view"); node != nullptr; node = node.next_sibling("view"))
	{
		if(!node.attribute("name"))
			throw ThemeException("View missing \"name\" attribute!", mPaths);

		const char* delim = " \t\r\n,";
		const std::string nameAttr = node.attribute("name").as_string();
		size_t prevOff = nameAttr.find_first_not_of(delim, 0);
		size_t off = nameAttr.find_first_of(delim, prevOff);
		std::string viewKey;
		while(off != std::string::npos || prevOff != std::string::npos)
		{
			viewKey = nameAttr.substr(prevOff, off - prevOff);
			if (viewKey == "menu")
				SetThemeHasMenuView(true);
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
	for (pugi::xml_node node = root.first_child(); node != nullptr; node = node.next_sibling())
	{
		if(!node.attribute("name"))
			throw ThemeException("Element of type \"" + std::string(node.name()) + R"(" missing "name" attribute!)", mPaths);
		if (std::string(node.name()) == "helpsystem")
			SetThemeHasHelpSystem(true);

    const std::map< std::string, std::map<std::string, ThemeData::ElementProperty> >& elementMap = ElementMap();

		auto elemTypeIt = elementMap.find(node.name());
		if(elemTypeIt == elementMap.end())
			throw ThemeException("Unknown element of type \"" + std::string(node.name()) + "\"!", mPaths);
    const auto & subElementMap = elemTypeIt->second;

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
			if (elemKey == mRegion)
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
	element.SetRootData(root.name(), root.attribute("extra").as_bool(false));
	
	for (pugi::xml_node node = root.first_child(); node != nullptr; node = node.next_sibling())
	{
		auto typeIt = typeMap.find(node.name());
		if(typeIt == typeMap.end())
			throw ThemeException("Unknown property type \"" + std::string(node.name()) + "\" (for element of type " + root.name() + ").", mPaths);
		
    std::string str = Strings::Trim(resolveSystemVariable(mSystemThemeFolder, node.text().as_string(), mRandomPath));

		switch(typeIt->second)
		{
		case ElementProperty::NormalizedPair:
		{
      float x = 0, y = 0;
      if (Strings::ToFloat(str, 0, ' ', x))
      {
        size_t pos = str.find(' ');
        if (pos != std::string::npos)
          if (Strings::ToFloat(str, (int) pos + 1, 0, y))
          {
            element.AddVectorProperty(node.name(), x, y);
            break;
          }
      }
      throw ThemeException("invalid normalized pair (property \"" + std::string(node.name()) + "\", value \"" + str + "\")", mPaths);
		}
		case ElementProperty::String:
    {
      element.AddStringProperty(node.name(), str);
      break;
    }
		case ElementProperty::Path:
		{
			Path path = Path(str).ToAbsolute(mPaths.back().Directory());
			std::string variable = node.text().get();
			if(!ResourceManager::fileExists(path))
			{
				//too many warnings with region and system variable surcharge in themes
				if (!root.attribute("region") && variable.find("$system") == std::string::npos)
				{
					std::string ss = "  Warning " + ThemeException::AddFiles(mPaths); // "from theme yadda yadda, included file yadda yadda
					ss += std::string("could not find file \"") + node.text().get() + "\" ";
					if(node.text().get() != path.ToString())
						ss += "(which resolved to \"" + path.ToString() + "\") ";
          { LOG(LogWarning) << "[ThemeData] " << ss; }
				}
				break;
			}
			element.AddStringProperty(node.name(), path.ToString());
			break;
		}
		case ElementProperty::Color:
    {
      element.AddIntProperty(node.name(), (int) getHexColor(str.c_str()));
      break;
    }
		case ElementProperty::Float:
		{
			float floatVal = 0;
			if (!Strings::ToFloat(str, floatVal))
        throw ThemeException("invalid float value (property \"" + std::string(node.name()) + "\", value \"" + str + "\")", mPaths);
		  element.AddFloatProperty(node.name(), floatVal);
  		break;
		}
		case ElementProperty::Boolean:
		{
			// only look at first char
			char first = str[0];
			// 1*, t* (true), T* (True), y* (yes), Y* (YES)
			bool boolVal = (first == '1' || first == 't' || first == 'T' || first == 'y' || first == 'Y');
			element.AddBoolProperty(node.name(), boolVal);
  			break;
		}
		default:
			throw ThemeException("Unknown ElementPropertyType for \"" + std::string(root.attribute("name").as_string()) + "\", property " + node.name(), mPaths);
		}
	}
}


const ThemeElement* ThemeData::getElement(const std::string& view, const std::string& element, const std::string& expectedType) const
{
	auto viewIt = mViews.find(view);
	if(viewIt == mViews.end())
		return nullptr; // not found

	auto elemIt = viewIt->second.elements.find(element);
	if(elemIt == viewIt->second.elements.end()) return nullptr;

	if(elemIt->second.Type() != expectedType && !expectedType.empty())
	{
    { LOG(LogWarning) << "[ThemeData] Requested mismatched theme type for [" << view << "." << element << "] - expected \"" << expectedType << "\", got \"" << elemIt->second.Type() << "\""; }
		return nullptr;
	}

	return &elemIt->second;
}

/*const ThemeData& ThemeData::getDefault()
{
  static ThemeData sDefault;
  static bool sLoaded = false;

  if (!sLoaded)
  {
		const Path path = RootFolders::DataRootFolder / "system/.emulationstation/es_theme_default.xml";
		if (path.Exists())
		{
			try
			{
				std::string empty;
        sDefault.loadFile(empty, path);
			}
			catch(ThemeException& e)
			{
				{ LOG(LogError) << "[ThemeData] " << e.what(); }
			}
		}
		sLoaded = true;
	}

	return sDefault;
}*/

const ThemeData& ThemeData::getCurrent()
{
  if (IsThemeChanged())
  {
    delete sCurrent;
    sCurrent = nullptr;
  }

	if (sCurrent == nullptr)
	{
		Path path;
		const std::string& currentTheme = RecalboxConf::Instance().GetThemeFolder();

		static constexpr size_t pathCount = 3;
		Path paths[pathCount] =
		{
			RootFolders::TemplateRootFolder / "system/.emulationstation/themes/" / currentTheme,
			RootFolders::DataRootFolder     / "themes/" / currentTheme,
			RootFolders::DataRootFolder     / "system/.emulationstation/themes/" / currentTheme
		};

		sCurrent = new ThemeData();

		for (const auto& master : paths)
		{
			if(!master.IsDirectory()) continue;

			Path::PathList list = master.GetDirectoryContent();
			for(auto& sub : list)
			{
				if (sub.IsDirectory())
				{
					Path subTheme = sub / "theme.xml";
					if (subTheme.Exists())
					{
						try
						{
							std::string empty;
              sCurrent->loadFile(empty, subTheme);
							break;
						} catch(ThemeException& e)
						{
              { LOG(LogError) << "[ThemeData] " << e.what(); }
						}
					}
				
				}
			}
			Path masterTheme = master / "theme.xml";
			if (masterTheme.Exists())
      {
        try
        {
          std::string empty;
          sCurrent->loadFile(empty, masterTheme);
          break;
        } catch(ThemeException& e)
        {
          { LOG(LogError) << "[ThemeData] " << e.what(); }
          *sCurrent = ThemeData(); //reset to empty
        }
      }
		}

    ThemeData::SetThemeChanged(false);
    MenuThemeData::Reset();
  }

	return *sCurrent;
}

void ThemeData::SetThemeChanged(bool themeChanged){
    sThemeChanged = themeChanged;
}

bool ThemeData::IsThemeChanged(){
    return sThemeChanged;
}

std::string ThemeData::getGameClipView() const {
    return mGameClipView;
}

std::vector<Component*> ThemeData::makeExtras(const ThemeData& theme, const std::string& view, WindowManager& window)
{
	std::vector<Component*> comps;

	auto viewIt = theme.mViews.find(view);
	if(viewIt == theme.mViews.end())
		return comps;
	
	for (const auto & key : viewIt->second.orderedKeys)
	{
		const ThemeElement& elem = viewIt->second.elements.at(key);
		if(elem.Extra())
		{
			Component* comp = nullptr;
			const std::string& t = elem.Type();
			if(t == "image")
				comp = new ImageComponent(window);

			else if (t == "video")
				comp = new VideoComponent(window);

			else if(t == "text")
				comp = new TextComponent(window);

			if (comp != nullptr)
      {
        comp->setDefaultZIndex(10);
        comp->applyTheme(theme, view, key, ThemeProperties::All);
        comps.push_back(comp);
      }
		}
	}

	return comps;
}

std::map<std::string, ThemeSet> ThemeData::getThemeSets()
{
	std::map<std::string, ThemeSet> sets;

	static const size_t pathCount = 3;
	static Path paths[pathCount] =
	{
		RootFolders::TemplateRootFolder / "/system/.emulationstation/themes",
		RootFolders::DataRootFolder     / "/themes",
		RootFolders::DataRootFolder     / "/system/.emulationstation/themes"
	};

	for (const auto& path : paths)
	{
    Path::PathList list = path.GetDirectoryContent();
    for(auto& setPath : list)
		{
			if(setPath.IsDirectory())
			{
				ThemeSet set(setPath);
				sets[set.getName()] = set;
			}
		}
	}

	return sets;
}

std::map<std::string, std::string> ThemeData::getThemeSubSets(const std::string& theme)
{
	std::map<std::string, std::string> sets;
	std::deque<Path> dequepath;

	static const size_t pathCount = 3;
	Path paths[pathCount] =
	{
		RootFolders::TemplateRootFolder / "/system/.emulationstation/themes/" / theme,
		RootFolders::DataRootFolder     / "/themes/" / theme,
		RootFolders::DataRootFolder     / "/system/.emulationstation/themes/" / theme
	};

	for (const auto& path : paths)
	{
    Path::PathList list = path.GetDirectoryContent();
    for(auto& setPath : list)
    {
      if (setPath.IsDirectory())
			{
				Path themePath = setPath / "theme.xml";
				dequepath.push_back(themePath);
				pugi::xml_document doc;
				doc.load_file(themePath.ToChars());
				pugi::xml_node root = doc.child("theme");
				crawlIncludes(root, sets, dequepath);
				dequepath.pop_back();
			}
		}
		Path master = path / "theme.xml";
    if (master.Exists())
    {
      dequepath.push_back(master);
      pugi::xml_document doc;
      doc.load_file(master.ToChars());
      pugi::xml_node root = doc.child("theme");
      crawlIncludes(root, sets, dequepath);
      findRegion(doc, sets);
      dequepath.pop_back();
    }
	}

	return sets;
}

void ThemeData::crawlIncludes(const pugi::xml_node& root, std::map<std::string, std::string>& sets, std::deque<Path>& dequepath)
{
	for (pugi::xml_node node = root.child("include"); node != nullptr; node = node.next_sibling("include"))
	{
		sets[node.attribute("name").as_string()] = node.attribute("subset").as_string();
		
		Path relPath(node.text().get());
		Path path = relPath.ToAbsolute(dequepath.back().Directory());
		dequepath.push_back(path);
		pugi::xml_document includeDoc;
		/*pugi::xml_parse_result result =*/ includeDoc.load_file(path.ToChars());
		pugi::xml_node newRoot = includeDoc.child("theme");
		crawlIncludes(newRoot, sets, dequepath);
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

	if(subset == "gameclipview" && !sortedsets.empty() )
	    sortedsets[getNoTheme()] = getNoTheme();

    return sortedsets;
}


Path ThemeData::getThemeFromCurrentSet(const std::string& system)
{
	auto themeSets = ThemeData::getThemeSets();
	if(themeSets.empty())
	{
		// no theme sets available
		return Path::Empty;
	}

	auto set = themeSets.find(RecalboxConf::Instance().GetThemeFolder());
	if(set == themeSets.end())
	{
		// currently selected theme set is missing, so just pick the first available set
		set = themeSets.begin();
    RecalboxConf::Instance().SetThemeFolder(set->first);
	}

	return set->second.getThemePath(system);
}

std::string ThemeData::getTransition() const
{
	std::string result;
	const auto* elem = getElement("system", "systemcarousel", "carousel");
	if (elem != nullptr) {
		if (elem->HasProperty("defaultTransition")) {
			if (elem->AsString("defaultTransition") == "instant") {
				result = "instant";
				return result;
			}
			if (elem->AsString("defaultTransition") == "fade") {
				result = "fade";
				return result;
			}
			if (elem->AsString("defaultTransition") == "slide") {
				result = "slide";
				return result;
			}
		}
	}
	return result;
}

bool ThemeData::isFolderHandled() const
{
	const auto* elem = getElement("detailed", "md_folder_name", "text");
	return elem != nullptr && elem->HasProperty("pos");
}