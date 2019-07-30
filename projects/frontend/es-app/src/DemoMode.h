#pragma once

#include <vector>
#include <random>
#include "Settings.h"
#include "RecalboxConf.h"
#include "SystemData.h"
#include "FileData.h"
#include "PlatformId.h"

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
    int mDefaultDuration;

    //! Out-screen duration
    int mInfoScreenDuration;

    //! List of system from which to get random games
    std::vector<SystemData*> mDemoSystems;

    //! Duration of game session per system
    std::vector<int> mDurations;

    //! Random device to seed random generator
    std::random_device mRandomDevice;
    //! Random generator
    std::mt19937 mRandomGenerator;
    //! Random repartition (system)
    std::uniform_int_distribution<int> mSystemRandomizer;
    //! Random repartition (game)
    std::uniform_int_distribution<int> mGameRandomizer;

    //! History deepness
    static constexpr int MAX_HISTORY = 32;

    //! Game History
    int mGameHistories[(int)PlatformIds::PlatformId::PLATFORM_COUNT][MAX_HISTORY] = { { 0 } };

    //! Previously selected system
    int mSystemHistory[(int)PlatformIds::PlatformId::PLATFORM_COUNT] = { 0 };

    //! Secondary seed
    int mSeed;

    void init();

    /*!
     * @brief Check if an item is in its history records
     * @param item item to seek for
     * @param history history records
     * @param maxitems Maximum items
     * @return True if the item is in the history records, false otherwise
     */
    static bool isInHistory(int item, const int history[], int maxitems);

    /*!
     * @brief Insert an item into its history records
     * @param item Item to insert
     * @param history History records
     */
    static void insertIntoHistory(int item, int history[]);

    /*!
     * @brief Get next random game
     * @param outputGame Selected game
     * @param outputDuration Session Duration
     * @return True if a game is available.
     */
    bool getRandomGame(FileData*& outputGame, int& outputDuration);

    /*!
     * @brief Get next system
     * @param outputSystemIndex Selected system index
     * @param outputDuration Session Duration
     * @return True if a system is available.
     */
    bool getRandomSystem(int& outputSystemIndex, int& outputDuration);

public:
    //! Default constructor
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

