//
// Created by xizor on 20/05/18.
//
#pragma once

#include <components/ComponentList.h>
#include "GuiComponent.h"
#include "components/NinePatchComponent.h"
#include "components/ComponentGrid.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <recalbox/RecalboxSystem.h>
#include <boost/thread.hpp>
#include <MenuThemeData.h>
#include <components/BusyComponent.h>
#include <utils/os/system/Mutex.h>
#include "FileData.h"

class TextComponent;

class ButtonComponent;

#define TITLE_VERT_PADDING (Renderer::getDisplayHeightAsFloat()*0.0637f)

class LobbyGame
{
  public:
    FileData* mGame;
    std::string mIp;
    std::string mPort;
    std::string mGameName;
    std::string mGameCRC;
    std::string mCoreName;
    std::string mCoreVersion;
    std::string mUserName;
    std::string mFrontEnd;
    std::string mRetroarchVersion;
    std::string mCountry;
    std::string mHostMethod;
    std::string mMitmIp;
    std::string mMitmPort;
    int mPingTimeInMs;

    LobbyGame()
      : mGame(nullptr),
        mPingTimeInMs(0)
    {
    }
};

class GuiNetPlay : public GuiComponent
{
  public:
    explicit GuiNetPlay(Window* window);

    ~GuiNetPlay() override;

    inline void addRow(const ComponentListRow& row, bool setCursorHere = false, bool updateGeometry = true)
    {
      mList->addRow(row, setCursorHere, updateGeometry);
      if (updateGeometry) updateSize();
    }

    void populateGrid();

    void populateGridMeta(int i);

    void launch();

    static std::pair<std::string, std::string> getCoreInfo(const std::string& name);

    float getButtonGridHeight() const;

    void updateSize();

    bool input(InputConfig* config, Input input) override;

    std::vector<HelpPrompt> getHelpPrompts() override;

    void onSizeChanged() override;

    void update(int deltaTime) override;

    void render(const Transform4x4f& parentTrans) override;

    static void stopLobbyThread()
    {
      if (mlobbyThreadHandle != nullptr)
      {
        mMustQuit = true;
        mlobbyThreadHandle->join();
      }
    }

  private:

    enum class State
    {
        WaitingForLobby,
        Initializing,
        Ready,
    };

    State mState;

    /*!
     * @brief Ping a remote host
     * @param ip IP to ping
     * @return time in milliseconds
     */
    static int pingHost(const std::string& ip);

    /*!
     * @brief Look for a game in all gamelist available
     * @param game game or hash
     * @return FileData of the game is found, otherwise nullptr
     */
    static FileData* findGame(const std::string& game);

    /*!
     * @brief Get the playable games from the lobby and fill the list
     */
    static void parseLobby();

    /*!
     * @brief Netplayable Game list
     */
    static std::vector<LobbyGame> mLobbyList;

    /*!
     * @brief Lobby thread handle
     */
    static boost::thread* mlobbyThreadHandle;

    /*!
     * @brief Protect lobby list access againt multiple accesses
     */
    static Mutex mLobbyProtector;

    /*!
     * @brief True if the thread must quit
     */
    static bool mMustQuit;

    /*!
     * @brief True if the thread is active and must seek the lobby
     * when set tp false the thread must sleep
     */
    static bool mIsActive;

    /*!
     * @brief If true, the lobby list has been loaded
     * Set this flag to false to force a reload
     */
    static bool mIsLobbyLoaded;

    /*!
     * @brief Wake up the lobby thread and make it search for active game room continuously
     */
    static void activateLobbyThread() { mIsActive = true; }

    /*!
     * @brief Send the lobby thread back to sleep
     */
    static void deactivateLobbyThread() { mIsActive = false; }

    /*!
     * @brief Lobby thread - Collect data from lobby
     */
    static void lobbyEngine(void* param);

    /*!
     * @brief Start lobby engine IIF it's not already started
     * This thread must be stopped before the application exit
     */
    static void startLobbyThread();

    NinePatchComponent mBackground;
    BusyComponent mBusyAnim;
    ComponentGrid mGrid;
    std::shared_ptr<MenuTheme> mMenuTheme;

    std::shared_ptr<ComponentGrid> mGridMeta;
    std::shared_ptr<ComponentGrid> mGridMetaRight;
    std::shared_ptr<ComponentGrid> mButtonGrid;
    std::vector<std::shared_ptr<ButtonComponent> > mButtons;
    std::shared_ptr<TextComponent> mTitle;
    std::shared_ptr<ComponentList> mList;
    std::shared_ptr<TextComponent> mMetaTextLblUsername;
    std::shared_ptr<TextComponent> mMetaTextUsername;
    std::shared_ptr<TextComponent> mMetaTextLblCountry;
    std::shared_ptr<TextComponent> mMetaTextCountry;
    std::shared_ptr<TextComponent> mMetaTextLblRomHash;
    std::shared_ptr<TextComponent> mMetaTextRomHash;
    std::shared_ptr<TextComponent> mMetaTextLblRomFile;
    std::shared_ptr<TextComponent> mMetaTextRomFile;
    std::shared_ptr<TextComponent> mMetaTextLblCore;
    std::shared_ptr<TextComponent> mMetaTextCore;
    std::shared_ptr<TextComponent> mMetaTextLblCoreVer;
    std::shared_ptr<TextComponent> mMetaTextCoreVer;
    std::shared_ptr<TextComponent> mMetaTextLblLatency;
    std::shared_ptr<TextComponent> mMetaTextLatency;
    std::shared_ptr<TextComponent> mMetaTextLblRAVer;
    std::shared_ptr<TextComponent> mMetaTextRAVer;
    std::shared_ptr<TextComponent> mMetaTextLblHostArch;
    std::shared_ptr<TextComponent> mMetaTextHostArch;
    std::shared_ptr<TextComponent> mMetaTextLblCanJoin;
    std::shared_ptr<TextComponent> mMetaTextCanJoin;
};
