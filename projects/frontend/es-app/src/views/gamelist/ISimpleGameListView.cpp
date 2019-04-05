#include <Log.h>
#include <RecalboxConf.h>
#include <guis/GuiNetPlay.h>
#include <guis/GuiSettings.h>
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
    mThemeExtras(window),
    mFavoriteChange(false)
{
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

	FileData* cursor = getCursor();
	if (RecalboxConf::getInstance()->getBool(getRoot()->getSystem()->getName() + ".flatfolder"))
	{
		populateList(getRoot());
	}
	else
	{
		refreshList();
	}

	setCursor(cursor);

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
		}
	}

	updateInfoPanel();
}

bool ISimpleGameListView::input(InputConfig* config, Input input) {
	bool hideSystemView = RecalboxConf::getInstance()->get("emulationstation.hidesystemview") == "1";

	if (input.value != 0)
	{
		if (config->isMappedTo("b", input))
		{
			FileData* cursor = getCursor();
			if (cursor->isGame())
			{
				//Sound::getFromTheme(getTheme(), getName(), "launch")->play();
				launch(cursor);
			}
			else
			{
				// it's a folder
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

				if (mFavoriteChange)
				{
					ViewController::get()->setInvalidGamesList(getRoot()->getSystem());
					mFavoriteChange = false;
				}

				ViewController::get()->goToSystemView(getRoot()->getSystem());
			}
			return true;
		}

		if (config->isMappedTo("y", input)) {
			FileData* cursor = getCursor();
			if (!ViewController::get()->getState().getSystem()->isFavorite() && cursor->getSystem()->getHasFavorites()) {
				if (cursor->isGame()) {
					mFavoriteChange = true;
          MetadataDescriptor& md = cursor->Metadata();
          bool value = md.Favorite();
					bool removeFavorite = false;
					SystemData *favoriteSystem = SystemData::getFavoriteSystem();

					if (value) {
                        md.SetFavorite(false);
                        if (favoriteSystem != nullptr) {
                            favoriteSystem->getRootFolder()->removeChild(cursor);
                        }
                        removeFavorite = true;
					} else {
                        md.SetFavorite(true);
                        if (favoriteSystem != nullptr) {
                            favoriteSystem->getRootFolder()->addChild(cursor, false);
                        }
					}

					if (favoriteSystem != nullptr) {
					  ViewController::get()->setInvalidGamesList(favoriteSystem);
					  ViewController::get()->getSystemListView()->manageFavorite();
					}

					// Reload to refresh the favorite icon
					int cursorPlace = getCursorIndex();
                    refreshList();
					setCursorIndex(std::max(0, cursorPlace + (removeFavorite ? -1 : 1)));
				}
			}
      RecalboxSystem::getInstance()->NotifyGame(*getCursor(), false, false);
      return true;
		}

		if (config->isMappedTo("right", input)) {
			if (Settings::getInstance()->getBool("QuickSystemSelect") && !hideSystemView) {
				onFocusLost();
				if (mFavoriteChange) {
					ViewController::get()->setInvalidGamesList(getCursor()->getSystem());
					mFavoriteChange = false;
				}
				ViewController::get()->goToNextGameList();
        RecalboxSystem::getInstance()->NotifyGame(*getCursor(), false, false);
        return true;
			}
		}

		if (config->isMappedTo("left", input)) {
			if (Settings::getInstance()->getBool("QuickSystemSelect") && !hideSystemView) {
				onFocusLost();
				if (mFavoriteChange) {
					ViewController::get()->setInvalidGamesList(getCursor()->getSystem());
					mFavoriteChange = false;
				}
				ViewController::get()->goToPrevGameList();
        RecalboxSystem::getInstance()->NotifyGame(*getCursor(), false, false);
				return true;
			}
		}else if ((config->isMappedTo("x", input)) && (RecalboxConf::getInstance()->get("global.netplay") == "1")
		          && (RecalboxConf::getInstance()->isInList("global.netplay.systems", getCursor()->getSystem()->getName())))
		{
			FileData* cursor = getCursor();
			if(cursor->isGame())
			{
				Eigen::Vector3f target(Renderer::getScreenWidth() / 2.0f, Renderer::getScreenHeight() / 2.0f, 0);
				ViewController::get()->launch(cursor, target, "host");
			}
		}
	}

	bool result = IGameListView::input(config, input);

  // TODO: Guess there is a better way to detect a game change
  if (config->isMappedTo("down", input) ||
      config->isMappedTo("up", input) ||
      config->isMappedTo("pagedown", input) ||
      config->isMappedTo("pageup", input) )
    RecalboxSystem::getInstance()->NotifyGame(*getCursor(), false, false);

	return result;
}
