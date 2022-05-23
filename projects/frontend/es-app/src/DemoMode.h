#pragma once

#include <vector>
#include <random>
#include "RecalboxConf.h"
#include "systems/SystemData.h"
#include <games/GameRandomSelector.h>
#include <utils/storage/HashMap.h>

class SystemManager;

class DemoMode
{
  private:
    //! Window
    WindowManager& mWindow;
    //! SystemManager instance
    SystemManager& mSystemManager;

    //! Game selector
    GameRandomSelector mGameSelector;

    //! Out-screen duration
    int mInfoScreenDuration;

  public:
    //! Default constructor
    explicit DemoMode(WindowManager& window, SystemManager& systemManager);

    /*!
     * @brief Return true is the uer set the screensaver to "demo"
     * @return True if demo screensaver is on
     */
    static bool hasDemoMode();

    /*!
     * @brief Run random games until user interaction
     */
    void runDemo();
};

