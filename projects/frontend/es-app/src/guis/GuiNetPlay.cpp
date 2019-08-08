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


#define BUTTON_GRID_VERT_PADDING Renderer::getScreenHeight() * 0.025f
#define BUTTON_GRID_HORIZ_PADDING 10

#define TITLE_HEIGHT (mTitle->getFont()->getLetterHeight() + TITLE_VERT_PADDING)

bool compareLowerCase(std::string str1, std::string str2)
{
	for (unsigned int i = 0; i < str1.length(); i++)
	{
		str1[i] = tolower(str1[i]);
	}

	for (unsigned int i = 0; i < str2.length(); i++)
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

GuiNetPlay::GuiNetPlay(Window* window)
  : GuiComponent(window),
    mBackground(window, ":/frame.png"),
		mBusyAnim(window),
    mGrid(window, Vector2i(1, 3)),
    mGridMeta(new ComponentGrid(window, Vector2i(2, 1))),
    mGridMetaRight(new ComponentGrid(window, Vector2i(2, 11))),
    mList(nullptr)
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
	mGrid.setEntry(mTitle, Vector2i(0, 0), false);

	mButtons.push_back(std::make_shared<ButtonComponent>(mWindow, _("CLOSE"), _("CLOSE"), [this] { delete this; }));

	mButtonGrid = makeButtonGrid(mWindow, mButtons);
	mGrid.setEntry(mButtonGrid, Vector2i(0, 2), true, false);

	updateSize();
	mBusyAnim.setSize(mSize);
	setPosition((Renderer::getScreenWidth() - mSize.x()) / 2, (Renderer::getScreenHeight() - mSize.y()) / 2);

}

GuiNetPlay::~GuiNetPlay()
{
	if (mList)
	{
		mList->clear();
	}
	// kill the ping thread or wait for it to join on close
	pthread_cancel(mHandle->native_handle());
	//mHandle->join();
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
		if (ss.str() == "[]")
        {
            return false;
        }
		try {
			json::read_json(ss, root);
		}
		catch (const boost::property_tree::json_parser_error& e1) {
			return false;
		}

		for (json::ptree::value_type &array_element : root) {
		    if (array_element.second.get<bool>("fields.has_password"))
            {
                continue;
            }
			if (!(std::regex_search(array_element.second.get<std::string>("fields.frontend"), std::regex("@RECALBOX"))))
			{
				continue;
			}
			FileData* tmp = nullptr;
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
		mGridMeta->setColWidthPerc(0, 0.57);
		mGrid.setEntry(mGridMeta, Vector2i(0, 1), true);

		mMetaTextLblUsername = std::make_shared<TextComponent>(mWindow, "    " +  _("Username") + " : ", mMenuTheme->menuTextSmall.font, mMenuTheme->menuTextSmall.color, ALIGN_LEFT);
		mMetaTextUsername = std::make_shared<TextComponent>(mWindow, "", mMenuTheme->menuTextSmall.font, mMenuTheme->menuTextSmall.color, ALIGN_LEFT);
		mGridMetaRight->setEntry(mMetaTextLblUsername, Vector2i(0, 0), false, true);
		mGridMetaRight->setEntry(mMetaTextUsername, Vector2i(1, 0), false, true);
		mMetaTextLblCountry = std::make_shared<TextComponent>(mWindow, "    " +  _("Country") + " : ", mMenuTheme->menuTextSmall.font, mMenuTheme->menuTextSmall.color, ALIGN_LEFT);
		mMetaTextCountry = std::make_shared<TextComponent>(mWindow, "", mMenuTheme->menuTextSmall.font, mMenuTheme->menuTextSmall.color, ALIGN_LEFT);
		mGridMetaRight->setEntry(mMetaTextLblCountry, Vector2i(0, 1), false, true);
		mGridMetaRight->setEntry(mMetaTextCountry, Vector2i(1, 1), false, true);
		mMetaTextLblRomHash = std::make_shared<TextComponent>(mWindow, "    " +  _("Rom hash") + " : ", mMenuTheme->menuTextSmall.font, mMenuTheme->menuTextSmall.color, ALIGN_LEFT);
		mMetaTextRomHash = std::make_shared<TextComponent>(mWindow, "", mMenuTheme->menuTextSmall.font, mMenuTheme->menuTextSmall.color, ALIGN_LEFT);
		mGridMetaRight->setEntry(mMetaTextLblRomHash, Vector2i(0, 2), false, true);
		mGridMetaRight->setEntry(mMetaTextRomHash, Vector2i(1, 2), false, true);
		mMetaTextLblRomFile = std::make_shared<TextComponent>(mWindow, "    " +  _("Rom file") + " : ", mMenuTheme->menuTextSmall.font, mMenuTheme->menuTextSmall.color, ALIGN_LEFT);
		mMetaTextRomFile = std::make_shared<TextComponent>(mWindow, "", mMenuTheme->menuTextSmall.font, mMenuTheme->menuTextSmall.color, ALIGN_LEFT);
		mGridMetaRight->setEntry(mMetaTextLblRomFile, Vector2i(0, 3), false, true);
		mGridMetaRight->setEntry(mMetaTextRomFile, Vector2i(1, 3), false, true);
		mMetaTextLblCore = std::make_shared<TextComponent>(mWindow, "    " +  _("Core") + " : ", mMenuTheme->menuTextSmall.font, mMenuTheme->menuTextSmall.color, ALIGN_LEFT);
		mMetaTextCore = std::make_shared<TextComponent>(mWindow, "", mMenuTheme->menuTextSmall.font, mMenuTheme->menuTextSmall.color, ALIGN_LEFT);
		mGridMetaRight->setEntry(mMetaTextLblCore, Vector2i(0, 4), false, true);
		mGridMetaRight->setEntry(mMetaTextCore, Vector2i(1, 4), false, true);
		mMetaTextLblCoreVer = std::make_shared<TextComponent>(mWindow, "    " +  _("Core ver.") + " : ", mMenuTheme->menuTextSmall.font, mMenuTheme->menuTextSmall.color, ALIGN_LEFT);
		mMetaTextCoreVer = std::make_shared<TextComponent>(mWindow, "", mMenuTheme->menuTextSmall.font, mMenuTheme->menuTextSmall.color, ALIGN_LEFT);
		mGridMetaRight->setEntry(mMetaTextLblCoreVer, Vector2i(0, 5), false, true);
		mGridMetaRight->setEntry(mMetaTextCoreVer, Vector2i(1, 5), false, true);
		mMetaTextLblLatency = std::make_shared<TextComponent>(mWindow, "    " +  _("Latency") + " : ", mMenuTheme->menuTextSmall.font, mMenuTheme->menuTextSmall.color, ALIGN_LEFT);
		mMetaTextLatency = std::make_shared<TextComponent>(mWindow, "", mMenuTheme->menuTextSmall.font, mMenuTheme->menuTextSmall.color, ALIGN_LEFT);
		mGridMetaRight->setEntry(mMetaTextLblLatency, Vector2i(0, 6), false, true);
		mGridMetaRight->setEntry(mMetaTextLatency, Vector2i(1, 6), false, true);
		mMetaTextLblRAVer = std::make_shared<TextComponent>(mWindow, "    " +  _("RA ver.") + " : ", mMenuTheme->menuTextSmall.font, mMenuTheme->menuTextSmall.color, ALIGN_LEFT);
		mMetaTextRAVer = std::make_shared<TextComponent>(mWindow, "", mMenuTheme->menuTextSmall.font, mMenuTheme->menuTextSmall.color, ALIGN_LEFT);
		mGridMetaRight->setEntry(mMetaTextLblRAVer, Vector2i(0, 7), false, true);
		mGridMetaRight->setEntry(mMetaTextRAVer, Vector2i(1, 7), false, true);
		mMetaTextLblHostArch = std::make_shared<TextComponent>(mWindow, "    " +  _("Host arch.") + " : ", mMenuTheme->menuTextSmall.font, mMenuTheme->menuTextSmall.color, ALIGN_LEFT);
		mMetaTextHostArch = std::make_shared<TextComponent>(mWindow, "", mMenuTheme->menuTextSmall.font, mMenuTheme->menuTextSmall.color, ALIGN_LEFT);
		mGridMetaRight->setEntry(mMetaTextLblHostArch, Vector2i(0, 8), false, true);
		mGridMetaRight->setEntry(mMetaTextHostArch, Vector2i(1, 8), false, true);
		mMetaTextLblCanJoin = std::make_shared<TextComponent>(mWindow, "    " +  _("Can join") + " : ", mMenuTheme->menuTextSmall.font, mMenuTheme->menuTextSmall.color, ALIGN_LEFT);
		mMetaTextCanJoin = std::make_shared<TextComponent>(mWindow, "", mMenuTheme->menuTextSmall.font, mMenuTheme->menuTextSmall.color, ALIGN_LEFT);
		mGridMetaRight->setEntry(mMetaTextLblCanJoin, Vector2i(0, 10), false, true);
		mGridMetaRight->setEntry(mMetaTextCanJoin, Vector2i(1, 10), false, true);
		mGridMeta->setEntry(mGridMetaRight, Vector2i(1, 0), false, true, Vector2i(1, 1), GridFlags::BORDER_LEFT);
        mGridMetaRight->setColWidthPerc(1, 0.60, true);

		ComponentListRow row;
		std::shared_ptr<GuiComponent> ed;
		int i = 0;
		for (auto v : mRooms) {
			row.elements.clear();
			std::string text, gameName;
			if (v.second.get<std::string>("fields.core_name") == "FB Alpha" ||
					v.second.get<std::string>("fields.core_name") == "MAME 2000" ||
					v.second.get<std::string>("fields.core_name") == "MAME 2010" ||
					v.second.get<std::string>("fields.core_name") == "MAME 2003")
            {
                gameName = PlatformIds::getCleanMameName(v.second.get<std::string>("fields.game_name").c_str());
            }
            else
            {
                gameName = v.second.get<std::string>("fields.game_name");
            }
			if (mGames[i]) {
				if (mGames[i]->getHash() == v.second.get<std::string>("fields.game_crc")) {
					text = "\uf1c0 " + gameName;
				}
				else if (!getCoreInfo(v.second.get<std::string>("fields.core_name")).first.empty()) {
					text = "\uf1c1 " + gameName;
				} else
				{
					text = "\uf1c2 " + gameName;
				}
			} else {
				text = "\uf1c2 " + gameName;
			}
			ed = std::make_shared<TextComponent>(mWindow, text, mMenuTheme->menuText.font, mMenuTheme->menuText.color, ALIGN_LEFT);
			row.addElement(ed, true);
			row.makeAcceptInputHandler([this] { launch();});
			addRow(row);
			i++;
		}
		populateGridMeta(0);
		mList->setCursorChangedCallback([this](CursorState state) { (void)state; populateGridMeta(mList->getCursor()); });
		mList->setFocusLostCallback([this]{
		    mMetaTextUsername->setText("");
            mMetaTextCountry->setText("");
            mMetaTextRomHash->setText("");
            mMetaTextRomFile->setText("");
            mMetaTextCore->setText("");
            mMetaTextCoreVer->setText("");
            mMetaTextLatency->setText("");
            mMetaTextRAVer->setText("");
            mMetaTextHostArch->setText("");
            mMetaTextCanJoin->setText("");
		});
		mList->setFocusGainedCallback([this]{populateGridMeta(mList->getCursor());});

		mHandle = new boost::thread(boost::bind(&GuiNetPlay::pingLobby, this));

		mGrid.moveCursor(Vector2i(0, -1));
	} else {
		auto text = std::make_shared<TextComponent>(mWindow, _("NO GAMES OR NO CONNECTION"), mMenuTheme->menuText.font, mMenuTheme->menuText.color, ALIGN_CENTER);
		mGrid.setEntry(text, Vector2i(0, 1), true);
	}
	mLobbyChecked = false;
}

//called when changing cursor in mList
void GuiNetPlay::populateGridMeta(int i)
{

	bool hashMatch = false;
	bool coreVerMatch,coreMatch;

	if (mGames[i]) {
		hashMatch = mGames[i]->getHash() == mRooms[i].second.get<std::string>("fields.game_crc");
	}
	std::pair<std::string, std::string> CoreInfo = getCoreInfo(mRooms[i].second.get<std::string>("fields.core_name"));
	coreVerMatch = CoreInfo.second == mRooms[i].second.get<std::string>("fields.core_version");
	coreMatch = !CoreInfo.first.empty();

    std::string username = mRooms[i].second.get<std::string>("fields.username", "N/A");
	std::string frontend = mRooms[i].second.get<std::string>("fields.frontend");

    if (std::regex_search(username, std::regex("@RECALBOX")))
    {
        username = std::regex_replace(username, std::regex("@RECALBOX"), " \uF200");
    }
    else if (std::regex_search(frontend, std::regex("@RECALBOX")))
    {
        username = username + " \uF200";
		frontend = std::regex_replace(frontend, std::regex("@RECALBOX"), "");
    }

    mMetaTextUsername->setText(username);
    mMetaTextCountry->setText(mRooms[i].second.get<std::string>("fields.country", "N/A"));
    if (hashMatch) {
    	mMetaTextRomHash->setText("\uf1c0 " + _("Match"));
    } else {
        mMetaTextRomHash->setText("\uf1c2 " + _("No Match"));
    }
	if (mGames[i]) {
        mMetaTextRomFile->setText("\uf1c0 " + _("Match"));
	} else {
        mMetaTextRomFile->setText("\uf1c2 " + _("No Match"));
	}
	if (coreMatch) {
        mMetaTextCore->setText("\uf1c0 " + mRooms[i].second.get<std::string>("fields.core_name"));
	} else {
        mMetaTextCore->setText("\uf1c2 " + mRooms[i].second.get<std::string>("fields.core_name"));
	}
	if (coreVerMatch) {
        mMetaTextCoreVer->setText("\uf1c0 " + mRooms[i].second.get<std::string>("fields.core_version", "N/A"));
	} else {
        mMetaTextCoreVer->setText("\uf1c2 " + mRooms[i].second.get<std::string>("fields.core_version", "N/A"));
	}
    mMetaTextLatency->setText(mPings[i]);
    mMetaTextRAVer->setText(mRooms[i].second.get<std::string>("fields.retroarch_version", "N/A"));
    mMetaTextHostArch->setText(frontend);
	if (mGames[i]) {
		if (hashMatch && coreMatch) {
            mMetaTextCanJoin->setText("\uf1c0 " + _("Rom, hash and core match"));
            mMetaTextCanJoin->setColor(0x26B14AFF);
		}
		else if (coreMatch) {
            mMetaTextCanJoin->setText("\uf1c1 " + _("Rom and core match"));
            mMetaTextCanJoin->setColor(0x36A9E0FF);
		} else
		{
			mMetaTextCanJoin->setText("\uf1c2 " + _("No core match"));
			mMetaTextCanJoin->setColor(0xDC1F26FF);
		}
	} else {
        mMetaTextCanJoin->setText("\uf1c2 " + _("No rom match"));
        mMetaTextCanJoin->setColor(0xDC1F26FF);
	}
}

void GuiNetPlay::launch()
{
	if (mGames[mList->getCursor()]) {
        Vector3f target(Renderer::getScreenWidth() / 2.0f, Renderer::getScreenHeight() / 2.0f, 0);
        int index = mList->getCursor();
        std::string core = getCoreInfo(mRooms[index].second.get<std::string>("fields.core_name")).first;
        if (!core.empty())
        {
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
}

FileData* GuiNetPlay::findGame(std::string gameNameOrHash)
{
	for (auto tmp : SystemData::sSystemVector)
	{
		if (RecalboxConf::getInstance()->isInList("global.netplay.systems", tmp->getName()))
		{
			FileData* result = tmp->getRootFolder()->LookupGame(gameNameOrHash, FileData::SearchAttributes::ByName | FileData::SearchAttributes::ByHash);
			if (result != nullptr)
			{
				return result;
			}
		}
	}
	return nullptr;
}

std::pair<std::string, std::string> GuiNetPlay::getCoreInfo(const std::string &name) {
    boost::regex validLine("^(?<key>[^;|#].*?);(?<val>.*?)$");
    std::pair<std::string, std::string> result;
    result.first.clear();
    result.second.clear();
    std::map<std::string, std::string> coreMap;
    std::string line;
    std::string filePath = "/recalbox/share/system/configs/retroarch.corenames";
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
        if (!result.first.empty())
        {
	        result.second = s;
        }
        else {
        	result.first = s;
        }
        return result;
    }
    return result;
}

void GuiNetPlay::pingLobby()
{
	for (int i=0; i < (int)mRooms.size(); i++ )
	{
		std::string ip = mRooms[i].second.get<std::string>("fields.ip");
		mPings[i] = pingHost(ip);
	}
}

std::string GuiNetPlay::pingHost(const std::string& ip)
{
    std::pair<std::string, int> ping = RecalboxSystem::getInstance()->execute("ping -c 1 -w 1 " + ip + " | grep \"min/avg/max\" | cut -d '/' -f 5");
    if (!ping.first.empty()) {
        float latency = strtof(ping.first.c_str(), 0);
        if (latency <=80) {
            return "\uF1c8 " + _("good") + " (" + std::to_string((int)latency) + "ms)";
        } else if (latency <= 150) {
            return "\uF1c7 " + _("medium") + " (" + std::to_string((int)latency) + "ms)";
        } else {
            return "\uF1c6 " + _("bad") + " (" + std::to_string((int)latency) + "ms)";
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
	const float width = Renderer::getScreenWidth() * 0.9f;
	setSize(width, height);
}

void GuiNetPlay::onSizeChanged()
{
	mBackground.fitTo(mSize, Vector3f::Zero(), Vector2f(-32, -32));

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

void GuiNetPlay::render(const Transform4x4f &parentTrans)
{
	Transform4x4f trans = parentTrans * getTransform();

	renderChildren(trans);

	Renderer::setMatrix(trans);
	Renderer::drawRect(0.f, 0.f, mSize.x(), mSize.y(), 0x00000011);

	if (mLoading)
		mBusyAnim.render(trans);

}