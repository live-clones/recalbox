#pragma once

#include <map>
#include <deque>
#include <string>
#include <utils/os/fs/Path.h>
#include <RecalboxConf.h>
#include "pugixml/pugixml.hpp"
#include "ThemeElement.h"

template<typename T> class TextListComponent;

class Component;
class Sound;
class ImageComponent;
class NinePatchComponent;
class TextComponent;
class Window;

class ThemeSet
{
  private:
    Path mPath;

  public:
    ThemeSet() = default;
    explicit ThemeSet(const Path& path) : mPath(path) {}
    inline std::string getName() const { return mPath.FilenameWithoutExtension(); }
    inline Path getThemePath(const std::string& system) const { return mPath / system / "theme.xml"; }
};

class ThemeData
{
  private:
    class ThemeView
    {
      public:
        std::map<std::string, ThemeElement> elements;
        std::vector<std::string> orderedKeys;
    };

  public:

    ThemeData();

	// throws ThemeException
	void loadFile(const std::string& systemThemeFolder, const Path& path);

    enum class ElementProperty
    {
      NormalizedPair,
      Path,
      String,
      Color,
      Float,
      Boolean
    };

    // If expectedType is an empty string, will do no type checking.
    const ThemeElement* getElement(const std::string& view, const std::string& element, const std::string& expectedType) const;

    static std::vector<Component*> makeExtras(const ThemeData& theme, const std::string& view, Window& window);

    static const ThemeData& getDefault();
    static const ThemeData& getCurrent();

	static std::map<std::string, ThemeSet> getThemeSets();
	static std::map<std::string, std::string> getThemeSubSets(const std::string& theme);
	static std::map<std::string, std::string> sortThemeSubSets(const std::map<std::string, std::string>& subsetmap, const std::string& subset);
	static Path getThemeFromCurrentSet(const std::string& system);
	std::string getTransition() const;

    bool getHasFavoritesInTheme() const
    { return (mVersion >= CURRENT_THEME_FORMAT_VERSION); }
    bool isFolderHandled() const;

    static constexpr int MINIMUM_THEME_FORMAT_VERSION = 3;
    static constexpr int CURRENT_THEME_FORMAT_VERSION = 4;

  private:
    static std::map<std::string, std::map<std::string, ElementProperty>>& ElementMap();
    static std::vector<std::string>& SupportedFeatures();
    static std::vector<std::string>& SupportedViews();

	std::deque<Path> mPaths;
	float mVersion;
	std::string mColorset;
	std::string mIconset;
	std::string mMenu;
	std::string mSystemview;
	std::string mGamelistview;
	std::string mSystemThemeFolder;

    void parseFeatures(const pugi::xml_node& themeRoot);
    void parseIncludes(const pugi::xml_node& themeRoot);
    void parseViews(const pugi::xml_node& themeRoot);
    void parseView(const pugi::xml_node& viewNode, ThemeView& view);
    void parseElement(const pugi::xml_node& elementNode, const std::map<std::string, ElementProperty>& typeMap, ThemeElement& element);
    static bool parseRegion(const pugi::xml_node& root);
    bool parseSubset(const pugi::xml_node& node);
    static void crawlIncludes(const pugi::xml_node& root, std::map<std::string, std::string>& sets, std::deque<Path>& dequepath);
    static void findRegion(const pugi::xml_document& doc, std::map<std::string, std::string>& sets);

    static std::string resolveSystemVariable(const std::string& systemThemeFolder, const std::string& path)
    {
      std::string lccc = Strings::ToLowerASCII(RecalboxConf::Instance().AsString("system.language"));
      std::string lc = "en";
      std::string cc = "us";
      if (lccc.size() >= 5)
      {
        size_t pos = lccc.find('_');
        if (pos >=2 && pos < lccc.size() - 1)
        {
          lc = lccc.substr(0, pos);
          cc = lccc.substr(pos + 1);
        }
      }

      std::string result = path;
      Strings::ReplaceAllIn(result, "$system", systemThemeFolder);
      Strings::ReplaceAllIn(result, "$language", lc);
      Strings::ReplaceAllIn(result, "$country", cc);

      return result;
    }

    std::map<std::string, ThemeView> mViews;
};
