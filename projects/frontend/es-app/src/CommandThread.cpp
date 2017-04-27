
#include "CommandThread.h"
#include "guis/GuiMsgBox.h"
#include "SystemData.h"
#include "Log.h"
#include "views/ViewController.h"
#include <boost/asio.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
using boost::asio::ip::udp;

const int max_length = 2048;


CommandThread::CommandThread(Window* window) : mWindow(window) {
	mRunning = true;
	mThreadHandle = new boost::thread(boost::bind(&CommandThread::run, this));
}

CommandThread::~CommandThread() {
	mThreadHandle->join();
}

void CommandThread::run() {
	boost::this_thread::sleep(boost::posix_time::seconds(5));

	LOG(LogInfo) << "CommandThread started";

	boost::asio::io_service io_service;
	udp::socket sock(io_service, udp::endpoint(udp::v4(), 1337));

	char buf[max_length];
	while (mRunning) {
		udp::endpoint sender_endpoint;
		size_t length = sock.receive_from(boost::asio::buffer(buf, max_length), sender_endpoint);
		if (length <= 0) {
			continue;
		}
		buf[length + 1] = '\0';

		std::string str(buf, length);
		std::vector<std::string> tokens;
		boost::split(tokens, str, boost::is_from_range('|', '|'), boost::token_compress_on);

		// Check that the command is valid. Easy way as there is just 1 for now
		if (tokens[0] != "START") {
			LOG(LogError) << "Wrong network command " << tokens[0];
			continue;
		}
		
		SystemData *system = NULL;
		for (auto tmp = SystemData::sSystemVector.begin(); tmp != SystemData::sSystemVector.end(); tmp ++) {
			if ((*tmp)->getName() == tokens[1]) {
				system = *tmp;
				break;
			}
		}

		// The system is not a valid one
		if (system == NULL) {
			LOG(LogError) << "Invalid system on network command: " << tokens[1];
			continue;
		}

		std::vector<FileData*> games = system->getRootFolder()->getChildren();
		FileData* result = findRecursive(games, tokens[2]);
		if (result != NULL) {
			LOG(LogInfo) << "Starting game " << tokens[2] << " for system " << tokens[1];
			runGame(result);
		} else {
			LOG(LogError) << "Couldn't find game " << tokens[2] << " for system " << tokens[1];
		}
/*
		ViewController *view = ViewController::get();
		bool found = false;
		Eigen::Vector3f target(Renderer::getScreenWidth() / 2.0f, Renderer::getScreenHeight() / 2.0f, 0);
		LOG(LogInfo) << "Looking for game: " << tokens[2];
		std::vector<FileData*> games = system->getRootFolder()->getChildren();
		for (auto game = games.begin(); game != games.end(); ++game) {
			LOG(LogError) << "Checking: " << (*game)->getPath().filename();
			if ((*game)->getType() == FOLDER) {
				LOG(LogError) << "Skipping folder: " << (*game)->getPath().filename();
				continue;
			}
			if ((*game)->getType() == GAME and (*game)->getPath().filename() == tokens[2]) {
				LOG(LogInfo) << "Game found !";
				found = true;
				//mWindow->wakeUp();
				view->launch(*game, target);
				break;
			}
		}
		
		if (!found) {
			LOG(LogError) << "Could not find the game";
		}
//*/
	}
}

FileData* CommandThread::findRecursive(std::vector<FileData*> gameFolder, std::string gameName) {
	for (auto game = gameFolder.begin(); game != gameFolder.end(); ++game) {
		LOG(LogInfo) << "Checking: " << (*game)->getPath().filename();
		if ((*game)->getType() == FOLDER) {
			FileData* foundGame = findRecursive((*game)->getChildren(), gameName);
			if ( foundGame != NULL ) {
				LOG(LogInfo) << "Game found !";
				return foundGame;
			}
		}
		if ((*game)->getType() == GAME and (*game)->getPath().filename() == gameName) {
			LOG(LogInfo) << "Game found !";
			return *game;
		}
	}
	return NULL;
}

void CommandThread::runGame (FileData* game) {
	ViewController *view = ViewController::get();
	Eigen::Vector3f target(Renderer::getScreenWidth() / 2.0f, Renderer::getScreenHeight() / 2.0f, 0);
	//mWindow->wakeUp();
	view->launch(game, target);
}
