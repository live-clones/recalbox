#include "guis/GuiUpdate.h"
#include "guis/GuiMsgBox.h"

#include "utils/Log.h"
#include "recalbox/RecalboxSystem.h"
#include "LocaleHelper.h"
#include <recalbox/RecalboxUpgrade.h>
#include <MainRunner.h>

GuiUpdate::GuiUpdate(Window& window)
  : Gui(window),
    mBusyAnim(window),
    mBackground(window, Path(":/frame.png")),
    mState(State::CheckForUpdate)
{
  mIsProcessing = true;

  Thread::Start("CheckUpdate");

  setSize(Renderer::getDisplayWidthAsFloat(), Renderer::getDisplayHeightAsFloat());
  mBusyAnim.setSize(mSize);
  auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();

  mBackground.setImagePath(menuTheme->menuBackground.path);
  mBackground.setCenterColor(menuTheme->menuBackground.color);
  mBackground.setEdgeColor(menuTheme->menuBackground.color);
  addChild(&mBackground);
}

GuiUpdate::~GuiUpdate()
{
  Thread::Stop();
}

void GuiUpdate::Render(const Transform4x4f& parentTrans)
{
  Transform4x4f trans = parentTrans * getTransform();

  renderChildren(trans);

  Renderer::setMatrix(trans);
  Renderer::drawRect(0.f, 0.f, mSize.x(), mSize.y(), 0x00000011);

  if (mState == State::CheckForUpdate || mState == State::DoUpdate)
    mBusyAnim.Render(trans);
}

void GuiUpdate::Update(int deltaTime)
{
  Component::Update(deltaTime);
  mBusyAnim.Update(deltaTime);

  switch(mState)
  {
    case State::Wait:
    case State::CheckForUpdate:
    case State::DoUpdate: break;
    case State::AskForUpdate:
    {
      mWindow.pushGui(new GuiMsgBox(mWindow, _("REALLY UPDATE?"),
                       _("YES"), [this]
                       {
                         mState = State::DoUpdate;
                         Thread::Start("DownloadUpdate");
                       },
                       _("NO"), [this]
                       {
                         mState = State::Exit;
                       })

                     );
      mState = State::Wait;
      break;
    }
    case State::NeedNetwork:
    {
      mWindow.pushGui(new GuiMsgBox(mWindow, _("NETWORK CONNECTION NEEDED"), _("OK"), [this]
      {
        mState = State::Exit;
      }));
      mState = State::Wait;
      break;
    }
    case State::Reboot:
    {
      MainRunner::RequestQuit(MainRunner::ExitState::NormalReboot);
      mState = State::Wait;
      break;
    }
    case State::Error:
    {
      mWindow.pushGui(new GuiMsgBox(mWindow, mResult, _("OK"), [this]
      {
        mState = State::Exit;
      }));
      mState = State::Wait;
      break;
    }
    case State::NoUpdate:
    {
      mWindow.pushGui(new GuiMsgBox(mWindow, _("NO UPDATE AVAILABLE"), _("OK"), [this]
      {
        mState = State::Exit;
      }));
      mState = State::Wait;
      break;
    }
    case State::Exit:
    {
      Close();
      break;
    }
  }
}

void GuiUpdate::Run()
{
  switch(mState)
  {
    case State::CheckForUpdate:
    {
      if (RecalboxSystem::ping())
      {
        if (RecalboxUpgrade::canUpdate())
        {
          LOG(LogInfo) << "update available" << "\n";
          mState = State::AskForUpdate;
        }
        else
        {
          LOG(LogInfo) << "no update available" << "\n";
          mState = State::NoUpdate;
        }
      }
      else
      {
        LOG(LogInfo) << "ping nok" << "\n";
        mState = State::NeedNetwork;
      }
      break;
    }
    case DoUpdate:
    {
      std::pair<std::string, int> updateStatus = RecalboxUpgrade::updateSystem(&mBusyAnim);
      if (updateStatus.second == 0) mState = Reboot;
      else
      {
        mResult = _("AN ERROR OCCURED - DOWNLOADED") + std::string(": ") +
          Strings::Replace(updateStatus.first, "\e[1A", "");
        mState = State::Error;
      }
      break;
    }
    case Exit:
    case Wait:
    case AskForUpdate:
    case NeedNetwork:
    case Reboot:
    case Error:
    case NoUpdate:break;
  }
}