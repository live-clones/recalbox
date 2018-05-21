#include <RecalboxConf.h>
#include "components/TextListComponent.h"
#include "components/IList.h"
#include "views/gamelist/BasicGameListView.h"
#include "views/ViewController.h"
#include "Renderer.h"
#include "Window.h"
#include "ThemeData.h"
#include "SystemData.h"
#include "FileSorts.h"
#include "Settings.h"
#include "Locale.h"
#include <boost/assign.hpp>

static const std::map<std::string, const char*> favorites_icons_map = boost::assign::map_list_of
        ("snes", "\uF25e ")
        ("3do", "\uF28a")
        ("x68000", "\uF28b")
        ("amiga600", "\uF244")
        ("amiga1200", "\uF245")
        ("nds", "\uF267")
        ("c64", "\uF24c ")
        ("nes", "\uF25c ")
        ("n64", "\uF260 ")
        ("gba", "\uF266 ")
        ("gbc", "\uF265 ")
        ("gb", "\uF264 ")
        ("fds", "\uF25d ")
        ("virtualboy", "\uF25f ")
        ("gw", "\uF278 ")
        ("dreamcast", "\uF26e ")
        ("megadrive", "\uF26b ")
        ("segacd", "\uF26d ")
        ("sega32x", "\uF26c ")
        ("mastersystem", "\uF26a ")
        ("gamegear", "\uF26f ")
        ("sg1000", "\uF269 ")
        ("psp", "\uF274 ")
        ("psx", "\uF275 ")
        ("pcengine", "\uF271 ")
        ("pcenginecd", "\uF273 ")
        ("supergrafx", "\uF272 ")
        ("scummvm", "\uF27a ")
        ("dos", "\uF24a ")
        ("fba", "\uF252 ")
        ("fba_libretro", "\uF253 ")
        ("mame", "\uF255 ")
        ("neogeo", "\uF257 ")
        ("colecovision", "\uF23f ")
        ("atari2600", "\uF23c ")
        ("atari7800", "\uF23e ")
        ("lynx", "\uF270 ")
        ("ngp", "\uF258 ")
        ("ngpc", "\uF259 ")
        ("wswan", "\uF25a ")
        ("wswanc", "\uF25b ")
        ("prboom", "\uF277 ")
        ("vectrex", "\uF240 ")
        ("lutro", "\uF27d ")
        ("cavestory", "\uF276 ")
        ("atarist", "\uF248 ")
        ("amstradcpc", "\uF246 ")
        ("msx", "\uF24d ")
        ("msx1", "\uF24e ")
        ("msx2", "\uF24f ")
        ("odyssey2", "\uF241 ")
        ("zx81", "\uF250 ")
        ("zxspectrum", "\uF251 ")
        ("moonlight", "\uF27e ")
        ("apple2", "\uF247 ")
        ("gamecube", "\uF262 ")
        ("wii", "\uF263 ")
        ("imageviewer", "\uF27b ");

BasicGameListView::BasicGameListView(Window* window, FileData* root)
	: ISimpleGameListView(window, root), mList(window), listingOffset(0) {
	mList.setSize(mSize.x(), mSize.y() * 0.8f);
	mList.setPosition(0, mSize.y() * 0.2f);
	mList.setDefaultZIndex(20);
	addChild(&mList);

	populateList(root);
}

void BasicGameListView::onThemeChanged(const std::shared_ptr<ThemeData>& theme) {
	ISimpleGameListView::onThemeChanged(theme);
	using namespace ThemeFlags;
	mList.applyTheme(theme, getName(), "gamelist", ALL);
	sortChildren();
}

void BasicGameListView::onFileChanged(FileData* file, FileChangeType change) {
	ISimpleGameListView::onFileChanged(file, change);

	if(change == FILE_METADATA_CHANGED) {
		// might switch to a detailed view
		ViewController::get()->reloadGameListView(this);
		return;
	}
}

void BasicGameListView::populateList(const FileData* folder) {
    mPopulatedFolder = folder;
    auto files = folder->getChildren();

	mList.clear();
	mHeaderText.setText(mSystem->getFullName());

	bool favoritesOnly = false;

	// find at least one favorite, else, show all items
	if (Settings::getInstance()->getBool("FavoritesOnly") && !mSystem->isFavorite()) {
		for (auto it = files.begin(); it != files.end(); it++) {
			if ((*it)->getType() == GAME) {
				if ((*it)->metadata.get("favorite") == "true") {
					favoritesOnly = true;
					break;
				}
			}
		}
	}

    bool showHidden = Settings::getInstance()->getBool("ShowHidden");
    const FileData::SortType& sortType = mSystem->getSortType();

	// Do not show double names in favorite system.
	if (!mSystem->isFavorite() && !favoritesOnly) {
		for (auto it = files.begin(); it != files.end(); it++) {
            bool isHidden = (*it)->metadata.get("hidden") == "true";
			if (showHidden || !isHidden) {
				addItem(*it);
			}
		}

		mList.sortByObject(*sortType.comparisonFunction, sortType.ascending);
	}

    addFavorites(files, sortType);

    if (mSystem->isFavorite() || favoritesOnly) {
        listingOffset = 0;
    }
}

void BasicGameListView::refreshList() {
    populateList(mPopulatedFolder);
}

void BasicGameListView::addFavorites(const std::vector<FileData*>& files, const FileData::SortType& sortType) {
    std::vector<FileData*> favorites;
    getFavorites(files, favorites);

    std::sort(favorites.begin(), favorites.end(), *sortType.comparisonFunction);

    // Reverse to unshift
    if (sortType.ascending) {
        std::reverse(favorites.begin(), favorites.end());
    }

    listingOffset = favorites.size();

    for (auto it = favorites.begin(); it != favorites.end(); it++) {
        addItem(*it, true);
    }
}

void BasicGameListView::getFavorites(const std::vector<FileData*>& files, std::vector<FileData*>& favorites) {
    bool showHidden = Settings::getInstance()->getBool("ShowHidden");

    for (auto it = files.begin(); it != files.end(); it++) {
        bool isGame = (*it)->getType() == GAME;
        bool isFavorite = isGame && ((*it)->metadata.get("favorite") == "true");
        bool isHidden = (*it)->metadata.get("hidden") == "true";

        if (isFavorite && (showHidden || !isHidden)) {
            favorites.push_back(*it);
        } else if (!isGame) {
            getFavorites((*it)->getChildren(), favorites);
        }
    }
}

std::vector<FileData*> BasicGameListView::getFileDataList() {
    std::vector<FileData*> objects = mList.getObjects();
    std::vector<FileData*> slice;
    for (auto it = objects.begin() + listingOffset; it != objects.end(); it++) {
        slice.push_back(*it);
    }
    return slice;
}

void BasicGameListView::addItem(FileData* file, bool toTheBeginning) {
	std::string name = file->getName();
	bool isGame = file->getType() == GAME;
	bool isFavorite = isGame && (file->metadata.get("favorite") == "true");
	bool isHidden = file->metadata.get("hidden") == "true";

	if (!isGame) {
		std::vector<FileData *> children = file->getChildren();
		if (RecalboxConf::getInstance()->getBool(getRoot()->getSystem()->getName() + ".flatfolder")) {
			for (auto it = children.begin(); it != children.end(); it++) {
				addItem(*it);
			}
			return ;
		} else if (file->isSingleGameFolder()) {
            addItem(children.at(0));
            return ;
        }
	}
	if (isHidden) {
		name = "\uF070 " + name;
	}
	if (isFavorite) {
		if ((favorites_icons_map.find(file->getSystem()->getName())) != favorites_icons_map.end()) {
			name = (favorites_icons_map.find(file->getSystem()->getName())->second) + name;
		} else {
			name = "\uF006 " + name;
		}
	}
	mList.add(name, file, !isGame, toTheBeginning);
}

FileData* BasicGameListView::getCursor() {
	return mList.getSelected();
}

void BasicGameListView::setCursorIndex(int index){
	mList.setCursorIndex(index);
}

int BasicGameListView::getCursorIndex(){
	return mList.getCursorIndex();
}

void BasicGameListView::setCursor(FileData* cursor) {
	if(!mList.setCursor(cursor, listingOffset)) {
		populateList(mRoot);
		mList.setCursor(cursor);

		// update our cursor stack in case our cursor just got set to some folder we weren't in before
		if(mCursorStack.empty() || mCursorStack.top() != cursor->getParent()) {
			std::stack<FileData*> tmp;
			FileData* ptr = cursor->getParent();
			while(ptr && ptr != mRoot) {
				tmp.push(ptr);
				ptr = ptr->getParent();
			}
			
			// flip the stack and put it in mCursorStack
			mCursorStack = std::stack<FileData*>();
			while(!tmp.empty()) {
				mCursorStack.push(tmp.top());
				tmp.pop();
			}
		}
	}
}

void BasicGameListView::launch(FileData* game) {
	ViewController::get()->launch(game);
}

std::vector<HelpPrompt> BasicGameListView::getHelpPrompts() {
	std::vector<HelpPrompt> prompts;
	bool hideSystemView = RecalboxConf::getInstance()->get("emulationstation.hidesystemview") == "1";
	if(Settings::getInstance()->getBool("QuickSystemSelect") && !hideSystemView)
	  prompts.push_back(HelpPrompt("left/right", _("SYSTEM")));
	prompts.push_back(HelpPrompt("up/down", _("CHOOSE")));
	prompts.push_back(HelpPrompt("b", _("LAUNCH")));
	prompts.push_back(HelpPrompt("x", _("NETPLAY")));
	if(!hideSystemView)
	  prompts.push_back(HelpPrompt("a", _("BACK")));
	if(getRoot()->getSystem() != SystemData::getFavoriteSystem()) {
	  prompts.push_back(HelpPrompt("y", _("Favorite")));
	  prompts.push_back(HelpPrompt("select", _("OPTIONS")));
	}
	return prompts;
}
