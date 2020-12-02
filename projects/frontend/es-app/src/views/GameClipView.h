//
// Created by gugue_U on 17/10/2020
//

#pragma once

#include <vector>
#include <random>
#include <games/FileData.h>
#include <systems/PlatformId.h>
#include <systems/SystemManager.h>
#include <utils/datetime/HighResolutionTimer.h>
#include <components/TextComponent.h>
#include <components/VideoComponent.h>
#include <components/RatingComponent.h>
#include <components/ScrollableContainer.h>
#include <themes/ThemeExtras.h>
#include <components/DateTimeComponent.h>
#include <components/GameClipContainer.h>
#include "Settings.h"
#include "RecalboxConf.h"


class GameClipView : public Gui {
    enum class State {
        NoGameSelected,
        InitPlaying,
        SetInHistory,
        Playing,
    };

    enum class Direction {
        Next,
        Previous,
    };

private:

    //! Window
    WindowManager& mWindow;

    SystemManager &mSystemManager;

    RecalboxConf &mRecalboxConf;

    std::vector<FileData *> mDemoFiles;
    int mDemoFilesIndex =0;

    static constexpr int MAX_HISTORY = 60;
    int mHistoryPosition = 0;
    std::vector<FileData *> mHistory;
    //FileData *mHistory[MAX_HISTORY]{};
    Direction mDirection = Direction::Next;

    //! Random device to seed random generator
    std::random_device mRandomDevice;
    //! Random generator
    std::mt19937 mRandomGenerator;
    //! Random repartition (game)
    std::uniform_int_distribution<int> mGameRandomizer;

    FileData *mGame{};
    HighResolutionTimer mTimer;

    State mState = State::NoGameSelected;

    GameClipContainer mGameClipContainer;

    int systemIndex;

    int mSeed{};

    int getFirstOccurenceInHistory(FileData *game);

    void insertIntoHistory(FileData *game);

    void init();

    void getGame();

    void getNextGame();
    void getPreviousGame();

    void changeGameClip(Direction direction);

    void startGameClip();

    void checkEmptyDemoFiles();

    int mVideoDuration;

public:
    static const char *getName() { return "gameclip"; }
    static bool IsGameClipEnabled();

    //! Default constructor
    explicit GameClipView(WindowManager& window, SystemManager &systemManager);
    ~GameClipView() override;

    void Render(const Transform4x4f &parentTrans) override;

    bool ProcessInput(const InputCompactEvent &event) override;

    bool getHelpPrompts(Help &help) override;

    void quitGameClipView();

    void updateHelpPrompts();
    void ApplyHelpStyle() override;

};