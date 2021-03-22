//
// Created by gugue_U on 17/10/2020
//

#include <VideoEngine.h>
#include <recalbox/RecalboxSystem.h>
#include "RecalboxConf.h"
#include "GameClipView.h"
#include "utils/Log.h"
#include <views/ViewController.h>
#include <audio/AudioManager.h>
#include "utils/locale/LocaleHelper.h"
#include <usernotifications/NotificationManager.h>

GameClipView::GameClipView(WindowManager& window, SystemManager& systemManager)
  : Gui(window), mWindow(window), mSystemManager(systemManager), mRecalboxConf(RecalboxConf::Instance()),
    mRandomDevice(), mRandomGenerator(mRandomDevice()), mGameRandomizer(0, 1U << 30U), mGameClipContainer(window),
    mNoVideoContainer(window), systemIndex(-1), mVideoDuration(0)
{
  init();
}

GameClipView::~GameClipView()
{
  mDemoFiles.clear();
  mHistory.clear();
}

void GameClipView::init()
{
  class Filter : public IFilter
  {
    public:
      bool ApplyFilter(const FileData& file) const override
      {
        bool hasVideo = !file.Metadata().VideoAsString().empty();
        bool videoExist = file.Metadata().Video().Exists();
        if (hasVideo && !videoExist) { LOG(LogDebug) << "[GameClip] No video found for : " << file.getDisplayName(); }
        return hasVideo && videoExist;
      }
  } videoFilter;

  // Build system list filtered by user config
  const std::vector<SystemData*>& allSystems = mSystemManager.GetAllSystemList();

  bool systemListExists = !mRecalboxConf.AsString("global.demo.systemlist").empty();
  for (int i = (int) allSystems.size(); --i >= 0;)
  {
    const std::string& name = allSystems[i]->getName();
    bool systemIsInIncludeList = !systemListExists || mRecalboxConf.isInList("global.demo.systemlist", name);
    if (systemIsInIncludeList)
    {
      FileData::List list = allSystems[i]->MasterRoot().getFilteredItemsRecursively(&videoFilter, false,
                                                                                    allSystems[i]->IncludeAdultGames());
      mDemoFiles.reserve(mDemoFiles.size() + list.size());
      mDemoFiles.insert(mDemoFiles.end(), list.begin(), list.end());
    }
  }

  // Second seed - must never be negative
  mSeed = ((int) (DateTime() - DateTime(1970, 1, 1)).TotalMilliseconds()) & 0x7FFFFFFF;
  VideoEngine::Instance().StopVideo(true);
}

int GameClipView::getFirstOccurenceInHistory(FileData* game)
{
  for (int i = 0; i < (int) mHistory.size(); ++i)
  {
    if (game == mHistory.at(i))
    {
      return i;
    }
  }
  return -1;
}

void GameClipView::insertIntoHistory(FileData* game)
{
  mHistory.insert(mHistory.begin(), game);
  if (mHistory.size() == MAX_HISTORY + 1)
  {
    mHistory.erase(mHistory.begin() + MAX_HISTORY);
  }
}

void GameClipView::getGame()
{
  if (Direction::Next == mDirection)
  {
    getNextGame();
  }
  else
  {
    getPreviousGame();
  }
}

void GameClipView::getNextGame()
{
  if (mHistoryPosition == 0)
  {
    int tryNum = 0;
    int maxPosition = -1;
    int finalIndex = 0;
    // if already in list, could try 10 times and return the oldest game in list
    while (tryNum <= 10)
    {
      int index = ((mGameRandomizer(mRandomGenerator) + mSeed) & 0x7FFFFFFF) % (int) mDemoFiles.size();
      mSeed++;
      int gamePosition = getFirstOccurenceInHistory(mDemoFiles[index]);
      if (gamePosition == -1)
      {
        finalIndex = index;
        mGame = mDemoFiles[finalIndex];
        return;
      }
      else if (gamePosition > maxPosition)
      {
        maxPosition = gamePosition;
        finalIndex = index;
      }
      tryNum++;
    }
    mGame = mDemoFiles[finalIndex];
  }
  else
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

void GameClipView::getPreviousGame()
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
  // waiting to be destroy
  if (mState == State::Terminated)
  {
    return;
  }

  if (mState == State::Quit)
  {
    quitGameClipView();
    mState = State::Terminated;
    return;
  }

  if (mState == State::NoGameSelected && mDemoFiles.empty())
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
    startGameClip();
    mTimer.Initialize(0);
    mState = State::InitPlaying;
    updateHelpPrompts();
  }

  else if (mState == State::InitPlaying)
  {
    // when videoEngine cannot play video file
    if (mTimer.GetMilliSeconds() > 5000)
    {
      { LOG(LogDebug) << "[GameClip] Video do not start for game: " << mGame->Metadata().VideoAsString(); }
      // remove game from list
      mDemoFiles.erase(mDemoFiles.begin() + mDemoFilesIndex);
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
    insertIntoHistory(mGame);
    mState = State::Playing;
  }

  else if (mState == State::Playing)
  {
    if (mTimer.GetMilliSeconds() > mVideoDuration || mTimer.GetMilliSeconds() > 35000)
    {
      changeGameClip(Direction::Next);
      return;
    }
  }
  mGameClipContainer.Render(parentTrans);
}

void GameClipView::changeGameClip(Direction direction)
{
  if (direction == Direction::Previous && mHistoryPosition >= (int) mHistory.size() - 1)
    return;

  mDirection = direction;
  mState = State::NoGameSelected;
  VideoEngine::Instance().StopVideo(true);
  NotificationManager::Instance().Notify(*mGame, Notification::StopGameClip);
}

bool GameClipView::ProcessInput(const InputCompactEvent& event)
{
  if (event.CancelPressed())
  {
    mState = State::Quit;
  }

  if (mState != State::Playing || mTimer.GetMilliSeconds() < 1000)
  {
    return true;
  }

  // GO TO GAME LIST -  SELECT
  if (event.SelectPressed())
  {
    ViewController::Instance().goToGameList(mGame);
    mState = State::Quit;
    return true;
  }

  // RUN GAME - START
  if (event.StartPressed())
  {
    Vector3f target(mGameClipContainer.getVideoCenter());
    ViewController::Instance().LaunchCheck(mGame, NetPlayData(), target);

    ViewController::Instance().goToGameList(mGame);
    mState = State::Quit;
    return true;
  }

  // NEXT GAMECLIP  - RIGHT
  if (event.RightPressed())
  {
    changeGameClip(Direction::Next);
    return true;
  }
  // PREVIOUS GAMECLIP - LEFT
  if (event.LeftPressed())
  {
    changeGameClip(Direction::Previous);
    return true;
  }

  // TOGGLE FAVORITES - Y
  if (event.YPressed())
  {
    if (mGame->isGame() && mGame->getSystem()->getHasFavoritesInTheme())
    {
      MetadataDescriptor& md = mGame->Metadata();
      SystemData* favoriteSystem = mSystemManager.FavoriteSystem();
      md.SetFavorite(!md.Favorite());

      if (favoriteSystem != nullptr)
      {
        if (md.Favorite())
        {
          favoriteSystem->GetFavoriteRoot().addChild(mGame, false);
        }
        else
        {
          favoriteSystem->GetFavoriteRoot().removeChild(mGame);
        }

        ViewController::Instance().setInvalidGamesList(mGame->getSystem());
        ViewController::Instance().setInvalidGamesList(favoriteSystem);
      }
      ViewController::Instance().getSystemListView().manageFavorite();

      int popupDuration = RecalboxConf::Instance().GetPopupHelp();
      std::string message = md.Favorite() ? _("Added to favorites") : _("Removed from favorites");
      mWindow.InfoPopupAdd(new GuiInfoPopup(mWindow, message + ":\n" + mGame->getDisplayName(), popupDuration,
                                            GuiInfoPopup::PopupType::None));

        updateHelpPrompts();
    }
    return true;
  }

  return true;
}

void GameClipView::startGameClip()
{
  getGame();
  mGameClipContainer.setGameInfo(mGame);
}

void GameClipView::quitGameClipView()
{
  if (!mDemoFiles.empty())
  {
    VideoEngine::Instance().StopVideo(true);
    NotificationManager::Instance().Notify(*mGame, Notification::StopGameClip);
  }

  ViewController::Instance().quitGameClipView();
}

bool GameClipView::getHelpPrompts(Help& help)
{
  switch(mState)
  {
    case State::NoGameSelected :
    case State::Terminated :
    case State::Quit : break;
    case State::EmptyPlayList: return mNoVideoContainer.getHelpPrompts(help);
    case State::InitPlaying:
    case State::Playing:
    case State::SetInHistory: return mGameClipContainer.getHelpPrompts(help);

  }
  return true;
}