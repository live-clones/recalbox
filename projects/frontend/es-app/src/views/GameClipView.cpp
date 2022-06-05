//
// Created by gugue_U on 17/10/2020
//

#include <VideoEngine.h>
#include <recalbox/RecalboxSystem.h>
#include "RecalboxConf.h"
#include "GameClipView.h"
#include "utils/Log.h"
#include <views/ViewController.h>
#include "utils/locale/LocaleHelper.h"
#include <guis/GuiInfoPopup.h>
#include <usernotifications/NotificationManager.h>

GameClipView::GameClipView(WindowManager& window, SystemManager& systemManager)
  : Gui(window)
  , mEvent(this)
  , mWindow(window)
  , mSystemManager(systemManager)
  , mGameRandomSelector(systemManager, &mFilter)
  , mHistoryPosition(0)
  , mDirection(Direction::Next)
  , mGame(nullptr)
  , mGameClipContainer(window)
  , mNoVideoContainer(window)
  , systemIndex(-1)
  , mVideoDuration(0)
{

  if (VideoEngine::IsInstantiated())
    VideoEngine::Instance().StopVideo(true);
}

GameClipView::~GameClipView()
{
  mHistory.clear();
}

void GameClipView::InsertIntoHistory(FileData* game)
{
  mHistory.insert(mHistory.begin(), game);
  if (mHistory.size() == MAX_HISTORY + 1)
  {
    mHistory.erase(mHistory.begin() + MAX_HISTORY);
  }
}

void GameClipView::GetGame()
{
  if (Direction::Next == mDirection)
  {
    GetNextGame();
  }
  else
  {
    GetPreviousGame();
  }
}

void GameClipView::GetNextGame()
{
  if (mHistoryPosition == 0)
  {
    mGame = mGameRandomSelector.NextGame();

  } else
  {
    mHistoryPosition--;
    // security
    if (mHistoryPosition < 0)
    {
      mHistoryPosition = 0;
    }
    mGame = mHistory.at(mHistoryPosition);
  }
}

void GameClipView::GetPreviousGame()
{
  mHistoryPosition++;
  //security
  if (mHistoryPosition > (int) mHistory.size())
  {
    mHistoryPosition = (int) mHistory.size();
  }
  mGame = mHistory.at(mHistoryPosition);
}

void GameClipView::Render(const Transform4x4f& parentTrans)
{
  if (mState == State::LaunchGame)
  {
    ViewController::Instance().Launch(mGame, GameLinkedData(), Vector3f());
    mState = State::GoToSystem;
  }

  if (mState == State::GoToSystem)
  {
    ViewController::Instance().selectGamelistAndCursor(mGame);
    mState = State::Quit;
  }

  if (mState == State::Quit)
  {
    mEvent.Call(); // Asynchronous delete!
    mState = State::Terminated;
  }

  // waiting to be destroy
  if (mState == State::Terminated)
  {
    return;
  }

  if (mState == State::NoGameSelected && !mGameRandomSelector.HasValidSystems())
  {
    mState = State::EmptyPlayList;
    updateHelpPrompts();
    mTimer.Initialize(0);
    return;
  }

  if (mState == State::EmptyPlayList)
  {
    mNoVideoContainer.Render(parentTrans);
    if(mTimer.GetMilliSeconds() > 60000)
      mState = State::Quit;
  }

  if (mState == State::NoGameSelected)
  {
    StartGameClip();
    mTimer.Initialize(0);
    mState = State::InitPlaying;
    updateHelpPrompts();
  }

  else if (mState == State::InitPlaying)
  {
    // when videoEngine cannot play video file
    if (!mGame->Metadata().Video().Exists() ||  mTimer.GetMilliSeconds() > 3000)
    {
      { LOG(LogDebug) << "[GameClip] Video do not start for game: " << mGame->Metadata().VideoAsString(); }
      VideoEngine::Instance().StopVideo(false);
      mState = State::NoGameSelected;
      return;
    }
    if (VideoEngine::Instance().IsPlaying())
    {
      NotificationManager::Instance().Notify(*mGame, Notification::StartGameClip);
      mVideoDuration = VideoEngine::Instance().GetVideoDurationMs();
      if (Direction::Next == mDirection && mHistoryPosition == 0)
      {
        mState = State::SetInHistory;
      }
      else
      {
        mState = State::Playing;
      }
    }
  }

  else if (mState == State::SetInHistory)
  {
    InsertIntoHistory(mGame);
    mState = State::Playing;
  }

  else if (mState == State::Playing)
  {
    if (mTimer.GetMilliSeconds() > mVideoDuration || mTimer.GetMilliSeconds() > 35000)
    {
      ChangeGameClip(Direction::Next);
      return;
    }
  }
  mGameClipContainer.Render(parentTrans);
}

bool GameClipView::ProcessInput(const InputCompactEvent& event)
{
  if (event.CancelPressed())
  {
    mState = State::Quit;
    StopGameClipView();
  }

  if (State::EmptyPlayList == mState || State::Playing != mState || mTimer.GetMilliSeconds() < 1000) return true;

  // GO TO GAME LIST -  SELECT
  if (event.SelectPressed())
  {
    mState = State::GoToSystem;
    StopGameClipView();
    return true;
  }

  // RUN GAME - START
  if (event.StartPressed())
  {
    StopGameClipView();
    mState = State::LaunchGame;
    return true;
  }

  // NEXT GAMECLIP  - RIGHT
  if (event.RightPressed())
  {
    ChangeGameClip(Direction::Next);
    return true;
  }
  // PREVIOUS GAMECLIP - LEFT
  if (event.LeftPressed())
  {
    ChangeGameClip(Direction::Previous);
    return true;
  }

  // TOGGLE FAVORITES - Y
  if (event.YPressed())
  {
    if (mGame->IsGame() && mGame->System().HasFavoritesInTheme())
    {
      MetadataDescriptor& md = mGame->Metadata();
      SystemData* favoriteSystem = mSystemManager.FavoriteSystem();
      md.SetFavorite(!md.Favorite());

      if (favoriteSystem != nullptr)
      {
        if (md.Favorite())
        {
          favoriteSystem->GetFavoriteRoot().AddChild(mGame, false);
        }
        else
        {
          favoriteSystem->GetFavoriteRoot().RemoveChild(mGame);
        }

        ViewController::Instance().setInvalidGamesList(&mGame->System());
        ViewController::Instance().setInvalidGamesList(favoriteSystem);
      }
      ViewController::Instance().getSystemListView().manageFavorite();

      int popupDuration = RecalboxConf::Instance().GetPopupHelp();
      std::string message = md.Favorite() ? _("Added to favorites") : _("Removed from favorites");
      mWindow.InfoPopupAdd(new GuiInfoPopup(mWindow, message + ":\n" + mGame->DisplayName(), popupDuration,
                                            GuiInfoPopupBase::PopupType::None));

        updateHelpPrompts();
    }
    return true;
  }
  return true;
}

void GameClipView::StartGameClip()
{
  GetGame();
  mGameClipContainer.setGameInfo(mGame);
}

void GameClipView::StopGameClipView()
{
  NotificationManager::Instance().Notify(Notification::StopGameClip);
  if (State::EmptyPlayList != mState)
    VideoEngine::Instance().StopVideo(true);
  mGameClipContainer.CleanVideo();
}

void GameClipView::ChangeGameClip(Direction direction)
{
  if (direction == Direction::Previous && mHistoryPosition >= (int) mHistory.size() - 1)
    return;

  mDirection = direction;
  mState = State::NoGameSelected;
  VideoEngine::Instance().StopVideo(true);
}

bool GameClipView::getHelpPrompts(Help& help)
{
  switch(mState)
  {
    case State::NoGameSelected :
    case State::GoToSystem:
    case State::LaunchGame:
    case State::Terminated :
    case State::Quit : break;
    case State::EmptyPlayList: return mNoVideoContainer.getHelpPrompts(help);
    case State::InitPlaying:
    case State::Playing:
    case State::SetInHistory: return mGameClipContainer.getHelpPrompts(help);
  }
  return true;
}

void GameClipView::ReceiveSyncCallback(const SDL_Event& event)
{
  (void)event;
  ViewController::Instance().quitGameClipView();
}

