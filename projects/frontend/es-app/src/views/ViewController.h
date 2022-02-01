#pragma once

#include <utils/cplusplus/INoCopy.h>
#include <utils/cplusplus/StaticLifeCycleControler.h>
#include <views/GameClipView.h>
#include "views/gamelist/IGameListView.h"
#include "views/SystemView.h"
#include "SplashView.h"
#include "views/crt/CrtView.h"
#include <systems/GameLinkedData.h>

class SystemData;

// Used to smoothly transition the camera between multiple views (e.g. from system to system, from gamelist to gamelist).
class ViewController : public StaticLifeCycleControler<ViewController>, public Gui, private INoCopy
{
public:
	ViewController(WindowManager& window, SystemManager& systemManager);
  ~ViewController() override = default;

    /*!
     * @brief Wake up the system if it is in a sleeping state
     */
    void WakeUp() { mWindow.DoWake(); }

    /*!
     * @brief Check bios and call LaunchAnimated
     * @param game game to launch
     * @param netplay optional netplay data
     * @param centerCameraOn optional camera target point
     */
    void Launch(FileData* game, const GameLinkedData& netplay, const Vector3f& centerCameraOn, bool forceLaunch = false);

	// If a basic view detected a metadata change, it can request to recreate
	// the current gamelist view (as it may change to be detailed).
	bool reloadGameListView(IGameListView* gamelist, bool reloadTheme = false);
	inline bool reloadGameListView(SystemData* system, bool reloadTheme = false) { return reloadGameListView(getGameListView(system).get(), reloadTheme); }
	void setInvalidGamesList(SystemData* system);
	void setAllInvalidGamesList(SystemData* systemExclude);

	// Navigation.
	void goToNextGameList();
	void goToPrevGameList();
	bool goToGameList(std::string& systemName);
	void goToGameList(SystemData* system);
  void goToSystemView(SystemData* system);
	void goToGameClipView();
	void quitGameClipView();
  void goToCrtView();
  void quitCrtView();
	void selectGamelistAndCursor(FileData* file);
	void goToStart();
  void goToQuitScreen();

	void onFileChanged(FileData* file, FileChangeType change);

	void updateFavorite(SystemData* system, FileData* file);

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
		bool gameClipRunning;
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
	void playViewTransition();
	int getSystemId(SystemData* system);

  //! Game linked data internal instance
  GameLinkedData mGameLinkedData;

  //! SystemManager instance
	SystemManager& mSystemManager;

	Gui* mCurrentView;
	std::map< SystemData*, std::shared_ptr<IGameListView> > mGameListViews;
	SystemView mSystemListView;
	SplashView mSplashView;
	GameClipView* mGameClipView;
  CrtView* mCrtView;
	std::map<SystemData*, bool> mInvalidGameList;
	
	Transform4x4f mCamera;
	float mFadeOpacity;
	bool mLockInput;

	State mState;

    /*!
     * @brief Check bios and call LaunchAnimated
     * @param game game to launch
     * @param netplay optional netplay data
     * @param centerCameraOn optional camera target point
     */
    void LaunchCheck(FileData* game, const Vector3f& centerCameraOn, bool forceLaunch = false);

    /*!
     * @brief Run animation and call LaunchActually
     * @param game game to launch
     * @param netplay optional netplay data
     * @param centerCameraOn optional camera target point
     */
    void LaunchAnimated(FileData* game, const EmulatorData& emulator, const Vector3f& centerCameraOn);

    /*!
     * @brief Actually run the game :)
     * @param game game to launch
     * @param netplay optional netplay data
     */
    void LaunchActually(FileData* game, const EmulatorData& emulator);
};
