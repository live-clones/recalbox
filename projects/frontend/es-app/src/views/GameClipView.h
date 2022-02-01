//
// Created by gugue_U on 17/10/2020
//
#pragma once

#include <vector>
#include <games/FileData.h>
#include <systems/PlatformId.h>
#include <systems/SystemManager.h>
#include <utils/datetime/HighResolutionTimer.h>
#include <components/VideoComponent.h>
#include <components/ScrollableContainer.h>
#include <components/GameClipContainer.h>
#include <components/GameClipNoVideoContainer.h>
#include "RecalboxConf.h"
#include "games/GameRandomSelector.h"

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
    class Filter : public IFilter
    {
      public:
        bool ApplyFilter(const FileData& file) const override
        {
          bool showIfHidden = !file.Metadata().Hidden() || RecalboxConf::Instance().GetShowHidden();
          return  !file.Metadata().VideoAsString().empty() && showIfHidden;
        }
    } mFilter;

    //! Synchronous event
    SyncronousEvent mEvent;

    //! Window
    WindowManager& mWindow;

    SystemManager& mSystemManager;

    GameRandomSelector mGameRandomSelector;

    static constexpr int MAX_HISTORY = 60;
    int mHistoryPosition;
    std::vector<FileData*> mHistory;
    Direction mDirection;

    FileData* mGame;

    HighResolutionTimer mTimer;

    State mState = State::NoGameSelected;

    GameClipContainer mGameClipContainer;

    GameClipNoVideoContainer mNoVideoContainer;

    int systemIndex;

    int mVideoDuration;

    int GetFirstOccurenceInHistory(FileData* game);

    void InsertIntoHistory(FileData* game);

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