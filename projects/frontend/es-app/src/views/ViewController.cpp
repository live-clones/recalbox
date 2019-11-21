#include <RecalboxConf.h>
#include <recalbox/RecalboxSystem.h>
#include "views/ViewController.h"
#include "utils/Log.h"
#include "systems/SystemData.h"
#include "Settings.h"

#include "views/gamelist/BasicGameListView.h"
#include "views/gamelist/DetailedGameListView.h"
#include "guis/GuiDetectDevice.h"
#include "animations/LaunchAnimation.h"
#include "animations/MoveCameraAnimation.h"
#include "animations/LambdaAnimation.h"

#include "AudioManager.h"

#include <memory>
#include <systems/SystemManager.h>
#include <MainRunner.h>

ViewController* ViewController::sInstance = nullptr;

ViewController::ViewController(Window& window, SystemManager& systemManager)
	: Gui(window),
	  mSystemManager(systemManager),
	  mCurrentView(nullptr),
	  mCamera(Transform4x4f::Identity()),
	  mFadeOpacity(0),
	  mLockInput(false),
    mState()
{
  if (sInstance == nullptr)
    sInstance = this;
  else
  {
    LOG(LogError) << "ViewController multiple instance detected";
    exit(-1);
  }

	mState.viewing = ViewMode::None;
	mFavoritesOnly = Settings::Instance().FavoritesOnly();
}

ViewController::~ViewController()
{
  if (sInstance == this)
	  sInstance = nullptr;
}

void ViewController::goToStart()
{
	// TODO
	/* mState.viewing = START_SCREEN;
	mCurrentView.reset();
	playViewTransition(); */

  std::string systemName = RecalboxConf::Instance().AsString("emulationstation.selectedsystem");
  int index = systemName.empty() ? -1 : mSystemManager.getVisibleSystemIndex(systemName);
  SystemData* selectedSystem = index < 0 ? nullptr : mSystemManager.GetVisibleSystemList().at(index);

  if ((selectedSystem == nullptr) || !selectedSystem->HasGame())
    selectedSystem = mSystemManager.FirstNonEmptySystem();

  if (RecalboxConf::Instance().AsBool("emulationstation.hidesystemview"))
    goToGameList(selectedSystem);
  else
  {
    if (RecalboxConf::Instance().AsBool("emulationstation.bootongamelist"))
      goToGameList(selectedSystem);
    else
      goToSystemView(selectedSystem);
  }
}

int ViewController::getSystemId(SystemData* system)
{
	const std::vector<SystemData*>& sysVec = mSystemManager.GetVisibleSystemList();
	return std::find(sysVec.begin(), sysVec.end(), system) - sysVec.begin();
}

void ViewController::goToSystemView(SystemData* system)
{
  auto systemList = getSystemListView();
  systemList->setPosition((float)getSystemId(system) * Renderer::getDisplayWidthAsFloat(), systemList->getPosition().y());

  if (!system->HasGame()) {
    system = mSystemManager.FirstNonEmptySystem();
  }

	mState.viewing = ViewMode::SystemList;
	mState.system = system;

	systemList->goToSystem(system, false);
	mCurrentView = systemList;
	mCurrentView->onShow();

	playViewTransition();

	RecalboxSystem::NotifySystem(*system);
}

void ViewController::goToNextGameList()
{
	assert(mState.viewing == ViewMode::GameList);
	SystemData* system = getState().getSystem();
	assert(system);
	SystemData* next = mSystemManager.NextVisible(system);
	while(!next->getRootFolder()->hasChildren()) {
		next = mSystemManager.NextVisible(next);
	}
  AudioManager::Instance().StartPlaying(next->getTheme());

	goToGameList(next);
}

void ViewController::goToPrevGameList()
{
	assert(mState.viewing == ViewMode::GameList);
	SystemData* system = getState().getSystem();
	assert(system);
	SystemData* prev = mSystemManager.PreviousVisible(system);
	while(!prev->getRootFolder()->hasChildren()) {
		prev = mSystemManager.PreviousVisible(prev);
	}
  AudioManager::Instance().StartPlaying(prev->getTheme());
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
		auto sysList = getSystemListView();
		float offX = sysList->getPosition().x();
		int sysId = getSystemId(system);
		sysList->setPosition((float)sysId * Renderer::getDisplayWidthAsFloat(), sysList->getPosition().y());
		offX = sysList->getPosition().x() - offX;
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
		if (!system->isFavorite())
		{
			updateFavorite(system, getGameListView(system).get()->getCursor());
			mFavoritesOnly = Settings::Instance().FavoritesOnly();
		}
	}

	mState.viewing = ViewMode::GameList;
	mState.system = system;

	mCurrentView = getGameListView(system);
	playViewTransition();

  RecalboxSystem::NotifyGame(*getGameListView(system).get()->getCursor(), false, false);
}

void ViewController::updateFavorite(SystemData* system, FileData* file)
{
	IGameListView* view = getGameListView(system).get();
	if (Settings::Instance().FavoritesOnly())
	{
		view->populateList(system->getRootFolder());
		FileData* nextFavorite = system->getRootFolder()->GetNextFavoriteTo(file);
	  view->setCursor(nextFavorite != nullptr ? nextFavorite : file);
	}

	view->updateInfoPanel();
}

void ViewController::playViewTransition()
{
	Vector3f target(Vector3f::Zero());
	if(mCurrentView)
		target = mCurrentView->getPosition();

	// no need to animate, we're not going anywhere (probably goToNextGamelist() or goToPrevGamelist() when there's only 1 system)
	if(target == -mCamera.translation() && !isAnimationPlaying(0))
		return;

	std::string transition_style = Settings::Instance().TransitionStyle();
	if(transition_style == "fade")
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
	} else if (transition_style == "slide"){
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
	auto it = mGameListViews.find(file->getSystem());
	if (it != mGameListViews.end()) {
		it->second->onFileChanged(file, change);
	}
	if (file->Metadata().Favorite()) {
		for (auto& mGameListView : mGameListViews)
		{
			if (mGameListView.first->getName() == "favorites") {
				mGameListView.second->onFileChanged(file, change);
				break;
			}
		}
	}
}

void ViewController::launch(FileData* game, Vector3f center, const std::string& netplay, const std::string& core, const std::string& ip, const std::string& port)
{
	if(!game->isGame())
	{
		LOG(LogError) << "tried to launch something that isn't a game";
		return;
	}

	Transform4x4f origCamera = mCamera;
	origCamera.translation() = -mCurrentView->getPosition();

	center += mCurrentView->getPosition();
	stopAnimation(1); // make sure the fade in isn't still playing
	mLockInput = true;

	std::string transition_style = Settings::Instance().TransitionStyle();

	auto launchFactory = [this, game, origCamera, netplay, core, ip, port] (const std::function<void(std::function<void()>)>& backAnimation) {
		return [this, game, origCamera, backAnimation, netplay, core, ip, port] {
      game->getSystem()->RunGame(mWindow, *game, netplay, core, ip, port);
			mCamera = origCamera;
			backAnimation([this] { mLockInput = false; });
			this->onFileChanged(game, FileChangeType::Run);
		};
	};

	if(transition_style == "fade")
	{
		// fade out, launch game, fade back in
		auto fadeFunc = [this](float t) {
			mFadeOpacity = lerp<float>(0.0f, 1.0f, t);
		};
		setAnimation(new LambdaAnimation(fadeFunc, 800), 0, launchFactory([this, fadeFunc](const std::function<void()>& finishedCallback) {
			setAnimation(new LambdaAnimation(fadeFunc, 800), 0, finishedCallback, true);
		}));
	} else if (transition_style == "slide"){

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

	//decide type
	bool detailed = system->getRootFolder()->hasDetailedData();

	if(detailed && ! (RecalboxConf::Instance().AsBool("emulationstation.forcebasicgamelistview")))
		view = std::shared_ptr<IGameListView>(new DetailedGameListView(mWindow, system->getRootFolder(), system));
	else
		view = std::shared_ptr<IGameListView>(new BasicGameListView(mWindow, system->getRootFolder()));

	// uncomment for experimental "image grid" view
	//view = std::shared_ptr<IGameListView>(new GridGameListView(mWindow, system->getRootFolder()));

	view->setTheme(system->getTheme());

	const std::vector<SystemData*>& sysVec = mSystemManager.GetVisibleSystemList();
	int id = std::find(sysVec.begin(), sysVec.end(), system) - sysVec.begin();
	view->setPosition((float)id * Renderer::getDisplayWidthAsFloat(), Renderer::getDisplayHeightAsFloat() * 2);

	addChild(view.get());

	mGameListViews[system] = view;
	mInvalidGameList[system] = false;
	return view;
}

std::shared_ptr<SystemView> ViewController::getSystemListView()
{
	//if we already made one, return that one
	if(mSystemListView)
		return mSystemListView;

	mSystemListView = std::make_shared<SystemView>(mWindow, mSystemManager);
	addChild(mSystemListView.get());
	mSystemListView->setPosition(0, Renderer::getDisplayHeightAsFloat());
	return mSystemListView;
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
  if (mCurrentView) return mCurrentView->ProcessInput(event);

	return false;
}

void ViewController::update(int deltaTime)
{
	if(mCurrentView)
	{
		mCurrentView->update(deltaTime);
	}

	updateSelf(deltaTime);
}

void ViewController::render(const Transform4x4f& parentTrans)
{
	Transform4x4f trans = mCamera * parentTrans;
  Transform4x4f transInverse(Transform4x4f::Identity());
  transInverse.invert(trans);

	// camera position, position + size
	Vector3f viewStart = transInverse.translation();
	Vector3f viewEnd = transInverse * Vector3f(Renderer::getDisplayWidthAsFloat(), Renderer::getDisplayHeightAsFloat(), 0);

	int vpl = (int)viewStart.x();
  int vpu = (int)viewStart.y();
  int vpr = (int)viewEnd.x() - 1;
  int vpb = (int)viewEnd.y() - 1;

	// draw systemview
  for(;;)
  {
    auto systemView = getSystemListView();

    // clipping - only y
    const Vector3f& position = systemView->getPosition();
    const Vector2f& size = systemView->getSize();

    int gu = (int)position.y();
    int gb = (int)position.y() + (int)size.y() - 1;

    if (gb < vpu) break;
    if (gu > vpb) break;

    systemView->render(trans);
    break;
  }

	// draw gamelists
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

		mGameListView.second->render(trans);
	}

	if(mWindow.peekGui() == nullptr) // TODO:: dafuk?!
		mWindow.renderHelpPromptsEarly();

	// fade out
	if(mFadeOpacity != 0.0)
	{
		Renderer::setMatrix(parentTrans);
		Renderer::drawRect(0, 0, Renderer::getDisplayWidthAsInt(), Renderer::getDisplayHeightAsInt(), 0x00000000 | (unsigned char)(mFadeOpacity * 255));
	}
}

bool ViewController::reloadGameListView(IGameListView* view, bool reloadTheme)
{
	if (view->getRoot()->countAll(false) > 0)
	{
		for (auto it = mGameListViews.begin(); it != mGameListViews.end(); it++)
		{
			if (it->second.get() == view)
			{
				bool isCurrent = (mCurrentView == it->second);
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
					mCurrentView = newView;
				break;
			}
		}
		return true;
	}
	else
	{
    deleteAndReloadAll();
    return false;
  }
}

void ViewController::deleteAndReloadAll()
{
  MainRunner::RequestQuit(MainRunner::ExitState::Relaunch);
  /*Window *window = mWindow;
  window->renderShutdownScreen();
  mSystemManager.deleteSystems();
  mSystemManager.loadConfig();
  window->deleteAllGui();
  ViewController::Instance().reloadAll();
  window->pushGui(&ViewController::Instance());
  ViewController::Instance().goToStart();*/
}

void ViewController::reloadAll()
{
	std::map<SystemData*, FileData*> cursorMap;
	for (auto& mGameListView : mGameListViews)
	{
		cursorMap[mGameListView.first] = mGameListView.second->getCursor();
	}
	mGameListViews.clear();

	for (auto& it : cursorMap)
	{
		it.first->loadTheme();
		getGameListView(it.first)->setCursor(it.second);
	}

	mSystemListView.reset();
	getSystemListView();

	// update mCurrentView since the pointers changed
	if(mState.viewing == ViewMode::GameList)
	{
		mCurrentView = getGameListView(mState.getSystem());
	}else if(mState.viewing == ViewMode::SystemList)
	{
		
		SystemData* system = mState.getSystem();
		goToSystemView(mSystemManager.GetVisibleSystemList().front());
		mSystemListView->goToSystem(system, false);
		mCurrentView = mSystemListView;
		
	}else{
		goToSystemView(mSystemManager.GetVisibleSystemList().front());
	}

	updateHelpPrompts();
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
	return mCurrentView ? mCurrentView->getHelpPrompts(help) : true;
}

void ViewController::ApplyHelpStyle()
{
	if(!mCurrentView)
		return GuiComponent::ApplyHelpStyle();

	return mCurrentView->ApplyHelpStyle();
}
