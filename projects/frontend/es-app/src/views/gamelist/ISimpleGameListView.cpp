#include <Log.h>
#include <RecalboxConf.h>
#include <guis/GuiNetPlay.h>
#include <guis/GuiSettings.h>
#include "guis/GuiGamelistOptions.h"
#include "views/gamelist/ISimpleGameListView.h"
#include "ThemeData.h"
#include "SystemData.h"
#include "Window.h"
#include "views/ViewController.h"
#include "Sound.h"
#include "Settings.h"
#include "Gamelist.h"
#include "Locale.h"

ISimpleGameListView::ISimpleGameListView(Window* window, FolderData* root)
  : IGameListView(window, root),
    mHeaderText(window),
    mHeaderImage(window),
    mBackground(window),
    mThemeExtras(window)
{
	mFavoritesCount = getRoot()->getSystem()->getFavoritesCount();
	mFavoritesOnly = mFavoritesCount > 0 && Settings::getInstance()->getBool("FavoritesOnly");

	mHeaderText.setText("Logo Text");
	mHeaderText.setSize(mSize.x(), 0);
	mHeaderText.setPosition(0, 0);
	mHeaderText.setHorizontalAlignment(ALIGN_CENTER);
	mHeaderText.setDefaultZIndex(50);
	
	mHeaderImage.setResize(0, mSize.y() * 0.185f);
	mHeaderImage.setOrigin(0.5f, 0.0f);
	mHeaderImage.setPosition(mSize.x() / 2, 0);
	mHeaderImage.setDefaultZIndex(50);

	mBackground.setResize(mSize.x(), mSize.y());
	mBackground.setDefaultZIndex(0);

  mIsFavoriteSystem = getRoot()->getSystem() == SystemData::getFavoriteSystem();

	addChild(&mHeaderText);
	addChild(&mBackground);
}

void ISimpleGameListView::onThemeChanged(const std::shared_ptr<ThemeData>& theme) {
	using namespace ThemeFlags;
	mBackground.applyTheme(theme, getName(), "background", ALL);
	mHeaderImage.applyTheme(theme, getName(), "logo", ALL);
	mHeaderText.applyTheme(theme, getName(), "logoText", ALL);

	// Remove old theme extras
	for (auto extra : mThemeExtras.getmExtras()) {
		removeChild(extra);
	}
	mThemeExtras.getmExtras().clear();

	mThemeExtras.setExtras(ThemeData::makeExtras(theme, getName(), mWindow));
	mThemeExtras.sortExtrasByZIndex();

	// Add new theme extras

	for (auto extra : mThemeExtras.getmExtras()) {
		addChild(extra);
	}


	if (mHeaderImage.hasImage()) {
		removeChild(&mHeaderText);
		addChild(&mHeaderImage);
	} else {
		addChild(&mHeaderText);
		removeChild(&mHeaderImage);
	}
}

void ISimpleGameListView::onFileChanged(FileData* file, FileChangeType change)
{
	if (change == FileChangeType::Run)
	{
		updateInfoPanel();
		return ;
	}

	if ((change == FileChangeType::Removed) && (file == getEmptyListItem()))
		return;

	if (file->isGame())
	{
		SystemData* favoriteSystem = SystemData::getFavoriteSystem();
		bool isInFavorite = favoriteSystem->getRootFolder()->Contains(file, true);
		bool isFavorite = file->Metadata().Favorite();

		if (isInFavorite != isFavorite)
		{
			if (isInFavorite) favoriteSystem->getRootFolder()->removeChild(file);
			else favoriteSystem->getRootFolder()->addChild(file, false);
			ViewController::get()->setInvalidGamesList(SystemData::getFavoriteSystem());
			ViewController::get()->getSystemListView()->manageFavorite();
			mFavoritesCount = mFavoritesCount + (isFavorite ? 1 : -1);
			if (!mFavoritesCount) { mFavoritesOnly = false; }
			updateHelpPrompts();
		}
	}

	if (change == FileChangeType::Removed)
	{
		bool favorite = file->Metadata().Favorite();
		delete file;
		if (favorite)
		{
			ViewController::get()->setInvalidGamesList(SystemData::getFavoriteSystem());
			ViewController::get()->getSystemListView()->manageFavorite();
		}
	}

	int cursor = getCursorIndex();
	if (RecalboxConf::getInstance()->getBool(getRoot()->getSystem()->getName() + ".flatfolder"))
	{
		populateList(getRoot());
	}
	else
	{
		refreshList();
	}
	setCursorIndex(cursor);

	updateInfoPanel();
}

bool ISimpleGameListView::input(InputConfig* config, Input input) {
	bool hideSystemView = RecalboxConf::getInstance()->get("emulationstation.hidesystemview") == "1";

	if (input.value != 0)
	{

	  // RUN GAME or ENTER FOLDER
		if (config->isMappedTo("b", input))
		{
			FileData* cursor = getCursor();
			if (cursor->isGame())
			{
				//Sound::getFromTheme(getTheme(), getName(), "launch")->play();
				launch(cursor);
			}
			else if (cursor->isFolder())
			{
				FolderData* folder = static_cast<FolderData*>(cursor);
				if (folder->hasChildren())
				{
					mCursorStack.push(folder);
					populateList(folder);
					setCursorIndex(0);
				}
			}
      RecalboxSystem::getInstance()->NotifyGame(*getCursor(), false, false);
      return true;
		}

		// BACK to PARENT FOLDER or PARENT SYSTEM
		if(config->isMappedTo("a", input))
		{
			if (mCursorStack.size())
			{
				FolderData* selected = mCursorStack.top();

				// remove current folder from stack
				mCursorStack.pop();

				FolderData* cursor = mCursorStack.size() ? mCursorStack.top() : getRoot();
				populateList(cursor);

				setCursor(selected);
				//Sound::getFromTheme(getTheme(), getName(), "back")->play();
        RecalboxSystem::getInstance()->NotifyGame(*getCursor(), false, false);
			}
			else if (!hideSystemView)
			{
				onFocusLost();
				ViewController::get()->goToSystemView(getRoot()->getSystem());
			}
			return true;
		}

		// TOGGLE FAVORITES
		if (config->isMappedTo("y", input)) {
			FileData* cursor = getCursor();
			if (cursor->isGame() && cursor->getSystem()->getHasFavoritesInTheme()) {
        MetadataDescriptor& md = cursor->Metadata();
        SystemData *favoriteSystem = SystemData::getFavoriteSystem();

        md.SetFavorite(!md.Favorite());

        if (favoriteSystem != nullptr) {
          if (md.Favorite()) {
            favoriteSystem->getRootFolder()->addChild(cursor, false);
          } else {
            favoriteSystem->getRootFolder()->removeChild(cursor);
          }

          if (mIsFavoriteSystem) {
            ViewController::get()->setInvalidGamesList(cursor->getSystem());
          } else {
            ViewController::get()->setInvalidGamesList(favoriteSystem);
          }
          ViewController::get()->getSystemListView()->manageFavorite();
        }

        // Reload to refresh the favorite icon
        int cursorPlace = getCursorIndex();
        refreshList();
        setCursorIndex(cursorPlace);

        mFavoritesCount = mFavoritesCount + (md.Favorite() ? 1 : -1);
        if (!mFavoritesCount) { mFavoritesOnly = false; }
        updateHelpPrompts();
			}
      RecalboxSystem::getInstance()->NotifyGame(*getCursor(), false, false);
      return true;
		}

		// MOVE to NEXT GAMELIST
		if (config->isMappedTo("right", input)) {
			if (Settings::getInstance()->getBool("QuickSystemSelect") && !hideSystemView) {
				onFocusLost();
				ViewController::get()->goToNextGameList();
        RecalboxSystem::getInstance()->NotifyGame(*getCursor(), false, false);
        return true;
			}
		}

    // MOVE to PREVIOUS GAMELIST
		if (config->isMappedTo("left", input)) {
			if (Settings::getInstance()->getBool("QuickSystemSelect") && !hideSystemView) {
				onFocusLost();
				ViewController::get()->goToPrevGameList();
        RecalboxSystem::getInstance()->NotifyGame(*getCursor(), false, false);
				return true;
			}
		}


		// JUMP TO NEXT LETTER
		if(config->isMappedTo("PageUp", input))
		{
			jumpToNextLetter(mSystem->getSortId() == 1 ? 1 : -1);
			return true;
		}

		// JUMP TO PREVIOUS LETTER
        if(config->isMappedTo("PageDown", input))
        {
            jumpToNextLetter(mSystem->getSortId() == 1 ? -1 : 1);
            return true;
        }

		// JUMP TO -10
		if(config->isMappedTo("L2", input))
		{
			auto index = getCursorIndex();
			if (index > 0)
				setCursorIndex(index > 10 ? index - 10 : 0);
			else
				setCursorIndex(getCursorIndexMax());
			return true;
		}

		// JUMP TO +10
		if(config->isMappedTo("R2", input))
		{
			auto index = getCursorIndex();
			auto max = getCursorIndexMax();
			if (index == max)
				setCursorIndex(0);
			else
				setCursorIndex(index > max - 10 ? max : index + 10);
			return true;
		}

		// NETPLAY
		if ((config->isMappedTo("x", input)) && (RecalboxConf::getInstance()->get("global.netplay") == "1")
		          && (RecalboxConf::getInstance()->isInList("global.netplay.systems", getCursor()->getSystem()->getName())))
		{
			FileData* cursor = getCursor();
			if(cursor->isGame())
			{
				Vector3f target(Renderer::getScreenWidth() / 2.0f, Renderer::getScreenHeight() / 2.0f, 0);
				ViewController::get()->launch(cursor, target, "host");
			}
		}


		if(config->isMappedTo("start", input) && input.value)
		{
			mWindow->pushGui(new GuiGamelistOptions(mWindow, getRoot()->getSystem()));
			return true;
		}


		if(config->isMappedTo("select", input) && input.value && !mIsFavoriteSystem)
		{
			if (mFavoritesCount) {
				mFavoritesOnly = !mFavoritesOnly;
				refreshList();
				updateInfoPanel();
				updateHelpPrompts();
			}
			return true;
		}
	}

	bool result = IGameListView::input(config, input);

  // TODO: Guess there is a better way to detect a game change
  if (config->isMappedTo("down", input) ||
      config->isMappedTo("up", input) ||
      config->isMappedTo("pagedown", input) ||
	    config->isMappedTo("pageup", input) ||
      config->isMappedTo("l2", input) ||
	    config->isMappedTo("r2", input) )
      RecalboxSystem::getInstance()->NotifyGame(*getCursor(), false, false);

	return result;
}

std::vector<HelpPrompt> ISimpleGameListView::getHelpPrompts() {
	bool hideSystemView = RecalboxConf::getInstance()->get("emulationstation.hidesystemview") == "1";
	std::vector<HelpPrompt> prompts;

	prompts.push_back(HelpPrompt("b", _("LAUNCH")));

	if ((RecalboxConf::getInstance()->get("global.netplay") == "1") && (RecalboxConf::getInstance()->isInList("global.netplay.systems", getCursor()->getSystem()->getName())))
		prompts.push_back(HelpPrompt("x", _("NETPLAY")));

	if (mIsFavoriteSystem)
		prompts.push_back(HelpPrompt("y", _("Remove from favorite")));
	else
		prompts.push_back(HelpPrompt("y", _("Favorite")));

	if (!hideSystemView)
		prompts.push_back(HelpPrompt("a", _("BACK")));

	prompts.push_back(HelpPrompt("up/down", _("CHOOSE")));

	if (Settings::getInstance()->getBool("QuickSystemSelect") && !hideSystemView)
		prompts.push_back(HelpPrompt("left/right", _("SYSTEM")));

	if (!mIsFavoriteSystem)
	{
		prompts.push_back(HelpPrompt("start", _("OPTIONS")));

		if (mFavoritesCount)
		{
			if (mFavoritesOnly)
				prompts.push_back(HelpPrompt("select", _("ALL GAMES")));
			else
				prompts.push_back(HelpPrompt("select", _("FAVORITES ONLY")));
		}
	}

	return prompts;
}

std::vector<std::string> ISimpleGameListView::getAvailableLetters()
{
	// TODO: Algorithm!!! Use 128 array bitflag - Use better returning type - kill all vectors
	// TODO: Be consistent! Use gamelist items, not FileData
	std::vector<std::string> letters;
	std::set<std::string> setOfLetters;
	FileData::List files = getFileDataList();
	for (auto file : files) {
		if (file->getType() == ItemType::Game)
		{
			setOfLetters.insert(std::string(1, toupper(file->getName()[0])));
		}
	}

	for (const auto letter : setOfLetters)
	{
		letters.push_back(letter);
	}

	std::sort(letters.begin(), letters.end());
	return letters;
}


void ISimpleGameListView::jumpToNextLetter(int increment)
{
	std::string letter = std::string(1, toupper(getCursor()->getName()[0]));
	auto letters = getAvailableLetters();
	unsigned long size = letters.size();
	unsigned long pos = std::find(letters.begin(), letters.end(), letter) - letters.begin();

	if (pos < size) {
		pos = (pos + increment + size) % size;
		jumpToLetter(letters.at(pos)[0]);
	}
}

void ISimpleGameListView::jumpToLetter(char letter)
{
	// Jump to letter requires an alpha sort
	if ( mSystem->getSortId() > 1) {
		// apply sort
		mSystem->setSortId(0);
		// notify that the root folder has to be sorted
		onFileChanged(mSystem->getRootFolder(), FileChangeType::Sorted);
	}

	FileData::List files = getFileDataList();

	unsigned long min, max;
	unsigned long mid = 0;

	bool asc = mSystem->getSortId() == 0;

	// look for first game position
	for (min = 0; (min < files.size() - 1) && (files.at(min)->getType() != ItemType::Game) ; min++) ;

	// look for last game position
	for (max = files.size() - 1; max && (files.at(max)->getType() != ItemType::Game) ; max--) ;

	while(max >= min) {
		mid = ((max - min) / 2) + min;

		// game somehow has no first character to check
		if (files.at(mid)->getName().empty()) {
			continue;
		}

		char checkLetter = (char) toupper(files.at(mid)->getName()[0]);

		if (asc) {
			if (checkLetter < letter) {
				min = mid + 1;
			} else if (checkLetter > letter || (mid > 0 && (letter == toupper(files.at(mid - 1)->getName()[0])))) {
				max = mid - 1;
			} else {
				break; //exact match found
			}
		} else {
			if (checkLetter > letter) {
				min = mid + 1;
			} else if (checkLetter < letter || (mid > 0 && (letter == toupper(files.at(mid - 1)->getName()[0])))) {
				max = mid - 1;
			} else {
				break; //exact match found
			}
		}
	}

	setCursor(files.at(mid));
}