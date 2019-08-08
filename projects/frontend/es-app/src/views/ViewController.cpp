#include <RecalboxConf.h>
#include <recalbox/RecalboxSystem.h>
#include "views/ViewController.h"
#include "Log.h"
#include "SystemData.h"
#include "Settings.h"

#include "views/gamelist/BasicGameListView.h"
#include "views/gamelist/DetailedGameListView.h"
#include "views/gamelist/GridGameListView.h"
#include "guis/GuiMenu.h"
#include "guis/GuiDetectDevice.h"
#include "guis/GuiMsgBox.h"
#include "animations/LaunchAnimation.h"
#include "animations/MoveCameraAnimation.h"
#include "animations/LambdaAnimation.h"

#include "AudioManager.h"

#include <SDL.h>

ViewController* ViewController::sInstance = nullptr;

ViewController* ViewController::get()
{
	assert(sInstance);
	return sInstance;
}

void ViewController::init(Window* window)
{
	assert(!sInstance);
	sInstance = new ViewController(window);
}

ViewController::ViewController(Window* window)
	: GuiComponent(window), mCurrentView(nullptr), mCamera(Transform4x4f::Identity()), mFadeOpacity(0), mLockInput(false), mWindow(window)
{
	mState.viewing = NOTHING;
	mFavoritesOnly = Settings::getInstance()->getBool("FavoritesOnly");
}

ViewController::~ViewController()
{
	assert(sInstance == this);
	sInstance = nullptr;
}

void ViewController::goToStart()
{
	// TODO
	/* mState.viewing = START_SCREEN;
	mCurrentView.reset();
	playViewTransition(); */

  std::string systemName = RecalboxConf::getInstance()->get("emulationstation.selectedsystem");
  int index = systemName.empty() ? -1 : SystemData::getSystemIndex(systemName);
  SystemData* selectedSystem = index < 0 ? nullptr : SystemData::sSystemVector.at(index);

  if (!selectedSystem || !selectedSystem->hasGame())
    selectedSystem = SystemData::getFirstSystemWithGame();

  if (RecalboxConf::getInstance()->get("emulationstation.hidesystemview") == "1")
    goToGameList(selectedSystem);
  else
  {
    if (RecalboxConf::getInstance()->get("emulationstation.bootongamelist") == "1")
      goToGameList(selectedSystem);
    else
      goToSystemView(selectedSystem);
  }
}

int ViewController::getSystemId(SystemData* system)
{
	std::vector<SystemData*>& sysVec = SystemData::sSystemVector;
	return std::find(sysVec.begin(), sysVec.end(), system) - sysVec.begin();
}

void ViewController::goToSystemView(SystemData* system)
{
  auto systemList = getSystemListView();
  systemList->setPosition(getSystemId(system) * (float)Renderer::getScreenWidth(), systemList->getPosition().y());

  if (!system->hasGame()) {
    system = SystemData::getFirstSystemWithGame();
  }

	mState.viewing = SYSTEM_SELECT;
	mState.system = system;

	systemList->goToSystem(system, false);
	mCurrentView = systemList;
	mCurrentView->onShow();

	playViewTransition();

	RecalboxSystem::getInstance()->NotifySystem(*system);
}

void ViewController::goToNextGameList()
{
	assert(mState.viewing == GAME_LIST);
	SystemData* system = getState().getSystem();
	assert(system);
	SystemData* next = system->getNext();
	while(!next->getRootFolder()->hasChildren()) {
		next = next->getNext();
	}
	AudioManager::getInstance()->themeChanged(system->getNext()->getTheme());

	goToGameList(next);
}

void ViewController::goToPrevGameList()
{
	assert(mState.viewing == GAME_LIST);
	SystemData* system = getState().getSystem();
	assert(system);
	SystemData* prev = system->getPrev();
	while(!prev->getRootFolder()->hasChildren()) {
		prev = prev->getPrev();
	}
	AudioManager::getInstance()->themeChanged(prev->getTheme());
	goToGameList(prev);
}

bool ViewController::goToGameList(std::string& systemName) {
	SystemData* system = SystemData::getSystem(systemName);
	if (system != nullptr) {
		goToGameList(system);
		return true;
	}
	return false;
}

void ViewController::goToGameList(SystemData* system)
{
	if (mState.viewing == SYSTEM_SELECT)
	{
		// move system list
		auto sysList = getSystemListView();
		float offX = sysList->getPosition().x();
		int sysId = getSystemId(system);
		sysList->setPosition(sysId * (float)Renderer::getScreenWidth(), sysList->getPosition().y());
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
			mFavoritesOnly = Settings::getInstance()->getBool("FavoritesOnly");
		}
	}

	mState.viewing = GAME_LIST;
	mState.system = system;

	mCurrentView = getGameListView(system);
	playViewTransition();

  RecalboxSystem::getInstance()->NotifyGame(*getGameListView(system).get()->getCursor(), false, false);
}

void ViewController::updateFavorite(SystemData* system, FileData* file)
{
	IGameListView* view = getGameListView(system).get();
	if (Settings::getInstance()->getBool("FavoritesOnly"))
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

	std::string transition_style = Settings::getInstance()->getString("TransitionStyle");
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

void ViewController::launch(FileData* game, Vector3f center, std::string netplay, std::string core, std::string ip, std::string port)
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

	std::string transition_style = Settings::getInstance()->getString("TransitionStyle");

	auto launchFactory = [this, game, origCamera, netplay, core, ip, port] (std::function<void(std::function<void()>)> backAnimation) {
		return [this, game, origCamera, backAnimation, netplay, core, ip, port] {
      game->getSystem()->launchGame(mWindow, game, netplay, core, ip, port);
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
		setAnimation(new LambdaAnimation(fadeFunc, 800), 0, launchFactory([this, fadeFunc](std::function<void()> finishedCallback) {
			setAnimation(new LambdaAnimation(fadeFunc, 800), 0, finishedCallback, true);
		}));
	} else if (transition_style == "slide"){

		// move camera to zoom in on center + fade out, launch game, come back in
		setAnimation(new LaunchAnimation(mCamera, mFadeOpacity, center, 1500), 0, launchFactory([this, center](std::function<void()> finishedCallback) {
			setAnimation(new LaunchAnimation(mCamera, mFadeOpacity, center, 1000), 0, finishedCallback, true);
		}));
	} else {
		setAnimation(new LaunchAnimation(mCamera, mFadeOpacity, center, 10), 0, launchFactory([this, center](std::function<void()> finishedCallback) {
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

	if(detailed && ! (RecalboxConf::getInstance()->get("emulationstation.forcebasicgamelistview") == "1"))
		view = std::shared_ptr<IGameListView>(new DetailedGameListView(mWindow, system->getRootFolder(), system));
	else
		view = std::shared_ptr<IGameListView>(new BasicGameListView(mWindow, system->getRootFolder()));

	// uncomment for experimental "image grid" view
	//view = std::shared_ptr<IGameListView>(new GridGameListView(mWindow, system->getRootFolder()));

	view->setTheme(system->getTheme());

	std::vector<SystemData*>& sysVec = SystemData::sSystemVector;
	int id = std::find(sysVec.begin(), sysVec.end(), system) - sysVec.begin();
	view->setPosition(id * (float)Renderer::getScreenWidth(), (float)Renderer::getScreenHeight() * 2);

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

	mSystemListView = std::shared_ptr<SystemView>(new SystemView(mWindow));
	addChild(mSystemListView.get());
	mSystemListView->setPosition(0, (float)Renderer::getScreenHeight());
	return mSystemListView;
}


bool ViewController::input(InputConfig* config, Input input)
{
	if (mLockInput) return true;

	/* if we receive a button pressure for a non configured joystick, suggest the joystick configuration */
	if (!config->isConfigured() && (input.type == TYPE_BUTTON) )
	{
		mWindow->pushGui(new GuiDetectDevice(mWindow, false, nullptr));
		return true;
	}

	if (mCurrentView) return mCurrentView->input(config, input);

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
  Transform4x4f transInverse;
  transInverse.invert(trans);

	// camera position, position + size
	Vector3f viewStart = transInverse.translation();
	Vector3f viewEnd = transInverse * Vector3f((float)Renderer::getScreenWidth(), (float)Renderer::getScreenHeight(), 0);

	int vpl = (int)viewStart.x();
  int vpu = (int)viewStart.y();
  int vpr = (int)viewEnd.x() - 1;
  int vpb = (int)viewEnd.y() - 1;

	// draw systemview
  do
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
  }while(false);

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

	if(mWindow->peekGui() == this)
		mWindow->renderHelpPromptsEarly();

	// fade out
	if(mFadeOpacity != 0.0)
	{
		Renderer::setMatrix(parentTrans);
		Renderer::drawRect(0, 0, Renderer::getScreenWidth(), Renderer::getScreenHeight(), 0x00000000 | (unsigned char)(mFadeOpacity * 255));
	}
}

void ViewController::preload()
{
	for (auto& it : SystemData::sSystemVector)
	{
		getGameListView(it);
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
				bool hasGame = system->hasGame();
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
  Window *window = mWindow;
  window->renderShutdownScreen();
  delete ViewController::get();
  SystemData::deleteSystems();
  SystemData::loadConfig();
  GuiComponent *gui;
  while ((gui = window->peekGui()) != nullptr)
    window->removeGui(gui);
  ViewController::init(window);
  ViewController::get()->reloadAll();
  window->pushGui(ViewController::get());
  ViewController::get()->goToStart();
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
	if(mState.viewing == GAME_LIST)
	{
		mCurrentView = getGameListView(mState.getSystem());
	}else if(mState.viewing == SYSTEM_SELECT)
	{
		
		SystemData* system = mState.getSystem();
		goToSystemView(SystemData::sSystemVector.front());
		mSystemListView->goToSystem(system, false);
		mCurrentView = mSystemListView;
		
	}else{
		goToSystemView(SystemData::sSystemVector.front());
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

std::vector<HelpPrompt> ViewController::getHelpPrompts()
{
	std::vector<HelpPrompt> prompts;
	return mCurrentView ? mCurrentView->getHelpPrompts() : prompts;
}

HelpStyle ViewController::getHelpStyle()
{
	if(!mCurrentView)
		return GuiComponent::getHelpStyle();

	return mCurrentView->getHelpStyle();
}
