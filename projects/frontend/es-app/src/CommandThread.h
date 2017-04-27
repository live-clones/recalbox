#include <Window.h>
#include "FileData.h"
#include <boost/thread/thread.hpp>

class CommandThread {
public:
	CommandThread(Window* window);
	~CommandThread();
	void run();
	FileData* findRecursive(std::vector<FileData*> gameFolder, std::string gameName);
	void runGame (FileData* game);
private:
	Window* mWindow;
	bool mRunning;
	boost::thread* mThreadHandle;
};
