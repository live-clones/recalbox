#include <Window.h>
#include "FileData.h"
#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>

class CommandThread
	{
public:
	explicit CommandThread(Window* window);
	~CommandThread();
	void run();
	void runGame (FileData* game);

private:
	Window* mWindow;
	bool mRunning;
	boost::asio::io_service mIOService;
	boost::asio::ip::udp::socket mSocket;
	boost::thread* mThreadHandle;

};
