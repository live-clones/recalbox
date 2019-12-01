#pragma once

#include <utils/cplusplus/INoCopy.h>
#include "views/gamelist/IGameListView.h"
#include "views/SystemView.h"
#include "SplashView.h"

class SystemData;

// Used to smoothly transition the camera between multiple views (e.g. from system to system, from gamelist to gamelist).
class ViewController : public Gui, private INoCopy
{
public:
	static ViewController& Instance() { return *sInstance; };
	//inline static Window&getWindow(){return sInstance->mWindow;}

	ViewController(Window& window, SystemManager& systemManager);
  ~ViewController() override;

	/*!
	 * @brief Wake up the system if it is in a sleeping state
	 */
	void WakeUp() { mWindow.doWake(); }

	// If a basic view detected a metadata change, it can request to recreate
	// the current gamelist view (as it may change to be detailed).
	bool reloadGameListView(IGameListView* gamelist, bool reloadTheme = false);
	inline bool reloadGameListView(SystemData* system, bool reloadTheme = false) { return reloadGameListView(getGameListView(system).get(), reloadTheme); }
  static void deleteAndReloadAll();
	void setInvalidGamesList(SystemData* system);
	void setAllInvalidGamesList(SystemData* systemExclude);

	// Navigation.
	void goToNextGameList();
	void goToPrevGameList();
	bool goToGameList(std::string& systemName);
	void goToGameList(SystemData* system);
	void goToSystemView(SystemData* system);
	void goToStart();
  void goToQuitScreen();

	void onFileChanged(FileData* file, FileChangeType change);

	void updateFavorite(SystemData* system, FileData* file);

	// Plays a nice launch effect and launches the game at the end of it.
	// Once the game terminates, plays a return effect.
	void launch(FileData* game, Vector3f centerCameraOn = Vector3f(Renderer::getDisplayWidthAsFloat() / 2.0f, Renderer::getDisplayHeightAsFloat() / 2.0f, 0),
			const std::string& netplay = "", const std::string& core = "", const std::string& ip = "", const std::string& port = "");

	bool ProcessInput(const InputCompactEvent& event) override;
	void Update(int deltaTime) override;
	void Render(const Transform4x4f& parentTrans) override;

	enum class ViewMode
	{
		None,
		SplashScreen,
		SystemList,
		GameList,
	};

	struct State
	{
		ViewMode viewing;

		inline SystemData* getSystem() const { assert(viewing == ViewMode::GameList || viewing == ViewMode::SystemList); return system; }

	private:
		friend ViewController;
		SystemData* system;
	};

	inline const State& getState() const { return mState; }
	inline bool isViewing(ViewMode viewing) const { return mState.viewing == viewing; }

	bool getHelpPrompts(Help& help) override;
	void ApplyHelpStyle() override;

	std::shared_ptr<IGameListView> getGameListView(SystemData* system);
	SystemView& getSystemListView() { return mSystemListView; }

	Gui& CurrentUi() const { return *mCurrentView; }

	/*!
	 * @brief Get the progress interface
	 * @return Progress interface
	 */
	IProgressInterface& GetProgressInterface() { return mSplashView; }

private:
	static ViewController* sInstance;

	void playViewTransition();
	int getSystemId(SystemData* system);

  //! SystemManager instance
	SystemManager& mSystemManager;

	Gui* mCurrentView;
	std::map< SystemData*, std::shared_ptr<IGameListView> > mGameListViews;
	SystemView mSystemListView;
	SplashView mSplashView;
	std::map<SystemData*, bool> mInvalidGameList;
	
	Transform4x4f mCamera;
	float mFadeOpacity;
	bool mLockInput;
	bool mFavoritesOnly;

	State mState;
};
