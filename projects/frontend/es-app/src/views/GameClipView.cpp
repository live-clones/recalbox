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
    systemIndex(-1), mVideoDuration(0)
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
        if (hasVideo && !videoExist)
        {
          LOG(LogDebug) << "No video found for : " << file.getDisplayName();
        }
        return hasVideo && videoExist;
      }
  } videoFilter;

  // Build system list filtered by user config
  const std::vector<SystemData*>& allSystems = mSystemManager.GetAllSystemList();
  std::vector<SystemData*> demoSystems;

  bool systemListExists = !mRecalboxConf.AsString("global.demo.systemlist").empty();
  for (int i = (int) allSystems.size(); --i >= 0;)
  {
    const std::string& name = allSystems[i]->getName();
    bool systemIsInIncludeList = !systemListExists || mRecalboxConf.isInList("global.demo.systemlist", name);
    bool hasVisibleGame = allSystems[i]->MasterRoot().hasVisibleGameWithVideo();
    if (systemIsInIncludeList && hasVisibleGame)
    {
      demoSystems.push_back(allSystems[i]);
    }
    else LOG(LogDebug) << "System NOT selected for game clip : " << allSystems[i]->getName() << " - isSelected: "
                       << (systemIsInIncludeList) << " - hasVisibleGame: " << (hasVisibleGame ? 1 : 0);
  }

  // Check if there is at least one system.
  // If not, get all system with no filtering
  if (demoSystems.empty())
    for (int i = (int) allSystems.size(); --i >= 0;)
    {
      demoSystems.push_back(allSystems[i]);
    }

  // Filter and insert items
  for (const SystemData* system : demoSystems)
  {
    FileData::List list = system->MasterRoot().getFilteredItemsRecursively(&videoFilter, false,
                                                                           system->IncludeAdultGames());
    mDemoFiles.reserve(mDemoFiles.size() + list.size());
    mDemoFiles.insert(mDemoFiles.end(), list.begin(), list.end());
  }

  checkEmptyDemoFiles();

  // Second seed - must never be negative
  mSeed = ((int) (DateTime() - DateTime(1970, 1, 1)).TotalMilliseconds()) & 0x7FFFFFFF;
}

void GameClipView::checkEmptyDemoFiles()
{
  if (mDemoFiles.empty())
  {
    int popupDuration = RecalboxConf::Instance().GetPopupHelp();
    std::string message = _("Gameclips cannot be played\n No video availabe for you selection");
    mWindow.InfoPopupAdd(new GuiInfoPopup(mWindow, message, popupDuration, GuiInfoPopup::PopupType::None));
    quitGameClipView();
  }
}

bool GameClipView::IsGameClipEnabled()
{
  return (RecalboxConf::Instance().GetScreenSaverType() == getName());
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
    mHistory.erase(mHistory.begin() + MAX_HISTORY + 1);
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
  if (mState == State::Quit)
  {
    quitGameClipView();
    return;
  }
  if (mState == State::NoGameSelected && VideoEngine::Instance().IsIdle())
  {
    startGameClip();
    mState = State::InitPlaying;
  }

  if (mState == State::InitPlaying)
  {
    if (VideoEngine::Instance().IsPlaying())
    {
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
    else if (VideoEngine::Instance().IsError())
    {
      LOG(LogDebug) << "Video in error for game" << mGame->Metadata().VideoAsString();
      // remove game from list
      mDemoFiles.erase(mDemoFiles.begin() + mDemoFilesIndex);
      checkEmptyDemoFiles();
      stopGameClip();
      mState = State::NoGameSelected;
      return;
    }
  }

  if (mState == State::SetInHistory)
  {
    insertIntoHistory(mGame);
    mState = State::Playing;
  }

  if (mState == State::Playing)
  {
    if (mTimer.GetMilliSeconds() > mVideoDuration)
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
  stopGameClip();
}

bool GameClipView::ProcessInput(const InputCompactEvent& event)
{
  if (!VideoEngine::Instance().IsPlaying())
  {
    return true;
  }

  // QUIT GAMECLIPVIEW - A
  if (event.APressed())
  {
    mState = State::Quit;
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
      std::string message = md.Favorite() ? _("added to favorites") : _("removed from favorites");
      mWindow.InfoPopupAdd(new GuiInfoPopup(mWindow, message + ":\n" + mGame->getDisplayName(), popupDuration,
                                            GuiInfoPopup::PopupType::None));

      if (RecalboxConf::Instance().GetShowHelp())
        updateHelpPrompts();
    }
    return true;
  }

  // HIDE SHOW GAME CLIP HELP ITEMS - UP or DOWN
  if (RecalboxConf::Instance().GetShowHelp() && (event.AnyDownPressed() || event.AnyUpPressed()))
  {
    RecalboxConf::Instance().SetShowGameClipHelpItems(!RecalboxConf::Instance().GetShowGameClipHelpItems());
    updateHelpPrompts();
    return true;
  }
  return true;
}

void GameClipView::startGameClip()
{
  getGame();
  mGameClipContainer.setGameInfo(mGame);
  mTimer.Initialize(0);
  NotificationManager::Instance().Notify(*mGame, Notification::StartGameClip);
  if (RecalboxConf::Instance().GetShowHelp())
    updateHelpPrompts();
}

void GameClipView::stopGameClip()
{
  for (;;)
  {
    if (VideoEngine::Instance().IsIdle())
    {
      break;
    }
    mGameClipContainer.StopVideo();
  }
  NotificationManager::Instance().Notify(*mGame, Notification::StopGameClip);
}

void GameClipView::quitGameClipView()
{
  stopGameClip();
  ViewController::Instance().quitGameClipView();
}

bool GameClipView::getHelpPrompts(Help& help)
{
  help.Set(HelpType::A, _("QUIT")).Set(HelpType::Start, _("LAUNCH")).Set(HelpType::Y, mGame->Metadata().Favorite() ? _(
    "Remove from favorite") : _("Favorite")).Set(HelpType::LeftRight, _("CHANGE")).Set(HelpType::Select,
                                                                                       _("SHOW IN LIST"));
  return true;
}

void GameClipView::updateHelpPrompts()
{
  if (getParent() != nullptr)
  {
    getParent()->updateHelpPrompts();
    return;
  }

  HelpItems().Clear();
  HelpItems().Empty();
  if (RecalboxConf::Instance().GetShowGameClipHelpItems() && getHelpPrompts(HelpItems()))
  {
    ApplyHelpStyle();
  }
  mWindow.UpdateHelp();
}

void GameClipView::ApplyHelpStyle()
{
  HelpItemStyle().FromTheme(mGame->getSystem()->getTheme(), getName());
}