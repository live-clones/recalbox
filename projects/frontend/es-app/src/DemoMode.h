#pragma once

#include <vector>
#include <random>
#include "Settings.h"
#include "RecalboxConf.h"
#include "SystemData.h"
#include "FileData.h"

class DemoMode
{
  private:
    //! Window
    Window& mWindow;

    //! Settings direct access
    Settings& mSettings;

    //! Recalbox configuration direct access
    RecalboxConf& mRecalboxConf;

    //! Game session duration
    int       mDefaultDuration;

    //! List of system from which to get random games
    std::vector<SystemData*> mDemoSystems;

    //! Duration of game session per system
    std::vector<int> mDurations;

    //! Previously selected game path
    std::string mLastGamePath;

    //! Previously selected system
    std::string mLastSystem;

    //! Random device to seed random generator
    std::random_device mRandomRevice;
    std::mt19937 mRandomGenerator;

    /*!
     * @brief Get next random game
     * @return Game structure or nullptr if no game is available
     */
     /*!
      * @brief Get next random game
      * @param outputGame Choosen game
      * @param outputDuration Session Duration
      * @return True if a game is available.
      */
    bool getRandomGame(FileData*& outputGame, int& outputDuration);

    bool getRandomSystem(SystemData*& outputSystem, int& outputDuration);

public:
    DemoMode(Window& window);

    /*!
     * @brief Return true is the uer set the screensaver to "demo"
     * @return True if demo screensaver is on
     */
    bool hasDemoMode();

    /*!
     * @brief Run random games until user interaction
     */
    void runDemo();
};

