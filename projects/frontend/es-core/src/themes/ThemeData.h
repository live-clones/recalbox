#pragma once

#include <map>
#include <deque>
#include <random>
#include <string>
#include <utils/os/fs/Path.h>
#include <RecalboxConf.h>
#include "pugixml/pugixml.hpp"
#include "ThemeElement.h"
#include "SystemManager.h"

template<typename T> class TextListComponent;

class Component;
class Sound;
class ImageComponent;
class NinePatchComponent;
class TextComponent;
class WindowManager;

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

    static ThemeData* sCurrent;
    static bool sThemeChanged;
    static bool sThemeHasMenuView;
    static bool sThemeHasHelpSystem;

  public:

    ThemeData();

    static bool ThemeHasMenuView() { return sThemeHasMenuView; }
    static bool ThemeHasHelpSystem() { return sThemeHasHelpSystem; }

    static void SetThemeHasMenuView(bool on) { sThemeHasMenuView = on; }
    static void SetThemeHasHelpSystem(bool on) { sThemeHasHelpSystem = on; }

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

    static std::vector<Component*> makeExtras(const ThemeData& theme, const std::string& view, WindowManager& window);

    //static const ThemeData& getDefault();
    static const ThemeData& getCurrent();
    static void SetThemeChanged(bool themeChanged);
    static bool IsThemeChanged();
    std::string getGameClipView() const;
    static const char *getNoTheme() { return "0 - DEFAULT"; }

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
	std::string mRegion;
	std::string mGameClipView;
	std::string mSystemThemeFolder;
	std::string mRandomPath;
	static constexpr const char* sRandomMethod = "$random(";


    void parseFeatures(const pugi::xml_node& themeRoot);
    void parseIncludes(const pugi::xml_node& themeRoot);
    void parseViews(const pugi::xml_node& themeRoot);
    void parseView(const pugi::xml_node& viewNode, ThemeView& view);
    void parseElement(const pugi::xml_node& elementNode, const std::map<std::string, ElementProperty>& typeMap, ThemeElement& element);
    bool parseRegion(const pugi::xml_node& root);
    bool parseSubset(const pugi::xml_node& node);
    static void crawlIncludes(const pugi::xml_node& root, std::map<std::string, std::string>& sets, std::deque<Path>& dequepath);
    static void findRegion(const pugi::xml_document& doc, std::map<std::string, std::string>& sets);

    static bool CheckThemeOption(std::string& selected, const std::map<std::string, std::string>& subsets, const std::string& subset);
    static std::string resolveSystemVariable(const std::string& systemThemeFolder, const std::string& path, std::string& randomPath)
    {
      std::string lccc = Strings::ToLowerASCII(RecalboxConf::Instance().GetSystemLanguage());
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

       // Handle system+x and system-x variables (up to +10/-10)
      const std::vector<SystemData*> systems = mSystemManager.GetAllSystemList();
      int size = systems.size();
      int index = 0;

      for(int i = 0; i < size; ++i){
          if (systems[i]->ThemeFolder() == systemThemeFolder)
          {
            index = i;
          }
      }
          
      for(int i = 0; i < 10; ++i)
      {
        int positiveOffset = index + i;
        while( positiveOffset > size - 1) positiveOffset -= size;
        Strings::ReplaceAllIn(result, "$system+"+i, systems[positiveOffset]->ThemeFolder());
        
        int negativeOffset = index - i;
        while( negativeOffset < 0) negativeOffset += size;
        Strings::ReplaceAllIn(result, "$system-"+i, systems[negativeOffset]->ThemeFolder());
      }

      return PickRandomPath(result, randomPath);;
    }

    static std::string PickRandomPath(std::string value, std::string& randomPath)
    {

      if(!Strings::Contains(value, sRandomMethod))
        return value;

      std::string args = Strings::Extract(value, sRandomMethod, ")", 8, 1);

      if(randomPath.empty())
      {
        std::vector<std::string> paths = Strings::Split(args, ',');
        std::random_device rd;
        std::default_random_engine engine(rd());
        const int max = paths.size();
        std::uniform_int_distribution<int> distrib{0, max-1};
        randomPath = paths[distrib(engine)];
      }

      return Strings::Replace(value, sRandomMethod + args + ")", randomPath);
    }

    std::map<std::string, ThemeView> mViews;
};
