
#include "CommandThread.h"
#include "guis/GuiMsgBox.h"
#include "SystemData.h"
#include "Log.h"
#include "views/ViewController.h"
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
using boost::asio::ip::udp;

const int max_length = 2048;


CommandThread::CommandThread(Window* window)
  : mWindow(window),
    mRunning(true),
    mIOService(),
    mSocket(mIOService, udp::endpoint(udp::v4(), 1337))
{
	mThreadHandle = new boost::thread(boost::bind(&CommandThread::run, this));
}

CommandThread::~CommandThread()
{
	mRunning = false;
	mSocket.close();
	mThreadHandle->interrupt();
}

void CommandThread::run() {
	boost::this_thread::sleep(boost::posix_time::seconds(5));

	LOG(LogInfo) << "CommandThread started";

	char buf[max_length];
	while (mRunning)
	{
		udp::endpoint sender_endpoint;
		boost::system::error_code ec;
		size_t length = mSocket.receive_from(boost::asio::buffer(buf, max_length), sender_endpoint, 0, ec);
		if (ec.value() != 0) break;
		if (length <= 0) continue;
		buf[length + 1] = '\0';

		std::string str(buf, length);
		std::vector<std::string> tokens;
		boost::split(tokens, str, boost::is_from_range('|', '|'), boost::token_compress_on);

		// Check that the command is valid. Easy way as there is just 1 for now
		if (tokens[0] != "START") {
			LOG(LogError) << "Wrong network command " << tokens[0];
			continue;
		}
		
		SystemData *system = nullptr;
		for (auto tmp = SystemData::sSystemVector.begin(); tmp != SystemData::sSystemVector.end(); tmp ++) {
			if ((*tmp)->getName() == tokens[1]) {
				system = *tmp;
				break;
			}
		}

		// The system is not a valid one
		if (system == nullptr)
		{
			LOG(LogError) << "Invalid system on network command: " << tokens[1];
			continue;
		}

		FileData* result = system->getRootFolder()->LookupGame(tokens[2], FileData::SearchAttributes::ByNameWithExt);
		if (result != nullptr)
		{
			LOG(LogInfo) << "Starting game " << tokens[2] << " for system " << tokens[1];
			runGame(result);
		}
		else
		{
			LOG(LogError) << "Couldn't find game " << tokens[2] << " for system " << tokens[1];
		}
	}
}

void CommandThread::runGame (FileData* game)
{
	ViewController *view = ViewController::get();
	Vector3f target((float)Renderer::getScreenWidth() / 2.0f, (float)Renderer::getScreenHeight() / 2.0f, 0);
	mWindow->doWake();
	view->launch(game, target);
}
