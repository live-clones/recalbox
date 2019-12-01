//
// Created by xizor on 20/05/18.
//
#pragma once

#include <components/ComponentList.h>
#include "guis/Gui.h"
#include "components/NinePatchComponent.h"
#include "components/ComponentGrid.h"
#include <recalbox/RecalboxSystem.h>
#include <MenuThemeData.h>
#include <components/BusyComponent.h>
#include <utils/os/system/Thread.h>
#include <utils/sdl2/ISyncronousEvent.h>
#include <utils/sdl2/SyncronousEvent.h>
#include "games/FileData.h"

class TextComponent;
class ButtonComponent;
class SystemManager;

#define TITLE_VERT_PADDING (Renderer::getDisplayHeightAsFloat()*0.0637f)

class LobbyGame
{
  public:
    FileData*   mGame;
    std::string mGameName;
    std::string mGameCRC;
    std::string mCoreName;
    std::string mCoreVersion;
    std::string mUserName;
    std::string mFrontEnd;
    std::string mRetroarchVersion;
    std::string mCountry;
    std::string mIp;
    std::string mMitmIp;
    int         mPort;
    int         mMitmPort;
    int         mHostMethod;
    int         mPingTimeInMs;

    LobbyGame()
      : mGame(nullptr),
        mPort(0),
        mMitmPort(0),
        mHostMethod(0),
        mPingTimeInMs(0)
    {
    }
};

class GuiNetPlay : public Gui, private Thread, private ISyncronousEvent
{
  public:
    explicit GuiNetPlay(Window&window, SystemManager& systemManager);

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

    bool ProcessInput(const InputCompactEvent& event) override;

    bool getHelpPrompts(Help& help) override;

    void onSizeChanged() override;

    void Update(int deltaTime) override;

    void Render(const Transform4x4f& parentTrans) override;

  private:
    enum class MessageType
    {
      LobbyLoaded,
      Ping,
    };

    /*
     * Thread Implementation
     */

    /*!
     * @brief Main thread routine
     */
    void Run() override;

    /*
     * Synchronous event
     */

    /*!
     * @brief Receive SDL event from the main thread
     * @param event SDL event
     */
    void ReceiveSyncCallback(const SDL_Event& event) override;

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
    FileData* findGame(const std::string& game);

    /*!
     * @brief Get the playable games from the lobby and fill the list
     */
    void parseLobby();

    //! SystemManager instance
    SystemManager& mSystemManager;

    /*!
     * @brief Netplayable Game list
     */
    std::vector<LobbyGame> mLobbyList;

    bool mLobbyLoaded;

    SyncronousEvent mSender;

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
