//
// Created by xizor on 20/05/18.
//

#include <RecalboxConf.h>
#include <SystemData.h>
#include <views/ViewController.h>
#include "GuiNetPlay.h"
#include "components/TextComponent.h"
#include "components/ButtonComponent.h"
#include "components/MenuComponent.h"
#include <boost/regex.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <regex>


#define BUTTON_GRID_VERT_PADDING 32
#define BUTTON_GRID_HORIZ_PADDING 10

#define TITLE_HEIGHT (mTitle->getFont()->getLetterHeight() + TITLE_VERT_PADDING)

using namespace Eigen;

bool compareLowerCase(std::string str1, std::string str2)
{
	for(unsigned int i = 0; i < str1.length(); i++)
	{
		str1[i] = tolower(str1[i]);
	}

	for(unsigned int i = 0; i < str2.length(); i++)
	{
		str2[i] = tolower(str2[i]);
	}

	if (str1 == str2) {
		return true;
	} else {
		return false;
	};
}
//end util functions

GuiNetPlay::GuiNetPlay(Window* window) : GuiComponent(window),
        mBackground(window, ":/frame.png"), mGrid(window, Eigen::Vector2i(1, 3)), mList(NULL),
        mGridMeta(new ComponentGrid(window, Eigen::Vector2i(2, 1))),
        mGridMetaRight(new ComponentGrid(window, Eigen::Vector2i(1, 2))), mBusyAnim(window)
{
	addChild(&mBackground);
	addChild(&mGrid);

	mLoading = false;
	mLobbyChecked = false;

	mMenuTheme = MenuThemeData::getInstance()->getCurrentTheme();

	mBackground.setImagePath(mMenuTheme->menuBackground.path);
	mBackground.setCenterColor(mMenuTheme->menuBackground.color);
	mBackground.setEdgeColor(mMenuTheme->menuBackground.color);

	mTitle = std::make_shared<TextComponent>(mWindow, _("NETPLAY"), mMenuTheme->menuTitle.font, mMenuTheme->menuTitle.color, ALIGN_CENTER);
	mGrid.setEntry(mTitle, Eigen::Vector2i(0, 0), false);

	mButtons.push_back(std::make_shared<ButtonComponent>(mWindow, _("CLOSE"), _("CLOSE"), [this] { delete this; }));

	mButtonGrid = makeButtonGrid(mWindow, mButtons);
	mGrid.setEntry(mButtonGrid, Eigen::Vector2i(0, 2), true, false);

	updateSize();
	mBusyAnim.setSize(mSize);
	setPosition((Renderer::getScreenWidth() - mSize.x()) / 2, (Renderer::getScreenHeight() - mSize.y()) / 2);

}

//ugly method to start a new thread THEN populate the grid via update()
void GuiNetPlay::checkLobby()
{
	mLobbyLoaded = parseLobby();

	mLoading = false;

	mLobbyChecked = true;
}

bool GuiNetPlay::parseLobby()
{
	mRooms.clear();
	std::string lobby = RecalboxConf::getInstance()->get("global.netplay.lobby");
	auto json_req = RecalboxSystem::getInstance()->execute("curl -s --connect-timeout 3 -m 3 " + lobby);
	if (json_req.second == 0) {
		json::ptree root;
		std::stringstream ss;
		ss << json_req.first;
		try {
			json::read_json(ss, root);
		}
		catch (const boost::property_tree::json_parser_error& e1) {
			return false;
		}

		for (json::ptree::value_type &array_element : root) {
			FileData* tmp = NULL;
			if (array_element.second.get<std::string>("fields.game_crc") != "00000000") {
				tmp = findGame(array_element.second.get<std::string>("fields.game_crc"));
			}
			if (!tmp) {
				tmp = findGame(array_element.second.get<std::string>("fields.game_name"));
			}
			mGames.push_back(tmp);

			//empty strings for ping on loading
			mPings.push_back("\uF1c9 " + _("unknown"));
			mRooms.push_back(array_element);
		}
		return true;
	} else {
		return false;
	}
}

void GuiNetPlay::populateGrid()
{
	if (mLobbyLoaded) {
		mList = std::make_shared<ComponentList>(mWindow);
		mGridMeta->setEntry(mList, Vector2i(0, 0), true);
		mGridMeta->setColWidthPerc(0, 0.62);
		mGrid.setEntry(mGridMeta, Vector2i(0, 1), true);

		mMetaText = std::make_shared<TextComponent>(mWindow, "", mMenuTheme->menuTextSmall.font, mMenuTheme->menuTextSmall.color, ALIGN_LEFT);
		mMetaText->setVerticalAlignment(ALIGN_TOP);
		mLaunchText = std::make_shared<TextComponent>(mWindow, "", mMenuTheme->menuText.font, mMenuTheme->menuText.color, ALIGN_LEFT);
		mLaunchText->setVerticalAlignment(ALIGN_BOTTOM);
		mGridMetaRight->setEntry(mLaunchText, Vector2i(0, 1), false, true);
		mGridMetaRight->setEntry(mMetaText, Vector2i(0, 0), false, true);
		mGridMeta->setEntry(mGridMetaRight, Vector2i(1, 0), false, true, Eigen::Vector2i(1, 1), GridFlags::BORDER_LEFT);

		ComponentListRow row;
		std::shared_ptr<GuiComponent> ed;
		int i = 0;
		for (auto v : mRooms) {
			row.elements.clear();
			std::string text;
			if (mGames[i]) {
				if (mGames[i]->getHash() == v.second.get<std::string>("fields.game_crc")) {
					text = "\uf1c0 " + v.second.get<std::string>("fields.game_name");
				} else {
					text = "\uf1c1 " + v.second.get<std::string>("fields.game_name");
				}
			} else {
				text = "\uf1c2 " + v.second.get<std::string>("fields.game_name");
			}
			ed = std::make_shared<TextComponent>(mWindow, text, mMenuTheme->menuText.font, mMenuTheme->menuText.color, ALIGN_LEFT);
			row.addElement(ed, true);
			row.makeAcceptInputHandler([this] { launch();});
			addRow(row);
			i++;
		}
		populateGridMeta(0);
		mList->setCursorChangedCallback([this](CursorState state){populateGridMeta(mList->getCursor());});
		mList->setFocusLostCallback([this]{mMetaText->setText(""); mLaunchText->setText("");});
		mList->setFocusGainedCallback([this]{populateGridMeta(mList->getCursor());});

		mHandle = new boost::thread(boost::bind(&GuiNetPlay::pingLobby, this));

		mGrid.moveCursor(Eigen::Vector2i(0, -1));
	} else {
		auto text = std::make_shared<TextComponent>(mWindow, _("NO GAMES OR NO CONNECTION"), mMenuTheme->menuText.font, mMenuTheme->menuText.color, ALIGN_CENTER);
		mGrid.setEntry(text, Vector2i(0, 1), true);
	}
	mLobbyChecked = false;
}

//called when changing cursor in mList
void GuiNetPlay::populateGridMeta(int i)
{
	std::string text = "";

	bool hashMatch = false;
	bool coreVerMatch,coreMatch;

	if (mGames[i]) {
		hashMatch = mGames[i]->getHash() == mRooms[i].second.get<std::string>("fields.game_crc");
	}
		coreVerMatch = getCoreInfo(mRooms[i].second.get<std::string>("fields.core_name")).second == mRooms[i].second.get<std::string>("fields.core_version");
		coreMatch = getCoreInfo(mRooms[i].second.get<std::string>("fields.core_name")).first != "";

    std::string username = mRooms[i].second.get<std::string>("fields.username", "N/A");
    username = std::regex_replace(username, std::regex("@RECALBOX"), " \uF200");

    text += "    " + _("Username") + " : " + username;
    text += "\n    " + _("Country") + " : " + mRooms[i].second.get<std::string>("fields.country", "N/A");
    text += "\n    " + _("Rom hash") + " : ";
    if (hashMatch) {
    	text += "\uf1c0 " + _("Match");
    } else {
	    text += "\uf1c2 " + _("No match");
    }
	text += "\n    " + _("Rom file") + " : ";
	if (mGames[i]) {
		text += "\uf1c0 " + _("Match");
	} else {
		text += "\uf1c2 " + _("No match");
	}
    text += "\n    " + _("Core") + " : ";
	if (coreMatch) {
		text += "\uf1c0 ";
	} else {
		text += "\uf1c2 ";
	}
    text += mRooms[i].second.get<std::string>("fields.core_name", "N/A");
    text += "\n    " + _("Core ver.") + " : ";
	if (coreVerMatch) {
		text += "\uf1c0 ";
	} else {
		text += "\uf1c2 ";
	}
    text += mRooms[i].second.get<std::string>("fields.core_version", "N/A");
	text += "\n    Latency : " + mPings[i];
    text += "\n    " + _("RA ver.") + " : " + mRooms[i].second.get<std::string>("fields.retroarch_version", "N/A");
	text += "\n    " + _("Host arch.") + " : " + mRooms[i].second.get<std::string>("fields.frontend", "N/A");
	mMetaText->setText(text);
	std::string text2 = "    " + _("Can join") + " : ";
	if (mGames[i]) {
		if (hashMatch && coreMatch) {
			text2 += "\uf1c0 " + _("Rom and core match");
			mLaunchText->setColor(0x26B14AFF);
		} else {
			text2 += "\uf1c1 " + _("Rom found w/o hash");
			mLaunchText->setColor(0x36A9E0FF);
		}
	} else {
		text2 += "\uf1c2 " + _("No rom match");
		mLaunchText->setColor(0xDC1F26FF);
	}
	mLaunchText->setText(text2);
}

void GuiNetPlay::launch()
{
	if (mGames[mList->getCursor()]) {
        Eigen::Vector3f target(Renderer::getScreenWidth() / 2.0f, Renderer::getScreenHeight() / 2.0f, 0);
        int index = mList->getCursor();
        std::string core = getCoreInfo(mRooms[index].second.get<std::string>("fields.core_name")).first;
        std::string ip, port;
        if (mRooms[index].second.get<std::string>("fields.host_method") == "3") {
            ip = mRooms[index].second.get<std::string>("fields.mitm_ip");
            port = mRooms[index].second.get<std::string>("fields.mitm_port");
        } else {
            ip = mRooms[index].second.get<std::string>("fields.ip");
            port = mRooms[index].second.get<std::string>("fields.port");
        }
        ViewController::get()->launch(mGames[index], target, "client", core, ip, port);
        delete this;
	}
}



FileData* GuiNetPlay::findGame(std::string gameNameOrHash)
{
	for (auto tmp : SystemData::sSystemVector) {
		if (RecalboxConf::getInstance()->isInList("global.netplay.systems", tmp->getName())) {
			std::vector<FileData*> games = tmp->getRootFolder()->getChildren();
			FileData* result = findRecursive(games, gameNameOrHash);
			if (result != NULL) {
				return result;
			}
		}
	}
	return nullptr;
}

FileData* GuiNetPlay::findRecursive(const std::vector<FileData*>& gameFolder, const std::string& gameNameOrHash, const std::string& relativePath) {
	// Recursively look for the game in subfolders too
	for (auto game = gameFolder.begin(); game != gameFolder.end(); ++game) {
		std::string gameAndPath;
		if (relativePath.empty()) {
			gameAndPath = (*game)->getPath().stem().c_str();
		} else {
			gameAndPath = relativePath + '/' + (*game)->getPath().stem().c_str();
		}
		//LOG(LogInfo) << "Checking game " << gameNameOrHash << " in path: " << gameAndPath;
		if ((*game)->getType() == FOLDER) {
			FileData* foundGame = findRecursive((*game)->getChildren(), gameNameOrHash, gameAndPath);
			if ( foundGame != NULL ) {
				return foundGame;
			}
		}
		if ((*game)->getType() == GAME and (compareLowerCase(gameAndPath, gameNameOrHash) or (*game)->getHash() == gameNameOrHash)) {
			return *game;
		}
	}
	return NULL;
}

std::pair<std::string, std::string> GuiNetPlay::getCoreInfo(const std::string &name) {
    boost::regex validLine("^(?<key>[^;|#].*?);(?<val>.*?)$");
    std::pair<std::string, std::string> result;
    result.first = "";
    result.second = "";
    std::map<std::string, std::string> coreMap;
    std::string line;
    std::string filePath = "/recalbox/system/resources/retroarch.corenames";
    std::ifstream retroarchCores(filePath);
    if (retroarchCores && retroarchCores.is_open()) {
        while (std::getline(retroarchCores, line)) {
            boost::smatch lineInfo;
            if (boost::regex_match(line, lineInfo, validLine)) {
                coreMap[std::string(lineInfo["key"])] = std::string(lineInfo["val"]);
            }
        }
        retroarchCores.close();
    } else {
        LOG(LogError) << "Unable to open " << filePath;
        return result;
    }
    if (coreMap.count(name)) {
        std::string s = coreMap[name];
        std::string delimiter = ";";
        size_t pos = 0;
        std::string token;
        while (((pos = s.find(delimiter)) != std::string::npos) ) {
            token = s.substr(0, pos);
            result.first = token;
            s.erase(0, pos + delimiter.length());
        }
        result.second = s;
        return result;
    }
    return result;
}

void GuiNetPlay::pingLobby()
{
	for (int i=0; i<mRooms.size(); i++ ) {
		std::string ip = mRooms[i].second.get<std::string>("fields.ip");
		mPings[i] = pingHost(ip);
	}
}

std::string GuiNetPlay::pingHost(const std::string& ip)
{
    std::pair<std::string, int> ping = RecalboxSystem::getInstance()->execute("ping -c 1 -w 1 " + ip + " | grep \"min/avg/max\" | cut -d '/' -f 5");
    if (ping.first != "") {
        float latency = strtof(ping.first.c_str(), 0);
        if (latency <=80) {
            return "\uF1c8 " + _("good");
        } else if (latency <= 150) {
            return "\uF1c7 " + _("medium");
        } else {
            return "\uF1c6 " + _("bad");
        }
    } else {
        return "\uF1c9 " + _("unknown");
    };
}

float GuiNetPlay::getButtonGridHeight() const
{
	auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
	return (mButtonGrid ? mButtonGrid->getSize().y() : menuTheme->menuText.font->getHeight() + BUTTON_GRID_VERT_PADDING);
}

bool GuiNetPlay::input(InputConfig* config, Input input)
{
	if (config->isMappedTo("a", input) && input.value != 0)
	{
		delete this;
	}
	return GuiComponent::input(config, input);
}

void GuiNetPlay::updateSize()
{
	const float height = Renderer::getScreenHeight() * 0.7f;
	const float width = Renderer::getScreenWidth() * 0.8f;
	setSize(width, height);
}

void GuiNetPlay::onSizeChanged()
{
	mBackground.fitTo(mSize, Eigen::Vector3f::Zero(), Eigen::Vector2f(-32, -32));

	// update grid row/col sizes
	mGrid.setRowHeightPerc(0, TITLE_HEIGHT / mSize.y());
	mGrid.setRowHeightPerc(2, getButtonGridHeight() / mSize.y());

	mGrid.setSize(mSize);
}

std::vector<HelpPrompt> GuiNetPlay::getHelpPrompts()
{
	std::vector<HelpPrompt> prompts = mGrid.getHelpPrompts();
	prompts.push_back(HelpPrompt("a", _("BACK")));
	prompts.push_back(HelpPrompt("b", _("LAUNCH")));
	return prompts;
}

void GuiNetPlay::update(int deltaTime) {
	GuiComponent::update(deltaTime);
	mBusyAnim.update(deltaTime);

	if (!mLoaded) {
		mLoading = true;
		mHandle = new boost::thread(boost::bind(&GuiNetPlay::checkLobby, this));
		mLoaded = true;
	}

	if (mLobbyChecked) {
		populateGrid();
	}
}

void GuiNetPlay::render(const Eigen::Affine3f &parentTrans) {
	Eigen::Affine3f trans = parentTrans * getTransform();

	renderChildren(trans);

	Renderer::setMatrix(trans);
	Renderer::drawRect(0.f, 0.f, mSize.x(), mSize.y(), 0x00000011);

	if (mLoading)
		mBusyAnim.render(trans);

}