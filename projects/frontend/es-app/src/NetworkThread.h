#pragma once

#include "Window.h"
#include <boost/thread.hpp>

class NetworkThread {
public:
    explicit NetworkThread(Window * window);
    virtual ~NetworkThread();

private:
    Window* mWindow;
    bool mRunning;
    boost::thread * mThreadHandle;
    void run();
};


