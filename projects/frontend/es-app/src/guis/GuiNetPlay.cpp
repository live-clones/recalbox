//
// Created by xizor on 20/05/18.
//

#include <RecalboxConf.h>
#include <views/ViewController.h>
#include "GuiNetPlay.h"
#include "components/TextComponent.h"
#include "components/ButtonComponent.h"
#include "components/MenuComponent.h"
#include "GuiNetPlayClientPasswords.h"
#include <netplay/NetPlayThread.h>
#include <systems/SystemManager.h>
#include <GameNameMapManager.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/pointer.h>
#include <utils/Files.h>
#include <utils/locale/LocaleHelper.h>
#include <liboping/src/oping.h>
#include <netdb.h>

#define BUTTON_GRID_VERT_PADDING Renderer::Instance().DisplayHeightAsFloat() * 0.025f
#define BUTTON_GRID_HORIZ_PADDING 10

#define TITLE_HEIGHT (mTitle->getFont()->getLetterHeight() + TITLE_VERT_PADDING)

GuiNetPlay::GuiNetPlay(WindowManager& window, SystemManager& systemManager)
  : Gui(window),
    mSystemManager(systemManager),
    mLobbyLoaded(false),
    mSender(this),
    mBackground(window, Path(":/frame.png")),
    mBusyAnim(window),
    mGrid(window, Vector2i(1, 3)),
    mGridMeta(new ComponentGrid(window, Vector2i(2, 1))),
    mGridMetaRight(new ComponentGrid(window, Vector2i(2, 11))),
    mList(nullptr)
{
  addChild(&mBackground);
  addChild(&mGrid);

  mMenuTheme = MenuThemeData::getInstance()->getCurrentTheme();

  mBackground.setImagePath(mMenuTheme->menuBackground.path);
  mBackground.setCenterColor(mMenuTheme->menuBackground.color);
  mBackground.setEdgeColor(mMenuTheme->menuBackground.color);

  mTitle = std::make_shared<TextComponent>(mWindow, _("NETPLAY"), mMenuTheme->menuTitle.font,
                                           mMenuTheme->menuTitle.color, TextAlignment::Center);
  mGrid.setEntry(mTitle, Vector2i(0, 0), false);

  mButtons.push_back(std::make_shared<ButtonComponent>(mWindow, _("CLOSE"), _("CLOSE"), [this]
  { Close(); }));

  mButtonGrid = makeButtonGrid(mWindow, mButtons);
  mGrid.setEntry(mButtonGrid, Vector2i(0, 2), true, false);

  updateSize();
  mBusyAnim.setSize(mSize);
  setPosition((Renderer::Instance().DisplayWidthAsFloat() - mSize.x()) / 2,
              (Renderer::Instance().DisplayHeightAsFloat() - mSize.y()) / 2);

  // start the thread if not aleaady done
  Thread::Start("GuiNetPlay");
}

GuiNetPlay::~GuiNetPlay()
{
  Thread::Stop();
}

void GuiNetPlay::populateGrid()
{
  if (!mLobbyList.empty())
  {
    if (mList == nullptr)
    {
      mList = std::make_shared<ComponentList>(mWindow);
      mGridMeta->setEntry(mList, Vector2i(0, 0), true);
      mGridMeta->setColWidthPerc(0, 0.57);
      mGrid.setEntry(mGridMeta, Vector2i(0, 1), true);

      mMetaTextLblUsername = std::make_shared<TextComponent>(mWindow, "    " + _("Username") + " : ",
                                                             mMenuTheme->menuTextSmall.font,
                                                             mMenuTheme->menuTextSmall.color, TextAlignment::Left);
      mMetaTextUsername = std::make_shared<TextComponent>(mWindow, "", mMenuTheme->menuTextSmall.font,
                                                          mMenuTheme->menuTextSmall.color, TextAlignment::Left);
      mGridMetaRight->setEntry(mMetaTextLblUsername, Vector2i(0, 0), false, true);
      mGridMetaRight->setEntry(mMetaTextUsername, Vector2i(1, 0), false, true);
      mMetaTextLblCountry = std::make_shared<TextComponent>(mWindow, "    " + _("Country") + " : ",
                                                            mMenuTheme->menuTextSmall.font,
                                                            mMenuTheme->menuTextSmall.color, TextAlignment::Left);
      mMetaTextCountry = std::make_shared<TextComponent>(mWindow, "", mMenuTheme->menuTextSmall.font,
                                                         mMenuTheme->menuTextSmall.color, TextAlignment::Left);
      mGridMetaRight->setEntry(mMetaTextLblCountry, Vector2i(0, 1), false, true);
      mGridMetaRight->setEntry(mMetaTextCountry, Vector2i(1, 1), false, true);
      mMetaTextLblRomHash = std::make_shared<TextComponent>(mWindow, "    " + _("Rom hash") + " : ",
                                                            mMenuTheme->menuTextSmall.font,
                                                            mMenuTheme->menuTextSmall.color, TextAlignment::Left);
      mMetaTextRomHash = std::make_shared<TextComponent>(mWindow, "", mMenuTheme->menuTextSmall.font,
                                                         mMenuTheme->menuTextSmall.color, TextAlignment::Left);
      mGridMetaRight->setEntry(mMetaTextLblRomHash, Vector2i(0, 2), false, true);
      mGridMetaRight->setEntry(mMetaTextRomHash, Vector2i(1, 2), false, true);
      mMetaTextLblRomFile = std::make_shared<TextComponent>(mWindow, "    " + _("Rom file") + " : ",
                                                            mMenuTheme->menuTextSmall.font,
                                                            mMenuTheme->menuTextSmall.color, TextAlignment::Left);
      mMetaTextRomFile = std::make_shared<TextComponent>(mWindow, "", mMenuTheme->menuTextSmall.font,
                                                         mMenuTheme->menuTextSmall.color, TextAlignment::Left);
      mGridMetaRight->setEntry(mMetaTextLblRomFile, Vector2i(0, 3), false, true);
      mGridMetaRight->setEntry(mMetaTextRomFile, Vector2i(1, 3), false, true);
      mMetaTextLblCore = std::make_shared<TextComponent>(mWindow, "    " + _("Core") + " : ",
                                                         mMenuTheme->menuTextSmall.font, mMenuTheme->menuTextSmall.color,
                                                         TextAlignment::Left);
      mMetaTextCore = std::make_shared<TextComponent>(mWindow, "", mMenuTheme->menuTextSmall.font,
                                                      mMenuTheme->menuTextSmall.color, TextAlignment::Left);
      mGridMetaRight->setEntry(mMetaTextLblCore, Vector2i(0, 4), false, true);
      mGridMetaRight->setEntry(mMetaTextCore, Vector2i(1, 4), false, true);
      mMetaTextLblCoreVer = std::make_shared<TextComponent>(mWindow, "    " + _("Core ver.") + " : ",
                                                            mMenuTheme->menuTextSmall.font,
                                                            mMenuTheme->menuTextSmall.color, TextAlignment::Left);
      mMetaTextCoreVer = std::make_shared<TextComponent>(mWindow, "", mMenuTheme->menuTextSmall.font,
                                                         mMenuTheme->menuTextSmall.color, TextAlignment::Left);
      mGridMetaRight->setEntry(mMetaTextLblCoreVer, Vector2i(0, 5), false, true);
      mGridMetaRight->setEntry(mMetaTextCoreVer, Vector2i(1, 5), false, true);
      mMetaTextLblLatency = std::make_shared<TextComponent>(mWindow, "    " + _("Latency") + " : ",
                                                            mMenuTheme->menuTextSmall.font,
                                                            mMenuTheme->menuTextSmall.color, TextAlignment::Left);
      mMetaTextLatency = std::make_shared<TextComponent>(mWindow, "", mMenuTheme->menuTextSmall.font,
                                                         mMenuTheme->menuTextSmall.color, TextAlignment::Left);
      mGridMetaRight->setEntry(mMetaTextLblLatency, Vector2i(0, 6), false, true);
      mGridMetaRight->setEntry(mMetaTextLatency, Vector2i(1, 6), false, true);
      mMetaTextLblRAVer = std::make_shared<TextComponent>(mWindow, "    " + _("RA ver.") + " : ",
                                                          mMenuTheme->menuTextSmall.font, mMenuTheme->menuTextSmall.color,
                                                          TextAlignment::Left);
      mMetaTextRAVer = std::make_shared<TextComponent>(mWindow, "", mMenuTheme->menuTextSmall.font,
                                                       mMenuTheme->menuTextSmall.color, TextAlignment::Left);
      mGridMetaRight->setEntry(mMetaTextLblRAVer, Vector2i(0, 7), false, true);
      mGridMetaRight->setEntry(mMetaTextRAVer, Vector2i(1, 7), false, true);
      mMetaTextLblHostArch = std::make_shared<TextComponent>(mWindow, "    " + _("Host arch.") + " : ",
                                                             mMenuTheme->menuTextSmall.font,
                                                             mMenuTheme->menuTextSmall.color, TextAlignment::Left);
      mMetaTextHostArch = std::make_shared<TextComponent>(mWindow, "", mMenuTheme->menuTextSmall.font,
                                                          mMenuTheme->menuTextSmall.color, TextAlignment::Left);
      mGridMetaRight->setEntry(mMetaTextLblHostArch, Vector2i(0, 8), false, true);
      mGridMetaRight->setEntry(mMetaTextHostArch, Vector2i(1, 8), false, true);
      mMetaTextLblCanJoin = std::make_shared<TextComponent>(mWindow, "    " + _("Can join") + " : ",
                                                            mMenuTheme->menuTextSmall.font,
                                                            mMenuTheme->menuTextSmall.color, TextAlignment::Left);
      mMetaTextCanJoin = std::make_shared<TextComponent>(mWindow, "", mMenuTheme->menuTextSmall.font,
                                                         mMenuTheme->menuTextSmall.color, TextAlignment::Left);
      mGridMetaRight->setEntry(mMetaTextLblCanJoin, Vector2i(0, 10), false, true);
      mGridMetaRight->setEntry(mMetaTextCanJoin, Vector2i(1, 10), false, true);
      mGridMeta->setEntry(mGridMetaRight, Vector2i(1, 0), false, true, Vector2i(1, 1), Borders::Left);
      mGridMetaRight->setColWidthPerc(1, 0.60, true);
    }

    ComponentListRow row;
    std::shared_ptr<Component> ed;

    std::sort(mLobbyList.begin(), mLobbyList.end(), [](const LobbyGame& game1, const LobbyGame& game2) { return game1.mFormattedName < game2.mFormattedName; });

    mList->clear();
    for (auto& game : mLobbyList)
    {
      row.elements.clear();

      ed = std::make_shared<TextComponent>(mWindow, GetFormattedName(game), mMenuTheme->menuText.font, mMenuTheme->menuText.color,
                                           TextAlignment::Left);
      row.addElement(ed, true);
      row.makeAcceptInputHandler([this]
                                 { launch(); });
      addRow(row);
    }

    populateGridMeta(0);
    mList->setCursorChangedCallback([this](CursorState state)
                                    {
                                      (void) state;
                                      populateGridMeta(mList->getCursor());
                                    });
    mList->setFocusLostCallback([this]
                                {
                                  mMetaTextUsername->setText("");
                                  mMetaTextCountry->setText("");
                                  mMetaTextRomHash->setText("");
                                  mMetaTextRomFile->setText("");
                                  mMetaTextCore->setText("");
                                  mMetaTextCoreVer->setText("");
                                  mMetaTextLatency->setText("");
                                  mMetaTextRAVer->setText("");
                                  mMetaTextHostArch->setText("");
                                  mMetaTextCanJoin->setText("");
                                });
    mList->setFocusGainedCallback([this]
                                  { populateGridMeta(mList->getCursor()); });

    mGrid.moveCursor(Vector2i(0, -1));
  }
  else
  {
    auto text = std::make_shared<TextComponent>(mWindow, _("NO GAMES OR NO CONNECTION"), mMenuTheme->menuText.font,
                                                mMenuTheme->menuText.color, TextAlignment::Center);
    mGrid.setEntry(text, Vector2i(0, 1), true);
  }
}

//called when changing cursor in mList
void GuiNetPlay::populateGridMeta(int i)
{
  if ((i < 0) || (i >= (int)mLobbyList.size()))
  {
    return;
  }
  LobbyGame& game = mLobbyList[i];

  bool hashMatch = false;
  if (game.mGame != nullptr)
  {
    hashMatch = game.mGame->Hash() == game.mGameCRC;
  }
  const CoreInfo& CoreInfo = GetCoreInfo(game.mCoreLongName);
  bool coreVerMatch = CoreInfo.Version() == game.mCoreVersion;
  bool coreMatch = !CoreInfo.Empty();

  std::string username = game.mUserName.empty() ? "N/A" : game.mUserName;
  std::string frontend = game.mFrontEnd;
  if (game.mIsRecalbox) frontend.append(" \uF200");

  mMetaTextUsername->setText(username);
  mMetaTextCountry->setText(game.mCountry.empty() ? "N/A" : game.mCountry);

  if (hashMatch) mMetaTextRomHash->setText("\uf1c0 " + _("Match"));
  else           mMetaTextRomHash->setText("\uf1c2 " + _("No Match"));

  if (game.mGame != nullptr) mMetaTextRomFile->setText("\uf1c0 " + _("Match"));
  else                       mMetaTextRomFile->setText("\uf1c2 " + _("No Match"));

  if (coreMatch) mMetaTextCore->setText("\uf1c0 " + game.mCoreLongName);
  else           mMetaTextCore->setText("\uf1c2 " + game.mCoreLongName);

  if (coreVerMatch) mMetaTextCoreVer->setText("\uf1c0 " + (game.mCoreVersion.empty() ? std::string("N/A") : game.mCoreVersion));
  else              mMetaTextCoreVer->setText("\uf1c2 " + (game.mCoreVersion.empty() ? std::string("N/A") : game.mCoreVersion));

  if (game.mPingTimeInMs < 0)         mMetaTextLatency->setText("\uF1c9 " + _("unknown"));
  else if (game.mPingTimeInMs < 80)   mMetaTextLatency->setText("\uF1c8 " + _("good") + " (" + std::to_string(game.mPingTimeInMs) + "ms)");
  else if (game.mPingTimeInMs < 150)  mMetaTextLatency->setText("\uF1c7 " + _("medium") + " (" + std::to_string(game.mPingTimeInMs) + "ms)");
  else                                mMetaTextLatency->setText("\uF1c6 " + _("bad") + " ( > 150ms)");

  mMetaTextRAVer->setText(game.mRetroarchVersion.empty() ? "N/A" : game.mRetroarchVersion);
  mMetaTextHostArch->setText(frontend);
  if (game.mGame != nullptr)
  {
    if (hashMatch && coreMatch)
    {
      mMetaTextCanJoin->setText("\uf1c0 " + _("Rom, hash and core match"));
      mMetaTextCanJoin->setColor(0x26B14AFF);
    }
    else if (coreMatch)
    {
      mMetaTextCanJoin->setText("\uf1c1 " + _("Rom and core match"));
      mMetaTextCanJoin->setColor(0x36A9E0FF);
    }
    else
    {
      mMetaTextCanJoin->setText("\uf1c2 " + _("No core match"));
      mMetaTextCanJoin->setColor(0xDC1F26FF);
    }
  }
  else
  {
    mMetaTextCanJoin->setText("\uf1c2 " + _("No rom match"));
    mMetaTextCanJoin->setColor(0xDC1F26FF);
  }
}

void GuiNetPlay::launch()
{
  int index = mList->getCursor();

  LobbyGame game = mLobbyList[index];

  if (!game.mCoreLongName.empty() && game.mGame != nullptr)
    mWindow.pushGui(new GuiNetPlayClientPasswords(mWindow, game));
}

void GuiNetPlay::LoadCoreMap()
{
  std::string content = Files::LoadFile(RootFolders::DataRootFolder / "system/configs/retroarch.corenames");
  for(std::string& line : Strings::Split(content, '\n'))
  {
    Strings::Vector parts = Strings::Split(line, ';');
    if (parts.size() == 3)
      mCoreList.push_back({ parts[0], parts[1], parts[2] });
  }
}

const GuiNetPlay::CoreInfo& GuiNetPlay::GetCoreInfo(const std::string& name)
{
  for(const CoreInfo& info : mCoreList)
    if (info.LongName() == name)
      return info;

  static CoreInfo sEmptyCoreInfo;
  return sEmptyCoreInfo;
}

float GuiNetPlay::getButtonGridHeight() const
{
  auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
  return (mButtonGrid ? mButtonGrid->getSize().y() : menuTheme->menuText.font->getHeight() + BUTTON_GRID_VERT_PADDING);
}

bool GuiNetPlay::ProcessInput(const InputCompactEvent& event)
{
  if (event.CancelPressed())
  {
    Close();
  }
  if (event.StartPressed())
  {
    // restart the thread if not aleaady done
    if (Thread::IsDone())
    {
      if (mList != nullptr) mList->clear();
      Thread::Start("GuiNetPlay");
    }
  }
  return Component::ProcessInput(event);
}

void GuiNetPlay::updateSize()
{
  const float height = Renderer::Instance().DisplayHeightAsFloat() * 0.8f;
  const float width = Renderer::Instance().DisplayWidthAsFloat() * 0.9f;
  setSize(width, height);
}

void GuiNetPlay::onSizeChanged()
{
  mBackground.fitTo(mSize, Vector3f::Zero(), Vector2f(-32, -32));

  // update grid row/col sizes
  mGrid.setRowHeightPerc(0, TITLE_HEIGHT / mSize.y());
  mGrid.setRowHeightPerc(2, getButtonGridHeight() / mSize.y());

  mGrid.setSize(mSize);
}

bool GuiNetPlay::getHelpPrompts(Help& help)
{
  mGrid.getHelpPrompts(help);
  help.Set(Help::Cancel(), _("BACK"))
      .Set(Help::Valid(), _("LAUNCH"))
      .Set(HelpType::Start, _("RESCAN"));
  return true;
}

void GuiNetPlay::Update(int deltaTime)
{
  Component::Update(deltaTime);
  mBusyAnim.Update(deltaTime);
}

void GuiNetPlay::Render(const Transform4x4f& parentTrans)
{
  Transform4x4f trans = parentTrans * getTransform();

  renderChildren(trans);

  Renderer::SetMatrix(trans);
  Renderer::DrawRectangle(0.f, 0.f, mSize.x(), mSize.y(), 0x00000011);

  if (!mLobbyLoaded)
    mBusyAnim.Render(trans);
}

FileData* GuiNetPlay::FindGame(const std::string& game)
{
  for (auto* tmp : mSystemManager.GetAllSystemList())
    if (tmp->Descriptor().HasNetPlayCores())
    {
      FileData* result = tmp->MasterRoot().LookupGame(game, FileData::SearchAttributes::ByName |
                                                            FileData::SearchAttributes::ByHash);
      if (result != nullptr) return result;
    }
  return nullptr;
}

void GuiNetPlay::ParseLobby()
{
  mLobbyList.clear();

  for(;;)
  {
    auto json_req = RecalboxSystem::execute(NetPlayThread::getLobbyListCommand());
    if (json_req.second == 0)
    {
      rapidjson::Document json;
      json.Parse(json_req.first.c_str());

      for (auto& item : json.GetArray())
      {
        LobbyGame game;
        const rapidjson::Value& fields = item["fields"];

        // Take only recalbox games
        //if (strstr(fields["frontend"].GetString(), "@RECALBOX") == nullptr)
        //  continue;

        FileData* tmp = nullptr;
        if (fields["game_crc"] != "00000000")
          tmp = FindGame(fields["game_crc"].GetString());
        if (tmp == nullptr)
          tmp = FindGame(fields["game_name"].GetString());

        game.mGame = tmp;
        game.mIp = fields["ip"].GetString();
        game.mPort = fields["port"].GetInt();
        game.mGameName = fields["game_name"].GetString();
        game.mGameCRC = fields["game_crc"].GetString();
        game.mCoreLongName = fields["core_name"].GetString();
        game.mCoreVersion = fields["core_version"].GetString();
        game.mUserName = fields["username"].GetString();
        game.mFrontEnd = fields["frontend"].GetString();
        game.mRetroarchVersion = fields["retroarch_version"].GetString();
        game.mCountry = fields["country"].GetString();
        game.mHostMethod = fields["host_method"].GetInt();
        game.mMitmIp = fields["mitm_ip"].GetString();
        game.mMitmPort = fields["mitm_port"].GetInt();
        game.mNeedPlayerPassword = fields["has_password"].GetBool();
        game.mNeedViewerPassword = fields["has_spectate_password"].GetBool();
        game.mIsRecalbox = (strstr(game.mFrontEnd.data(), "@RECALBOX") != nullptr);
        if (game.mIsRecalbox)
          Strings::ReplaceAllIn(game.mFrontEnd, "@RECALBOX", "");
        game.mFormattedName = GetFormattedName(game);

        const CoreInfo& coreInfo = GetCoreInfo(game.mCoreLongName);
        game.mCoreShortName = coreInfo.Empty() ? "invalid" : coreInfo.ShortName();

        mLobbyList.push_back(game);
      }
    }
    break;
  }
}

void GuiNetPlay::Run()
{
  { LOG(LogDebug) << "[NetplayGui] Loading core list"; }
  LoadCoreMap();
  { LOG(LogDebug) << "[NetplayGui] Start getting lobby list"; }
  ParseLobby();
  mSender.Call((int)MessageType::LobbyLoaded);

  { LOG(LogDebug) << "[NetplayGui] Start pinging players"; }
  // Build ping object
  pingobj_t* pinger = ping_construct();
  // Add all ip
  for (auto& game : mLobbyList)
  {
    if (ping_host_add(pinger, game.mIp.data()) != 0)
    { LOG(LogWarning) << "[Ping] Error pinging Host: " << game.mIp; }
    game.mPingTimeInMs = -1;
  }
  // Set timeout to 150ms
  double timeout = 0.150; // 150 ms
  if (ping_setopt(pinger, PING_OPT_TIMEOUT, &timeout) != 0)
  { LOG(LogWarning) << "[Ping] Error setting ping timeout to: " << (int)(timeout * 1000.0); }
  // Ping!
  int r = ping_send(pinger);
  { LOG(LogWarning) << "[Ping] Number of ping response: " << r; }
  if (r < 0) LOG(LogError) << "[Ping] Ping error: " << ping_get_error(pinger);
  // Iterate results
  if (r > 0)
    for (pingobj_iter_t *iter = ping_iterator_get(pinger); iter != nullptr; iter = ping_iterator_next(iter))
    {
      // Get ip
      char host[NI_MAXHOST];
      size_t hostlen = sizeof(host);
      ping_iterator_get_info(iter, PING_INFO_HOSTNAME, host, &hostlen);
      { LOG(LogWarning) << "[Ping] Host: " << std::string(host); }
      // Get latency
      double latency = -1;
      size_t latencylen = sizeof(latency);
      ping_iterator_get_info(iter, PING_INFO_LATENCY, &latency, &latencylen);
      { LOG(LogWarning) << "[Ping] Ms: " << (latency < 0 ? 1000 : (int)latency); }
      // Lookup and store
      for (auto& game : mLobbyList)
        if (game.mIp == std::string(host))
          game.mPingTimeInMs = latency < 0 ? 1000 : (int)latency;
    }
  // Destroy pinger
  ping_destroy(pinger);

  { LOG(LogDebug) << "[NetplayGui] Start sleeping"; }
  mSender.Call((int)MessageType::Ping);
}

void GuiNetPlay::ReceiveSyncCallback(const SDL_Event& event)
{
  switch((MessageType)event.user.code)
  {
    case MessageType::LobbyLoaded:
    {
      mLobbyLoaded = true;
      populateGrid();
      break;
    }
    case MessageType::Ping:
    {
      populateGridMeta(mList->getCursor());
      break;
    }
  }
}

std::string GuiNetPlay::GetFormattedName(const LobbyGame& game)
{
  // Get game name
  std::string gameName = game.mGameName;
  static std::string arcadesCores("FinalBurn Neo,MAME 2000,MAME 2003,MAME 2003-Plus,MAME 2010,MAME 2015,MAME 2016");
  if (arcadesCores.find(gameName) != std::string::npos && game.mGame != nullptr)
    gameName = GameNameMapManager::Rename(game.mGame->System(), game.mGameName);

  // Add decorations
  if (game.mNeedPlayerPassword)
  {
    gameName.insert(0, "\uf070 ");
    if (game.mNeedViewerPassword)
      gameName.insert(0, "\uf070");
  }
  if (game.mIsRecalbox)
    gameName.insert(0, " \uf200");

  // Matching status
  if (game.mGame != nullptr)
  {
    if (game.mGame->Hash() == game.mGameCRC)        gameName.insert(0, "\uf1c0 ");
    else if (!GetCoreInfo(game.mCoreLongName).Empty()) gameName.insert(0, "\uf1c1 ");
    else gameName.insert(0, "\uf1c2 ");
  }
  else gameName.insert(0, "\uf1c2 ");

  return gameName;
}

