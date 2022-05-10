#include <RecalboxConf.h>
#include <recalbox/RecalboxSystem.h>
#include "views/ViewController.h"
#include "utils/Log.h"
#include "systems/SystemData.h"

#include "views/gamelist/BasicGameListView.h"
#include "views/gamelist/DetailedGameListView.h"
#include "guis/GuiDetectDevice.h"
#include "animations/LaunchAnimation.h"
#include "animations/MoveCameraAnimation.h"
#include "animations/LambdaAnimation.h"

#include "audio/AudioManager.h"
#include <audio/AudioMode.h>

#include <memory>
#include <systems/SystemManager.h>
#include <MainRunner.h>
#include <bios/BiosManager.h>
#include <guis/GuiMsgBox.h>
#include <guis/menus/GuiCheckMenu.h>
#include <utils/locale/LocaleHelper.h>
#include <usernotifications/NotificationManager.h>

#include <games/GameFilesUtils.h>

ViewController::ViewController(WindowManager& window, SystemManager& systemManager)
	: StaticLifeCycleControler<ViewController>("ViewController")
	, Gui(window)
	, mSystemManager(systemManager)
	, mCurrentView(&mSplashView)
	, mSystemListView(window, systemManager)
	, mSplashView(window)
	, mGameClipView(nullptr)
  , mCrtView(nullptr)
	, mCamera(Transform4x4f::Identity())
	, mFadeOpacity(0)
	, mLockInput(false)
  , mState()
{
  // Progress interface
  systemManager.SetProgressInterface(&mSplashView);

  // default view mode
	mState.viewing = ViewMode::SplashScreen;

	// System View
  mSystemListView.setPosition(0, Renderer::Instance().DisplayHeightAsFloat());
  // Splash
  mSplashView.setPosition(0,0);
}

void ViewController::goToStart()
{
  CheckFilters();

  std::string systemName = RecalboxConf::Instance().GetStartupSelectedSystem();
  int index = systemName.empty() ? -1 : mSystemManager.getVisibleSystemIndex(systemName);
  SystemData* selectedSystem = index < 0 ? nullptr : mSystemManager.GetVisibleSystemList()[index];

  if ((selectedSystem == nullptr) || !selectedSystem->HasVisibleGame())
    selectedSystem = mSystemManager.FirstNonEmptySystem();

  if (selectedSystem == nullptr)
  {

    mWindow.pushGui(new GuiMsgBox(mWindow, "Your filters preferences hide all your games !\nThe filters will be reseted and recalbox will be reloaded.", _("OK"), [this] { ResetFilters();}));
    return;
  }

  if (RecalboxConf::Instance().GetStartupHideSystemView())
    goToGameList(selectedSystem);
  else
  {
    if (RecalboxConf::Instance().GetStartupStartOnGamelist())
      goToGameList(selectedSystem);
    else
      goToSystemView(selectedSystem);
  }
}

bool ViewController::CheckFilters()
{

  if(mSystemManager.FirstNonEmptySystem() == nullptr)
  {
    ResetFilters();
    mWindow.pushGui(new GuiMsgBox(mWindow, "Your filters preferences hide all your games !\nAll filters have been reseted.", _("OK"), []{}));
    return false;
  }
  return true;
}

void ViewController::ResetFilters()
{
  RecalboxConf& conf = RecalboxConf::Instance();

  conf.SetShowOnlyLatestVersion(false);
  conf.SetFavoritesOnly(false);
  conf.SetGlobalHidePreinstalled(false);
  conf.SetHideNoGames(false);
  conf.SetFilterAdultGames(false);

  conf.SetCollectionHide("ports",false);
//  MainRunner::RequestQuit(MainRunner::ExitState::Relaunch, true);
}

int ViewController::getSystemId(SystemData* system)
{
	const std::vector<SystemData*>& sysVec = mSystemManager.GetVisibleSystemList();
	return (int)(std::find(sysVec.begin(), sysVec.end(), system) - sysVec.begin());
}

void ViewController::goToQuitScreen()
{
  mSplashView.Quit();
  mState.viewing = ViewMode::SplashScreen;
  mCamera.translation().Set(0,0,0);
}

void ViewController::goToSystemView(SystemData* system)
{
  CheckFilters();
  mSystemListView.setPosition((float)getSystemId(system) * Renderer::Instance().DisplayWidthAsFloat(), mSystemListView.getPosition().y());

  if (!system->HasVisibleGame()) {
    system = mSystemManager.FirstNonEmptySystem();
  }

	mState.viewing = ViewMode::SystemList;
	mState.system = system;

  mSystemListView.goToSystem(system, false);
	mCurrentView = &mSystemListView;
	mCurrentView->onShow();

	playViewTransition();

  NotificationManager::Instance().Notify(*system, Notification::SystemBrowsing);
}

void ViewController::goToGameClipView()
{
    if(GameClipView::IsGameClipEnabled() && !mState.gameClipRunning) {

      if(AudioMode::MusicsXorVideosSound == RecalboxConf::Instance().GetAudioMode())
      {
        AudioManager::Instance().StopAll();
      }
        mGameClipView = new GameClipView(mWindow, mSystemManager);
        NotificationManager::Instance().Notify(Notification::StartGameClip);
        mState.gameClipRunning = true;
    }
}

void ViewController::quitGameClipView()
{
  if(!mState.gameClipRunning) return;

  WakeUp();
  delete mGameClipView;
  if(AudioMode::MusicsXorVideosSound == RecalboxConf::Instance().GetAudioMode())
  {
    AudioManager::Instance().StartPlaying(mState.system->Theme());
  }
  switch (mState.viewing)
  {
    case ViewMode::SystemList:
      goToSystemView(mSystemListView.getSelected());
      break;
    case ViewMode::GameList:
      if(mState.getSystem()->HasVisibleGame())
        goToGameList(mState.getSystem());
      else goToStart();
      break;
    case ViewMode::None:
    case ViewMode::SplashScreen:
      break;
  }

  mState.gameClipRunning = false;
  updateHelpPrompts();
}

void ViewController::goToCrtView()
{
  mCrtView = new CrtView(mWindow);
  mCrtView->onShow();
}

void ViewController::quitCrtView()
{
  delete mCrtView;
  mCrtView = nullptr;
  switch (mState.viewing)
  {
    case ViewMode::SystemList: goToSystemView(mSystemListView.getSelected()); break;
    case ViewMode::GameList:   goToGameList(mState.getSystem()); break;
    case ViewMode::None:
    case ViewMode::SplashScreen:
    default: break;
  }
  updateHelpPrompts();
}

void ViewController::selectGamelistAndCursor(FileData *file) {
  mState.viewing = ViewMode::GameList;
  SystemData& system = file->System();
  goToGameList(&system);
  IGameListView* view = getGameListView(&system).get();
  view->setCursorStack(file);
  view->setCursor(file);
}

void ViewController::goToNextGameList()
{
	assert(mState.viewing == ViewMode::GameList);
	SystemData* system = getState().getSystem();
	assert(system);

  CheckFilters();
  SystemData* next = mSystemManager.NextVisible(system);
	while(!next->HasVisibleGame()) {
		next = mSystemManager.NextVisible(next);

	}

  AudioManager::Instance().StartPlaying(next->Theme());

	goToGameList(next);
}

void ViewController::goToPrevGameList()
{
	assert(mState.viewing == ViewMode::GameList);
	SystemData* system = getState().getSystem();
	assert(system);

  CheckFilters();
  SystemData* prev = mSystemManager.PreviousVisible(system);
	while(!prev->HasVisibleGame()) {
		prev = mSystemManager.PreviousVisible(prev);
	}

  AudioManager::Instance().StartPlaying(prev->Theme());
	goToGameList(prev);
}

bool ViewController::goToGameList(std::string& systemName) {
	SystemData* system = mSystemManager.SystemByName(systemName);
	if (system != nullptr) {
		goToGameList(system);
		return true;
	}
	return false;
}

void ViewController::goToGameList(SystemData* system)
{
	if (mState.viewing == ViewMode::SystemList)
	{
		// move system list
		float offX = mSystemListView.getPosition().x();
		int sysId = getSystemId(system);
    mSystemListView.setPosition((float)sysId * Renderer::Instance().DisplayWidthAsFloat(), mSystemListView.getPosition().y());
		offX = mSystemListView.getPosition().x() - offX;
		mCamera.translation().x() -= offX;
	}

	if (mInvalidGameList[system])
	{
		mInvalidGameList[system] = false;
    if (!reloadGameListView(system))
    {
      // if listview has been reload due to last game has been deleted,
      // we have to stop the previous goToGameList process because current
      // system will no longer exists in the available list
      return;
    }
		if (!system->IsFavorite())
		{
			updateFavorite(system, getGameListView(system)->getCursor());
		}
	}

	mState.viewing = ViewMode::GameList;
	mState.system = system;

	mCurrentView = getGameListView(system).get();
	playViewTransition();

  NotificationManager::Instance().Notify(*getGameListView(system)->getCursor(), Notification::GamelistBrowsing);
  // for reload cursor video path if present
  getGameListView(system)->DoUpdateGameInformation();
}

void ViewController::updateFavorite(SystemData* system, FileData* file)
{
	IGameListView* view = getGameListView(system).get();
	if (RecalboxConf::Instance().GetFavoritesOnly())
	{
		view->populateList(system->MasterRoot());
		FileData* nextFavorite = system->MasterRoot().GetNextFavoriteTo(file);
	  view->setCursor(nextFavorite != nullptr ? nextFavorite : file);
	}

	view->updateInfoPanel();
}

void ViewController::playViewTransition()
{
	Vector3f target(Vector3f::Zero());
  target = mCurrentView->getPosition();

	// no need to animate, we're not going anywhere (probably goToNextGamelist() or goToPrevGamelist() when there's only 1 system)
	if(target == -mCamera.translation() && !isAnimationPlaying(0))
		return;

	std::string transitionTheme = ThemeData::getCurrent().getTransition();
	if (transitionTheme.empty()) transitionTheme = RecalboxConf::Instance().GetThemeTransition();
	if(transitionTheme == "fade")
	{
		// fade
		// stop whatever's currently playing, leaving mFadeOpacity wherever it is
		cancelAnimation(0);

		auto fadeFunc = [this](float t) {
			mFadeOpacity = lerp<float>(0, 1, t);
		};

		const static int FADE_DURATION = 240; // fade in/out time
		const static int FADE_WAIT = 320; // time to wait between in/out
		setAnimation(new LambdaAnimation(fadeFunc, FADE_DURATION), 0, [this, fadeFunc, target] {
			this->mCamera.translation() = -target;
			updateHelpPrompts();
			setAnimation(new LambdaAnimation(fadeFunc, FADE_DURATION), FADE_WAIT, nullptr, true);
		});

		// fast-forward animation if we're partway faded
		if(target == -mCamera.translation())
		{
			// not changing screens, so cancel the first half entirely
			advanceAnimation(0, FADE_DURATION);
			advanceAnimation(0, FADE_WAIT);
			advanceAnimation(0, FADE_DURATION - (int)(mFadeOpacity * FADE_DURATION));
		}else{
			advanceAnimation(0, (int)(mFadeOpacity * FADE_DURATION));
		}
	} else if (transitionTheme == "slide"){
		// slide or simple slide
		setAnimation(new MoveCameraAnimation(mCamera, target));
		updateHelpPrompts(); // update help prompts immediately
	} else {
		// instant
		setAnimation(new LambdaAnimation(
				[this, target](float t)
		{
      (void)t;
		  this->mCamera.translation() = -target;
		}, 1));
		updateHelpPrompts();
	}
}

void ViewController::onFileChanged(FileData* file, FileChangeType change)
{
	auto it = mGameListViews.find(&file->System());
	if (it != mGameListViews.end()) {
		it->second->onFileChanged(file, change);
	}
	if (file->Metadata().Favorite()) {
		for (auto& mGameListView : mGameListViews)
		{
			if (mGameListView.first->Name() == "favorites") {
				mGameListView.second->onFileChanged(file, change);
				break;
			}
		}
	}
}

void ViewController::Launch(FileData* game, const GameLinkedData& data, const Vector3f& cameraTarget, bool forceLaunch)
{
  mGameLinkedData = data;
  LaunchCheck(game, cameraTarget, forceLaunch);
}

void ViewController::LaunchCheck(FileData* game, const Vector3f& cameraTarget, bool forceLaunch)
{
  EmulatorData emulator = mSystemManager.Emulators().GetGameEmulator(*game);
  if (!emulator.IsValid())
  {
    { LOG(LogError) << "[ViewController] Empty emulator/core when running " << game->FilePath().ToString() << '!'; }
    return;
  }

  if (!forceLaunch)
  {
    const BiosList& biosList = BiosManager::Instance().SystemBios(game->System().Name());
    if (biosList.TotalBiosKo() != 0)
    {
      // Build emulator name
      std::string emulatorString = emulator.Emulator();
      if (emulator.Emulator() != emulator.Core()) emulatorString.append(1, '/').append(emulator.Core());
      // Build text
      std::string text = _("At least one mandatory BIOS is missing for %emulator%!\nYour game '%game%' will very likely not run at all until required BIOS are put in the expected folder.\n\nDo you want to launch the game anyway?");
      text = Strings::Replace(text, "%emulator%", emulatorString);
      text = Strings::Replace(text, "%game%", game->Name());
      // Add bios names
      /*text.append("\n\n")
          .append(_("Missing bios list:"))
          .append(1, ' ')
          .append(Strings::Join(biosList.GetMissingBiosFileList(), '\n'));*/
      // Show the dialog box
      Gui* gui = new GuiMsgBox(mWindow,
                               text,
                               _("YES"),
                               [this, game, &cameraTarget] { LaunchCheck(game, cameraTarget, true); },
                               _("NO"),
                               nullptr);
      mWindow.pushGui(gui);
      return;
    }
  }

  if (mGameLinkedData.Crt().IsRegionOrStandardConfigured())
  {
    if (mGameLinkedData.Crt().MustChoosePALorNTSC(game->System()))
    {
        if(game->System().Name() == "megadrive")
        {
            static int lastChoiceMulti60 = 0;
            mWindow.pushGui(new GuiCheckMenu(mWindow,
                                             _("Game refresh rate"),
                                             game->Name(),
                                             lastChoiceMulti60,
                                             _("AUTO"),
                                             _("AUTO"),
                                             [this, game, &cameraTarget] { mGameLinkedData.ConfigurableCrt().ConfigureRegion(CrtData::CrtRegion::AUTO);
                mGameLinkedData.ConfigurableCrt().ConfigureVideoStandard(CrtData::CrtVideoStandard::AUTO);
                LaunchCheck(game, cameraTarget, true); lastChoiceMulti60 = 0;},
                                             _("60Hz (US)"),
                                             _("60Hz (US)"),
                                             [this, game, &cameraTarget] { mGameLinkedData.ConfigurableCrt().ConfigureRegion(CrtData::CrtRegion::US);
                mGameLinkedData.ConfigurableCrt().ConfigureVideoStandard(CrtData::CrtVideoStandard::NTSC);
                LaunchCheck(game, cameraTarget, true); lastChoiceMulti60 = 1;},
                                             _("60Hz (JP)"),
                                             _("60Hz (JP)"),
                                             [this, game, &cameraTarget] { mGameLinkedData.ConfigurableCrt().ConfigureRegion(CrtData::CrtRegion::JP);
                mGameLinkedData.ConfigurableCrt().ConfigureVideoStandard(CrtData::CrtVideoStandard::NTSC);
                LaunchCheck(game, cameraTarget, true);lastChoiceMulti60 = 2;},
                                             _("50Hz (EU)"),
                                             _("50Hz (EU)"),
                                             [this, game, &cameraTarget] { mGameLinkedData.ConfigurableCrt().ConfigureRegion(CrtData::CrtRegion::EU);
                mGameLinkedData.ConfigurableCrt().ConfigureVideoStandard(CrtData::CrtVideoStandard::PAL);
                LaunchCheck(game, cameraTarget, true); lastChoiceMulti60 = 3;}
                ));
        } else
        {
            static int lastChoice = 0;
            mWindow.pushGui(new GuiCheckMenu(mWindow,
                                         _("Game refresh rate"),
                                         game->Name(),
                                         lastChoice,
                                         _("AUTO"),
                                         _("AUTO"),
                                         [this, game, &cameraTarget] { mGameLinkedData.ConfigurableCrt().ConfigureRegion(CrtData::CrtRegion::AUTO);
                mGameLinkedData.ConfigurableCrt().ConfigureVideoStandard(CrtData::CrtVideoStandard::AUTO);
                LaunchCheck(game, cameraTarget, true); lastChoice = 0;},
                                         _("60Hz"),
                                         _("60Hz"),
                                         [this, game, &cameraTarget] { mGameLinkedData.ConfigurableCrt().ConfigureRegion(CrtData::CrtRegion::AUTO);
                mGameLinkedData.ConfigurableCrt().ConfigureVideoStandard(CrtData::CrtVideoStandard::NTSC);
                LaunchCheck(game, cameraTarget, true); lastChoice = 1;},
                                         _("50Hz"),
                                         _("50Hz"),
                                         [this, game, &cameraTarget] { mGameLinkedData.ConfigurableCrt().ConfigureRegion(CrtData::CrtRegion::AUTO);
                mGameLinkedData.ConfigurableCrt().ConfigureVideoStandard(CrtData::CrtVideoStandard::PAL);
                LaunchCheck(game, cameraTarget, true);lastChoice = 2;}
                ));
        }
        return;
    }
  }
  if (mGameLinkedData.Crt().IsHighResolutionConfigured())
  {
    const bool is31Khz = Board::Instance().CrtBoard().GetHorizontalFrequency() == ICrtInterface::HorizontalFrequency::KHz31;
    if (is31Khz || mGameLinkedData.Crt().MustChooseHighResolution(game->System()))
    {
        static int lastChoice = 0;
        mWindow.pushGui(new GuiCheckMenu(mWindow,
                                         _("Game resolution"),
                                         game->Name(),
                                         lastChoice,
                                         is31Khz ? _("240p@120") : _("240p"),
                                         is31Khz ? _("240p@120") : _("240p"),
                                         [this, game, &cameraTarget] { mGameLinkedData.ConfigurableCrt().ConfigureHighResolution(false); LaunchCheck(game, cameraTarget, true); lastChoice = 0; },
                                         is31Khz ? _("480p@60") : _("480i"),
                                         is31Khz ? _("480p@60") : _("480i"),
                                         [this, game, &cameraTarget] {mGameLinkedData.ConfigurableCrt().ConfigureHighResolution(true); LaunchCheck(game, cameraTarget, true); lastChoice = 1; }
                                         ));
      return;
    }
  }
  bool coreIsSoftpatching = game->System().Descriptor().IsSoftpatching(emulator.Emulator(), emulator.Core());
  if(RecalboxConf::Instance().GetGlobalSoftpatching() == "disabled")
  {
    mGameLinkedData.ConfigurablePath().SetDisabledSoftPatching(true);
  }
  else if(RecalboxConf::Instance().GetGlobalSoftpatching() == "confirm" && coreIsSoftpatching
  && !mGameLinkedData.ConfigurablePath().IsConfigured() && GameFilesUtils::HasSoftPatch(game))
  {
    static int lastChoice = 0;
    mWindow.pushGui(new GuiCheckMenu(mWindow,
                                     _("A patch has been detected, launch ?"),
                                     game->Name(),
                                     lastChoice,
                                     _("original"),
                                     _("original"),
                                     [this, game, &cameraTarget] {
                                       mGameLinkedData.ConfigurablePath().SetDisabledSoftPatching(false); LaunchCheck(game, cameraTarget, true); lastChoice = 0; },
                                     _("patched"),
                                     _("patched"),
                                     [this, game, &cameraTarget] {
                                       mGameLinkedData.ConfigurablePath().SetDisabledSoftPatching(true); LaunchCheck(game, cameraTarget, true); lastChoice = 1; }
                                    ));
    return;
  }

  LaunchAnimated(game, emulator, cameraTarget);
}

void ViewController::LaunchActually(FileData* game, const EmulatorData& emulator)
{
  DateTime start;
  GameRunner::Instance().RunGame(*game, emulator, mGameLinkedData);
  TimeSpan elapsed = DateTime() - start;

  if (elapsed.TotalMilliseconds() <= 3000) // 3s
  {
    // Build text
    std::string text = _("It seems that your game didn't start at all!\n\nIt's most likely due to either:\n- bad rom\n- missing/bad mandatory bios files\n- missing/bad optional BIOS files (but required for this very game)");
    // Show the dialog box
    Gui* gui = new GuiMsgBox(mWindow,
                             text,
                             _("OK"),
                             TextAlignment::Left);
    mWindow.pushGui(gui);
    return;
  }
}

void ViewController::LaunchAnimated(FileData* game, const EmulatorData& emulator, const Vector3f& cameraTarget)
{
  Vector3f center = cameraTarget.isZero() ? Vector3f(Renderer::Instance().DisplayWidthAsFloat() / 2.0f, Renderer::Instance().DisplayHeightAsFloat() / 2.0f, 0) : cameraTarget;

	if(!game->IsGame())
	{
    { LOG(LogError) << "[ViewController] Tried to launch something that isn't a game"; }
		return;
	}

	Transform4x4f origCamera = mCamera;
	origCamera.translation() = -mCurrentView->getPosition();

	center += mCurrentView->getPosition();
	stopAnimation(1); // make sure the fade in isn't still playing
	mLockInput = true;

  std::string transitionTheme = ThemeData::getCurrent().getTransition();
  if (transitionTheme.empty()) transitionTheme = RecalboxConf::Instance().GetThemeTransition();

	auto launchFactory = [this, game, origCamera, &emulator] (const std::function<void(std::function<void()>)>& backAnimation)
	{
		return [this, game, origCamera, backAnimation, emulator]
		{
		  LaunchActually(game, emulator);

      mCamera = origCamera;
			backAnimation([this] { mLockInput = false; });
			this->onFileChanged(game, FileChangeType::Run);

			// Re-sort last played system if it exists
      SystemData* lastPlayedSystem = mSystemManager.LastPlayedSystem();
      if (lastPlayedSystem != nullptr)
      {
        auto it = mGameListViews.find(lastPlayedSystem);
        if (it != mGameListViews.end())
        {
          IGameListView* lastPlayedGameListView = it->second.get();
          if (lastPlayedGameListView != nullptr)
            lastPlayedGameListView->onChanged(ISimpleGameListView::Change::Resort);
        }
      }
		};
	};

	if(transitionTheme == "fade")
	{
		// fade out, launch game, fade back in
		auto fadeFunc = [this](float t) {
			mFadeOpacity = lerp<float>(0.0f, 1.0f, t);
		};
		setAnimation(new LambdaAnimation(fadeFunc, 800), 0, launchFactory([this, fadeFunc](const std::function<void()>& finishedCallback) {
			setAnimation(new LambdaAnimation(fadeFunc, 800), 0, finishedCallback, true);
		}));
	} else if (transitionTheme == "slide"){

		// move camera to zoom in on center + fade out, launch game, come back in
		setAnimation(new LaunchAnimation(mCamera, mFadeOpacity, center, 1500), 0, launchFactory([this, center](const std::function<void()>& finishedCallback) {
			setAnimation(new LaunchAnimation(mCamera, mFadeOpacity, center, 1000), 0, finishedCallback, true);
		}));
	} else {
		setAnimation(new LaunchAnimation(mCamera, mFadeOpacity, center, 10), 0, launchFactory([this, center](const std::function<void()>& finishedCallback) {
			setAnimation(new LaunchAnimation(mCamera, mFadeOpacity, center, 10), 0, finishedCallback, true);
		}));
	}
}

std::shared_ptr<IGameListView> ViewController::getGameListView(SystemData* system)
{
	//if we already made one, return that one
	auto exists = mGameListViews.find(system);
	if(exists != mGameListViews.end())
		return exists->second;

	//if we didn't, make it, remember it, and return it
	std::shared_ptr<IGameListView> view;

  view = std::shared_ptr<IGameListView>(new DetailedGameListView(mWindow, mSystemManager, *system));
	view->setTheme(system->Theme());

	const std::vector<SystemData*>& sysVec = mSystemManager.GetVisibleSystemList();
	int id = (int)(std::find(sysVec.begin(), sysVec.end(), system) - sysVec.begin());
	view->setPosition((float)id * Renderer::Instance().DisplayWidthAsFloat(), Renderer::Instance().DisplayHeightAsFloat() * 2);

	addChild(view.get());

	mGameListViews[system] = view;
	mInvalidGameList[system] = false;
	return view;
}

bool ViewController::ProcessInput(const InputCompactEvent& event)
{
	if (mLockInput) return true;

	/* if we receive a button pressure for a non configured joystick, suggest the joystick configuration */
	if (event.AskForConfiguration())
	{
		mWindow.pushGui(new GuiDetectDevice(mWindow, false, nullptr));
		return true;
	}

  // Normal processing
  if(mState.gameClipRunning)
    mGameClipView->ProcessInput(event);
  else if (mCrtView != nullptr)
    mCrtView->ProcessInput(event);
  else
    mCurrentView->ProcessInput(event);

	return false;
}

void ViewController::Update(int deltaTime)
{
  mCurrentView->Update(deltaTime);
	updateSelf(deltaTime);
}

void ViewController::Render(const Transform4x4f& parentTrans)
{
  if (mState.gameClipRunning)
  {
    mGameClipView->Render(parentTrans);
    return;
  }

  if (mCrtView != nullptr)
  {
    mCrtView->Render(parentTrans);
    return;
  }

  Transform4x4f trans = mCamera * parentTrans;
  Transform4x4f transInverse(Transform4x4f::Identity());
  transInverse.invert(trans);

	// camera position, position + size
	Vector3f viewStart = transInverse.translation();
	Vector3f viewEnd = transInverse * Vector3f(Renderer::Instance().DisplayWidthAsFloat(), Renderer::Instance().DisplayHeightAsFloat(), 0);

	int vpl = (int)viewStart.x();
  int vpu = (int)viewStart.y();
  int vpr = (int)viewEnd.x() - 1;
  int vpb = (int)viewEnd.y() - 1;

  // Draw splash
  for(;;)
  {
    // clipping - only y
    const Vector3f& position = mSplashView.getPosition();
    const Vector2f& size = mSplashView.getSize();

    int gu = (int)position.y();
    int gb = (int)position.y() + (int)size.y() - 1;

    if (gb < vpu) break;
    if (gu > vpb) break;

    mSplashView.Render(trans);
    break;
  }

	// Draw systemview
  for(;;)
  {
    // clipping - only y
    const Vector3f& position = mSystemListView.getPosition();
    const Vector2f& size = mSystemListView.getSize();

    int gu = (int)position.y();
    int gb = (int)position.y() + (int)size.y() - 1;

    if (gb < vpu) break;
    if (gu > vpb) break;

    mSystemListView.Render(trans);
    break;
  }

	// Draw gamelists
	for (auto& mGameListView : mGameListViews)
	{
    // clipping
    const Vector3f& position = mGameListView.second->getPosition();
    const Vector2f& size = mGameListView.second->getSize();

    int gl = (int)position.x();
    int gu = (int)position.y();
    int gr = (int)position.x() + (int)size.x() - 1;
    int gb = (int)position.y() + (int)size.y() - 1;

    if (gr < vpl) continue;
    if (gl > vpr) continue;
    if (gb < vpu) continue;
    if (gu > vpb) continue;

    mGameListView.second->Render(trans);
	}

	if(mWindow.peekGui() == nullptr) // TODO:: dafuk?!
		mWindow.renderHelpPromptsEarly();

	// fade out
	if(mFadeOpacity != 0.0)
	{
		Renderer::SetMatrix(parentTrans);
		Renderer::DrawRectangle(0, 0, Renderer::Instance().DisplayWidthAsInt(), Renderer::Instance().DisplayHeightAsInt(), 0x00000000 | (unsigned char)(mFadeOpacity * 255));
	}
}

bool ViewController::reloadGameListView(IGameListView* view, bool reloadTheme)
{
	if (view->System().HasVisibleGame())
	{
		for (auto it = mGameListViews.begin(); it != mGameListViews.end(); it++)
		{
			if (it->second.get() == view)
			{
				bool isCurrent = (mCurrentView == it->second.get());
				SystemData *system = it->first;
				bool hasGame = system->HasGame();
				FileData *cursor = hasGame ? view->getCursor() : nullptr;
				mGameListViews.erase(it);

				if (reloadTheme)
					system->loadTheme();

				std::shared_ptr<IGameListView> newView = getGameListView(system);
				if (hasGame)
					newView->setCursor(cursor);
				if (isCurrent)
					mCurrentView = newView.get();
				break;
			}
		}
		return true;
	}
	else
	{
    MainRunner::RequestQuit(MainRunner::ExitState::Relaunch, false);
    return false;
  }
}

void ViewController::setInvalidGamesList(SystemData* system)
{
	for (auto& mGameListView : mGameListViews)
	{
		if (system == (mGameListView.first))
		{
			mInvalidGameList[mGameListView.first] = true;
			break;
		}
	}
}

void ViewController::setAllInvalidGamesList(SystemData* systemExclude)
{
	for (auto& mGameListView : mGameListViews)
	{
		if (systemExclude != (mGameListView.first))
		{
			mInvalidGameList[mGameListView.first] = true;
		}
	}
}

bool ViewController::getHelpPrompts(Help& help)
{
  if (mCrtView != nullptr) return mCrtView->getHelpPrompts(help);
	return mCurrentView->getHelpPrompts(help);
}

void ViewController::ApplyHelpStyle()
{
	return mCurrentView->ApplyHelpStyle();
}

