//
// Created by xizor on 01/06/18.
//

#include <RecalboxConf.h>
#include <recalbox/RecalboxSystem.h>
#include <guis/GuiMsgBox.h>
#include <systems/SystemManager.h>
#include <utils/hash/Crc32File.h>
#include "GuiHashStart.h"
#include "components/OptionListComponent.h"

GuiHashStart::GuiHashStart(Window& window, SystemManager& systemManager)
  : Gui(window),
    mSystemManager(systemManager),
    mBusyAnim(window),
    mMenu(window, _("HASH NOW")),
    mState(State::Wait),
    mThreadPool(this, "Crc32", false),
    mTotalGames(0),
    mRemaininglGames(0)
{
  addChild(&mMenu);

  mBusyAnim.setSize(Renderer::getDisplayWidthAsFloat(), Renderer::getDisplayHeightAsFloat());

  mFilter = std::make_shared<OptionListComponent<std::string> >(mWindow, _("FILTER"), false);
  mFilter->add(_("Only missing hashs"), "missing", true);
  mFilter->add(_("All Games"), "all", false);
  mMenu.addWithLabel(mFilter, _("FILTER"));

  // add systems (all with a platformid specified selected)
  mSystems = std::make_shared<OptionListComponent<SystemData*> >(mWindow, _("HASH THESE SYSTEMS"), true);
  for (auto it : mSystemManager.GetVisibleSystemList())
  {
    if (RecalboxConf::Instance().isInList("global.netplay.systems", it->getName()))
      mSystems->add(it->getFullName(), it, true);
  }
  mMenu.addWithLabel(mSystems, _("SYSTEMS"));

  mMenu.addButton(_("START"), "start", [this]
  {
    mWindow.pushGui(new GuiMsgBox(mWindow, _("THIS COULD TAKE A WHILE, CONFIRM?"), _("YES"), [this]
    {
      mState = State::Hashing;
      Start();
    }, _("NO"), [this] { mState = State::Exit; }));
  });
  mMenu.addButton(_("BACK"), "back", [&]
  { mState = State::Cancelled; });

  mMenu.setPosition((Renderer::getDisplayWidthAsFloat() - mMenu.getSize().x()) / 2,
                    (Renderer::getDisplayHeightAsFloat() - mMenu.getSize().y()) / 2);
}

bool GuiHashStart::ProcessInput(const InputCompactEvent& event)
{
  // Cancel or exit immediatelly on pressing A
  if (event.APressed())
    mState = State::Cancelled;

  return Component::ProcessInput(event);
}

void GuiHashStart::Update(int deltaTime)
{
  Component::Update(deltaTime);

  // Read summary text
  mMutex.Lock();
  std::string text = mSummaryText;
  mMutex.UnLock();

  mBusyAnim.setText(text);
  mBusyAnim.Update(deltaTime);

  switch(mState)
  {
    case State::Wait:
    case State::Hashing:break;
    case State::Cancelled:
    {
      if (mRemaininglGames == 0)
        mState = State::Exit;
      break;
    }
    case State::Exit:
    {
      Close();
      break;
    }
  }

  if (mState == State::Exit)
    Close();
}

void GuiHashStart::Render(const Transform4x4f& parentTrans)
{
  Transform4x4f trans = parentTrans * getTransform();

  if (mState != State::Hashing)
    renderChildren(trans);

  Renderer::setMatrix(trans);
  Renderer::drawRect(0.f, 0.f, mSize.x(), mSize.y(), 0x00000011);

  if (mState == State::Hashing)
    mBusyAnim.Render(trans);
}

bool GuiHashStart::getHelpPrompts(Help& help)
{
  mMenu.getHelpPrompts(help);
  help.Set(HelpType::A, _("BACK"));
  return true;
}

void GuiHashStart::Start()
{
  bool forceAll = mFilter->getSelected() == "all";

  // Run through systems...
  for (auto system : mSystems->getSelectedObjects())
  {
    // Run through games
    FileData::List games = system->getRootFolder().getAllItems(true, true);
    for (FileData* game : games)
      if (game->Metadata().RomCrc32() == 0 || forceAll)
        mThreadPool.PushFeed(game);
    mTotalGames = mThreadPool.PendingJobs();
    mRemaininglGames = mTotalGames;
  }

  // Run!
  mThreadPool.Run(-2, true);
}

FileData* GuiHashStart::ThreadPoolRunJob(FileData*& feed)
{
  if (mState == State::Hashing)
  {
    // Get hash
    unsigned int result = 0;
    if (Crc32File(feed->getPath()).Crc32(result))
      feed->Metadata().SetRomCrc32((int) result);

    std::string busyText(feed->getSystem()->getFullName());
    busyText.append(1, ' ')
            .append(Strings::ToString(mTotalGames - mRemaininglGames))
            .append(1, '/')
            .append(Strings::ToString(mTotalGames));

    // Write summary text
    mMutex.Lock();
    mSummaryText = busyText;
    mMutex.UnLock();
  }

  // Processed (or cancelled)
  --mRemaininglGames;

  return feed;
}

