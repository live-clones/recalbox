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
#include <components/VideoComponent.h>
#include <components/ScrollableContainer.h>
#include <components/GameClipContainer.h>
#include <components/GameClipNoVideoContainer.h>
#include "RecalboxConf.h"

class GameClipView : public Gui, public ISynchronousEvent
{
    enum class State
    {
        NoGameSelected,
        EmptyPlayList,
        InitPlaying,
        SetInHistory,
        Playing,
        LaunchGame,
        GoToSystem,
        Quit,
        Terminated,
    };

    enum class Direction
    {
        Next,
        Previous,
    };

  private:
    //! Synchronous event
    SyncronousEvent mEvent;

    //! Window
    WindowManager& mWindow;

    SystemManager& mSystemManager;

    RecalboxConf& mRecalboxConf;

    std::vector<FileData*> mDemoFiles;
    int mDemoFilesIndex = 0;

    static constexpr int MAX_HISTORY = 60;
    int mHistoryPosition = 0;
    std::vector<FileData*> mHistory;
    //FileData *mHistory[MAX_HISTORY]{};
    Direction mDirection = Direction::Next;

    //! Random device to seed random generator
    std::random_device mRandomDevice;
    //! Random generator
    std::mt19937 mRandomGenerator;
    //! Random repartition (game)
    std::uniform_int_distribution<int> mGameRandomizer;

    FileData* mGame {};

    HighResolutionTimer mTimer;

    State mState = State::NoGameSelected;

    GameClipContainer mGameClipContainer;

    GameClipNoVideoContainer mNoVideoContainer;

    int systemIndex;

    int mSeed {};

    int mVideoDuration;

    int GetFirstOccurenceInHistory(FileData* game);

    void InsertIntoHistory(FileData* game);

    void Initialize();

    void GetGame();

    void GetNextGame();

    void GetPreviousGame();

    void StartGameClip();

    void StopGameClipView();

    void ChangeGameClip(Direction direction);

    /*
     * Synchronous event
     */

    /*!
     * @brief Receive SDL event from the main thread
     * @param event SDL event
     */
    void ReceiveSyncCallback(const SDL_Event& event) override;

  public:
    static const char* getName()
    { return "gameclip"; }

    static bool IsGameClipEnabled()
    { return RecalboxConf::Instance().GetScreenSaverType() == "gameclip"; }

    //! Default constructor
    explicit GameClipView(WindowManager& window, SystemManager& systemManager);

    ~GameClipView() override;

    void Render(const Transform4x4f& parentTrans) override;

    bool ProcessInput(const InputCompactEvent& event) override;

    bool getHelpPrompts(Help& help) override;
};