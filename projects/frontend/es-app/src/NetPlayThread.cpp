//
// Created by xizor on 06/06/18.
//

#include "NetPlayThread.h"
#include <RecalboxConf.h>
#include <recalbox/RecalboxSystem.h>
#include <Log.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <Settings.h>
#include <views/ViewController.h>
#include <guis/GuiInfoPopup.h>
#include <Locale.h>

namespace json = boost::property_tree;

NetPlayThread::NetPlayThread(Window* window) : mWindow(window) {
	mRunning = true;
	mThreadHandle = new boost::thread(boost::bind(&NetPlayThread::run, this));
}

NetPlayThread::~NetPlayThread() {
	mThreadHandle->join();
}

void NetPlayThread::run() {
	boost::this_thread::sleep(boost::posix_time::seconds(3));

	LOG(LogInfo) << "NetPlayThread started";

	if (Settings::getInstance()->getInt("NetplayPopupTime") != 0) {
		std::vector<std::string> oldGames;
		std::string lobby = RecalboxConf::getInstance()->get("global.netplay.lobby");
		auto json_req = RecalboxSystem::getInstance()->execute("curl -s --connect-timeout 3 -m 3 " + lobby);
		if (json_req.second == 0) {
			json::ptree root;
			std::stringstream ss;
			ss << json_req.first;
			try {
				json::read_json(ss, root);
			}
			catch (const boost::property_tree::json_parser_error &e1) {
				LOG(LogInfo) << "NetPlayThread error";
			}

			for (json::ptree::value_type &array_element : root) {
				oldGames.push_back(array_element.second.get<std::string>("fields.username"));
			}
		}

		while (mRunning) {

			std::string lobby = RecalboxConf::getInstance()->get("global.netplay.lobby");
			auto json_req = RecalboxSystem::getInstance()->execute("curl -s --connect-timeout 3 -m 3 " + lobby);
			if (json_req.second == 0) {
				json::ptree root;
				std::stringstream ss;
				ss << json_req.first;
				try {
					json::read_json(ss, root);
				}
				catch (const boost::property_tree::json_parser_error &e1) {
					LOG(LogInfo) << "NetPlayThread error";
				}

				std::vector<std::string> newGames;

				for (json::ptree::value_type &array_element : root) {
					newGames.push_back(array_element.second.get<std::string>("fields.username"));
				}
				for (auto tmp : newGames) {
					std::vector<std::string>::const_iterator it;
					it = std::find(oldGames.begin(), oldGames.end(), tmp);
					if(it == oldGames.end()){
						if (tmp.find("RECALBOX") != std::string::npos) {
							int popupDuration = Settings::getInstance()->getInt("NetplayPopupTime");
							auto s = std::make_shared<GuiInfoPopup>(mWindow,
							                                        _("A Recalbox friend has started a Netplay game!"),
							                                        popupDuration, 20);
							mWindow->setInfoPopup(s);
							break;
						}
					}
				}
				oldGames = newGames;
			}
			boost::this_thread::sleep(boost::posix_time::seconds(30));
		}
	}
}