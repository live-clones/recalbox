#include <RecalboxConf.h>
#include "GuiGamelistOptions.h"
#include "GuiMetaDataEd.h"
#include "Settings.h"
#include "views/gamelist/IGameListView.h"
#include "views/ViewController.h"
#include "components/SwitchComponent.h"
#include "guis/GuiSettings.h"
#include "utils/locale/LocaleHelper.h"
#include "MenuMessages.h"
#include "guis/GuiMsgBox.h"

GuiGamelistOptions::GuiGamelistOptions(Window& window, SystemData* system)
  :	Gui(window),
		mMenu(window, _("OPTIONS")),
    mReloading(false),
    mSystem(system)
{
	ComponentListRow row;

	auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
	addChild(&mMenu);

	mJumpToLetterList = std::make_shared<LetterList>(mWindow, _("JUMP TO LETTER"), false);

    std::vector<FileSorts::SortType> & sortTypes = system->isFavorite() ? FileSorts::SortTypesForFavorites : FileSorts::SortTypes;

	std::vector<std::string> letters = getGamelist()->getAvailableLetters();
	if (!letters.empty()) { // may happen if only contains folders

		// jump to letter
		auto curChar = (char) toupper(getGamelist()->getCursor()->getName()[0]);

		// if curChar not found in available letter, take first one
		if (std::find(letters.begin(), letters.end(), std::string(1, curChar)) == letters.end()) {
			curChar = letters.at(0)[0];
		}

		for (auto letter : letters) {
			mJumpToLetterList->add(letter, letter[0], letter[0] == curChar);
		}

		row.addElement(std::make_shared<TextComponent>(mWindow, _("JUMP TO LETTER"), menuTheme->menuText.font,
													   menuTheme->menuText.color), true);
		row.addElement(mJumpToLetterList, false);
		row.input_handler = [&](const InputCompactEvent& event) {
			if (event.BPressed()) {
				jumpToLetter();
				return true;
			} else if (mJumpToLetterList->ProcessInput(event)) {
				return true;
			}
			return false;
		};
		mMenu.addRowWithHelp(row, _("JUMP TO LETTER"), _(MENUMESSAGE_GAMELISTOPTION_JUMP_LETTER_MSG));
	}

	// sort list by
	unsigned int currentSortId = mSystem->getSortId();
	if (currentSortId > sortTypes.size()) {
		currentSortId = 0;
	}
	mListSort = std::make_shared<SortList>(mWindow, _("SORT GAMES BY"), false);
	for (unsigned int i = 0; i < (unsigned int)sortTypes.size(); i++) {
		const FileSorts::SortType& sortType = sortTypes.at(i);
		mListSort->add(sortType.description, i, i == currentSortId);
	}

	mMenu.addWithLabel(mListSort, _("SORT GAMES BY"), _(MENUMESSAGE_GAMELISTOPTION_SORT_GAMES_MSG));
	addSaveFunc([this, system] { RecalboxConf::Instance().SetInt(system->getName() + ".sort", mListSort->getSelected()); });

	if (!system->isFavorite()) {
	    // favorite only
        auto favorite_only = std::make_shared<SwitchComponent>(mWindow);
        favorite_only->setState(Settings::Instance().FavoritesOnly());
        mMenu.addWithLabel(favorite_only, _("FAVORITES ONLY"), _(MENUMESSAGE_GAMELISTOPTION_FAVORITES_ONLY_MSG));
        addSaveFunc([favorite_only] { Settings::Instance().SetFavoritesOnly(favorite_only->getState()); });

        // show hidden
        auto show_hidden = std::make_shared<SwitchComponent>(mWindow);
        show_hidden->setState(Settings::Instance().ShowHidden());
        mMenu.addWithLabel(show_hidden, _("SHOW HIDDEN"), _(MENUMESSAGE_GAMELISTOPTION_SHOW_HIDDEN_MSG));
        addSaveFunc([show_hidden] { Settings::Instance().SetShowHidden(show_hidden->getState()); });

    	// flat folders
        auto flat_folders = std::make_shared<SwitchComponent>(mWindow);
        flat_folders->setState(RecalboxConf::Instance().AsBool(system->getName() + ".flatfolder"));
        mMenu.addWithLabel(flat_folders, _("SHOW FOLDERS CONTENT"), _(MENUMESSAGE_GAMELISTOPTION_SHOW_FOLDER_CONTENT_MSG));
        addSaveFunc([flat_folders, system] { RecalboxConf::Instance().SetBool(system->getName() + ".flatfolder", flat_folders->getState()); });
    }

	// edit game metadata
	row.elements.clear();

	if (RecalboxConf::Instance().AsString("emulationstation.menu") != "none" && RecalboxConf::Instance().AsString("emulationstation.menu") != "bartop"){
		row.addElement(std::make_shared<TextComponent>(mWindow, _("EDIT THIS GAME'S METADATA"), menuTheme->menuText.font, menuTheme->menuText.color), true);
		row.addElement(makeArrow(mWindow), false);
		row.makeAcceptInputHandler(std::bind(&GuiGamelistOptions::openMetaDataEd, this));
		mMenu.addRowWithHelp(row, _("EDIT THIS GAME'S METADATA"), _(MENUMESSAGE_GAMELISTOPTION_EDIT_METADATA_MSG));
	}

	if (!system->isFavorite()) {
        // update game list
        row.elements.clear();
        row.addElement(std::make_shared<TextComponent>(mWindow, _("UPDATE GAMES LISTS"), menuTheme->menuText.font,
                                                       menuTheme->menuText.color), true);
        row.addElement(makeArrow(mWindow), false);
        row.makeAcceptInputHandler([this] {
            mReloading = true;
            mWindow.pushGui(new GuiMsgBox(mWindow, _("REALLY UPDATE GAMES LISTS ?"),
                                          _("YES"), [] { ViewController::deleteAndReloadAll(true); },
                                          _("NO"), [this] { mReloading = false; }
            ));
        });
        mMenu.addRowWithHelp(row, _("UPDATE GAMES LISTS"), _(MENUMESSAGE_UI_UPDATE_GAMELIST_HELP_MSG));
    }

	// center the menu
	setSize(Renderer::getDisplayWidthAsFloat(), Renderer::getDisplayHeightAsFloat());
	mMenu.setPosition((mSize.x() - mMenu.getSize().x()) / 2, (mSize.y() - mMenu.getSize().y()) / 2);

	mFavoriteState = Settings::Instance().FavoritesOnly();
	mHiddenState = Settings::Instance().ShowHidden();
}

GuiGamelistOptions::~GuiGamelistOptions()
{
	if (mReloading) return;

	FolderData* root = getGamelist()->getRoot();
	if (root->countAll(false) != 0)
	{
		if (mListSort->getSelected() != (int)mSystem->getSortId())
		{
			// apply sort
			mSystem->setSortId((unsigned int)mListSort->getSelected());

			// notify that the root folder has to be sorted
			getGamelist()->onFileChanged(mSystem->getRootFolder(), FileChangeType::Sorted);
		}
		else
		{
			// require list refresh
			getGamelist()->onFileChanged(mSystem->getRootFolder(), FileChangeType::DisplayUpdated);
		}

		// if states has changed, invalidate and reload game list
		if (Settings::Instance().FavoritesOnly() != mFavoriteState || Settings::Instance().ShowHidden() != mHiddenState)
		{
      ViewController::Instance().setAllInvalidGamesList(getGamelist()->getCursor()->getSystem());
      ViewController::Instance().reloadGameListView(getGamelist()->getCursor()->getSystem());
		}
	}
	else
	{
    ViewController::deleteAndReloadAll(false);
	}
}

void GuiGamelistOptions::openMetaDataEd() {
	// open metadata editor
	FileData* file = getGamelist()->getCursor();
	mWindow.pushGui(new GuiMetaDataEd(mWindow, file->Metadata(), *file, file->getPath().Filename(),
									 std::bind(&IGameListView::onFileChanged, getGamelist(), file, FileChangeType::MetadataChanged), [this, file]
									 {
				             file->getPath().Delete();
				             if (file->getParent() != nullptr)
			                 file->getParent()->removeChild(file); //unlink it so list repopulations triggered from onFileChanged won't see it
				             getGamelist()->onFileChanged(file, FileChangeType::Removed); //tell the view
                	 }, file->getSystem(), true));
}

void GuiGamelistOptions::jumpToLetter() {
	char letter = mJumpToLetterList->getSelected();
	auto sortId = (unsigned int) mListSort->getSelected();
	IGameListView* gamelist = getGamelist();

	// if sort is not alpha, need to force an alpha
	if (sortId > 1) {
		sortId = 0; // Alpha ASC
		// select to avoid resort on destroy
		mListSort->select(sortId);
	}

	if (sortId != mSystem->getSortId()) {
		// apply sort
		mSystem->setSortId(sortId);
		// notify that the root folder has to be sorted
		gamelist->onFileChanged(mSystem->getRootFolder(), FileChangeType::Sorted);
	}

	gamelist->jumpToLetter(letter);
	Close();
}

bool GuiGamelistOptions::ProcessInput(const InputCompactEvent& event) {
	if (event.APressed() || event.StartPressed()) {
		save();
		Close();
		return true;
	}
	return mMenu.ProcessInput(event);
}

bool GuiGamelistOptions::getHelpPrompts(Help& help)
{
	mMenu.getHelpPrompts(help);
	help.Set(HelpType::A, _("CLOSE"));
	return true;
}

IGameListView* GuiGamelistOptions::getGamelist() {
	return ViewController::Instance().getGameListView(mSystem).get();
}

void GuiGamelistOptions::save() {
	if (mSaveFuncs.empty()) {
		return;
	}
	for (auto & mSaveFunc : mSaveFuncs) {
		mSaveFunc();
	}
	Settings::Instance().saveFile();
	RecalboxConf::Instance().SaveRecalboxConf();
}
