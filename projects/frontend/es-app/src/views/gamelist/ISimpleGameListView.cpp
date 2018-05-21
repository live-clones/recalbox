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

ISimpleGameListView::ISimpleGameListView(Window* window, FileData* root) : IGameListView(window, root),
mHeaderText(window), mHeaderImage(window), mBackground(window), mThemeExtras(window), mFavoriteChange(false) {
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

void ISimpleGameListView::onFileChanged(FileData* file, FileChangeType change) {
	if (change == FileChangeType::FILE_RUN) {
		updateInfoPanel();
		return ;
	}

    if (change == FileChangeType::FILE_REMOVED) {
        bool favorite = file->metadata.get("favorite") == "true";
        delete file;
        if (favorite) {
            ViewController::get()->setInvalidGamesList(SystemData::getFavoriteSystem());
            ViewController::get()->getSystemListView()->manageFavorite();
        }
    }

	FileData* cursor = getCursor();

	if (RecalboxConf::getInstance()->getBool(getRoot()->getSystem()->getName() + ".flatfolder")) {
		populateList(getRoot());
	} else {
		refreshList();
	}

	setCursor(cursor);


	/* Favorite */
	if (file->getType() == GAME) {
		SystemData * favoriteSystem = SystemData::getFavoriteSystem();
		bool isFavorite = file->metadata.get("favorite") == "true";
		bool foundInFavorite = false;
		/* Removing favorite case : */
		for (auto gameInFavorite = favoriteSystem->getRootFolder()->getChildren().begin();
			gameInFavorite != favoriteSystem->getRootFolder()->getChildren().end();
			gameInFavorite ++){
				if ((*gameInFavorite) == file) {
					if (!isFavorite) {
						favoriteSystem->getRootFolder()->removeAlreadyExistingChild(file);
						ViewController::get()->setInvalidGamesList(SystemData::getFavoriteSystem());
						ViewController::get()->getSystemListView()->manageFavorite();
					}
					foundInFavorite = true;
					break;
				}
		}
		/* Adding favorite case : */
		if (!foundInFavorite && isFavorite) {
			favoriteSystem->getRootFolder()->addAlreadyExistingChild(file);
			ViewController::get()->setInvalidGamesList(SystemData::getFavoriteSystem());
			ViewController::get()->getSystemListView()->manageFavorite();
		}
	}

	updateInfoPanel();
}

bool ISimpleGameListView::input(InputConfig* config, Input input) {
	bool hideSystemView = RecalboxConf::getInstance()->get("emulationstation.hidesystemview") == "1";

	if (input.value != 0) {
		if (config->isMappedTo("b", input)) {
			FileData* cursor = getCursor();
			if (cursor->getType() == GAME) {
				//Sound::getFromTheme(getTheme(), getName(), "launch")->play();
				launch(cursor);
			} else {
				// it's a folder
				if (cursor->getChildren().size() > 0) {
					mCursorStack.push(cursor);
					populateList(cursor);
					setCursorIndex(0);
				}
			}
			return true;
		}

		if(config->isMappedTo("a", input)) {
			if (mCursorStack.size()) {
				FileData* selected = mCursorStack.top();

				// remove current folder from stack
				mCursorStack.pop();

				FileData* cursor = mCursorStack.size() ? mCursorStack.top() : getRoot();
				populateList(cursor);

				setCursor(selected);
				//Sound::getFromTheme(getTheme(), getName(), "back")->play();
			} else if (!hideSystemView) {
				onFocusLost();

				if (mFavoriteChange) {
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
				if (cursor->getType() == GAME) {
					mFavoriteChange = true;
					MetaDataList* md = &cursor->metadata;
					std::string value = md->get("favorite");
					bool removeFavorite = false;
					SystemData *favoriteSystem = SystemData::getFavoriteSystem();

					if (value == "true") {
                        md->set("favorite", "false");
                        if (favoriteSystem != NULL) {
                            favoriteSystem->getRootFolder()->removeAlreadyExistingChild(cursor);
                        }
                        removeFavorite = true;
					} else {
                        md->set("favorite", "true");
                        if (favoriteSystem != NULL) {
                            favoriteSystem->getRootFolder()->addAlreadyExistingChild(cursor);
                        }
					}

					if (favoriteSystem != NULL) {
					  ViewController::get()->setInvalidGamesList(favoriteSystem);
					  ViewController::get()->getSystemListView()->manageFavorite();
					}

					// Reload to refresh the favorite icon
					int cursorPlace = getCursorIndex();
                    refreshList();
					setCursorIndex(std::max(0, cursorPlace + (removeFavorite ? -1 : 1)));
				}
			}
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
				return true;
			}
		}else if (config->isMappedTo("x", input))
		{
			FileData* cursor = getCursor();
			if(cursor->getType() == GAME)
			{
				auto s = new GuiSettings(mWindow, _("NETPLAY").c_str());
				auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
				Window *window = mWindow;
				ComponentListRow row;
				row.makeAcceptInputHandler([window, cursor] {
					Eigen::Vector3f target(Renderer::getScreenWidth() / 2.0f, Renderer::getScreenHeight() / 2.0f, 0);
					ViewController::get()->launch(cursor, target, "host");
				});
				auto lbl = std::make_shared<TextComponent>(mWindow, _("SERVER"), menuTheme->menuText.font, menuTheme->menuText.color);
				row.addElement(lbl, true); // label
				// spacer between text and arrow
				auto spacer = std::make_shared<GuiComponent>(mWindow);
				spacer->setSize(Renderer::getScreenWidth() * 0.005f, 0);
				row.addElement(spacer, false);
				auto bracket = std::make_shared<ImageComponent>(mWindow);
				bracket->setImage(":/arrow.svg");
				bracket->setResize(Eigen::Vector2f(0, lbl->getFont()->getLetterHeight()));
				row.addElement(bracket, false);
				s->addRow(row);

				row.elements.clear();

				row.makeAcceptInputHandler([window, cursor] {
					auto netplay = new GuiNetPlay(window, cursor);
					window->pushGui(netplay);
				});
				auto lbl2 = std::make_shared<TextComponent>(mWindow, _("CLIENT"), menuTheme->menuText.font, menuTheme->menuText.color);
				row.addElement(lbl2, true); // label
				// spacer between text and arrow
				row.addElement(spacer, false);
				auto bracket2 = std::make_shared<ImageComponent>(mWindow);
				bracket2->setImage(":/arrow.svg");
				bracket2->setResize(Eigen::Vector2f(0, lbl->getFont()->getLetterHeight()));
				row.addElement(bracket2, false);
				s->addRow(row);
				mWindow->pushGui(s);
			}
		}
	}

	return IGameListView::input(config, input);
}
