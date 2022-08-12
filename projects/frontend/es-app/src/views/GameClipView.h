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
#include <utils/sync/SyncMessageSender.h>

class GameClipView : public Gui
                   , public ISyncMessageReceiver<void>
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
        [[nodiscard]] bool ApplyFilter(const FileData& file) const override
        {
          return  !file.Metadata().VideoAsString().empty() && file.IsDisplayable();
        }
    } mFilter;

    //! Synchronous event
    SyncMessageSender<void> mEvent;

    //! Window
    WindowManager& mWindow;

    SystemManager& mSystemManager;

    GameRandomSelector mGameRandomSelector;

    static constexpr int MAX_HISTORY = 10;
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
    void ReceiveSyncMessage() override;

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