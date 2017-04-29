#include <Window.h>
#include "FileData.h"
#include <boost/thread/thread.hpp>

class CommandThread {
public:
	CommandThread(Window* window);
	~CommandThread();
	void run();
	FileData* findRecursive(const std::vector<FileData*> gameFolder, const std::string& gameName, const std::string& relativePath = "");
	void runGame (FileData* game);
private:
	Window* mWindow;
	bool mRunning;
	boost::thread* mThreadHandle;
};
