
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
	}
}

FileData* CommandThread::findRecursive(const std::vector<FileData*> gameFolder, const std::string& gameName, const std::string& relativePath) {
	// Recursively look for the game in subfolders too
	for (auto game = gameFolder.begin(); game != gameFolder.end(); ++game) {
		std::string gameAndPath;
		if (relativePath.empty()) {
			gameAndPath = (*game)->getPath().filename().c_str();
		} else {
			gameAndPath = relativePath + '/' + (*game)->getPath().filename().c_str();
		}
		LOG(LogInfo) << "Checking game " << gameName << " in path: " << gameAndPath;
		if ((*game)->getType() == FOLDER) {
			FileData* foundGame = findRecursive((*game)->getChildren(), gameName, gameAndPath);
			if ( foundGame != NULL ) {
				LOG(LogInfo) << "Game found !";
				return foundGame;
			}
		}
		if ((*game)->getType() == GAME and gameAndPath == gameName) {
			LOG(LogInfo) << "Game found !";
			return *game;
		}
	}
	return NULL;
}

void CommandThread::runGame (FileData* game) {
	ViewController *view = ViewController::get();
	Eigen::Vector3f target(Renderer::getScreenWidth() / 2.0f, Renderer::getScreenHeight() / 2.0f, 0);
	mWindow->doWake();
	view->launch(game, target);
}
