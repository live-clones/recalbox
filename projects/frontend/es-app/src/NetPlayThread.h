//
// Created by xizor on 06/06/18.
//

#ifndef EMULATIONSTATION_ALL_NETPLAYTHREAD_H
#define EMULATIONSTATION_ALL_NETPLAYTHREAD_H

#include <Window.h>
#include <boost/thread/thread.hpp>

class NetPlayThread {
public:
	NetPlayThread(Window* window);
	~NetPlayThread();
	void run();
private:
	Window* mWindow;
	bool mRunning;
	boost::thread* mThreadHandle;
};

#endif //EMULATIONSTATION_ALL_NETPLAYTHREAD_H
