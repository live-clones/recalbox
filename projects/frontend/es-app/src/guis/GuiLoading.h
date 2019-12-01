//
// Created by matthieu on 03/08/15.
//
#pragma once

#include "guis/Gui.h"
#include "components/MenuComponent.h"
#include "components/BusyComponent.h"
#include "components/NinePatchComponent.h"
#include <utils/os/system/Thread.h>
#include <utils/sdl2/ISyncronousEvent.h>
#include <utils/sdl2/SyncronousEvent.h>

template<class T> class GuiLoading: public Gui, private Thread, private ISyncronousEvent
{
  private:
    BusyComponent mBusyAnim;
    NinePatchComponent mBackground;
    SyncronousEvent mSender;

    const std::function<T()>     mAsyncMethodToExecute;
    const std::function<void(T)> mSyncMethodToPushResult;
    T mResult;

    /*
     * Thread implementation
     */

    /*!
     * @brief Main thread method
     */
    void Run() override
    {
      mResult = mAsyncMethodToExecute();
      mSender.Call();
    }

    /*
     * ISyncronousEvent implementation
     */

    /*!
     * @brief Sync event callback
     * @param event SDL event
     */
    void ReceiveSyncCallback(const SDL_Event&) override
    {
      if (mSyncMethodToPushResult != nullptr)
        mSyncMethodToPushResult(mResult);
      Close();
    }

public:
    /*!
     * @brief Constructor
     * @param window main ui window
     * @param asyncExecution Method to execute asynchronously
     * @param pushResult Method to call to push the result
     */
    GuiLoading(Window& window, const std::function<T()> asyncExecution, const std::function<void(T)> pushResult)
      : Gui(window),
        mBusyAnim(window),
        mBackground(window, Path(":/frame.png")),
        mSender(this),
        mAsyncMethodToExecute(asyncExecution),
        mSyncMethodToPushResult(pushResult),
        mResult()
    {
      setSize(Renderer::getDisplayWidthAsFloat(), Renderer::getDisplayHeightAsFloat());
      mBusyAnim.setSize(mSize);
      auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();

      mBackground.setImagePath(menuTheme->menuBackground.path);
      mBackground.setCenterColor(menuTheme->menuBackground.color);
      mBackground.setEdgeColor(menuTheme->menuBackground.color);
      addChild(&mBackground);

      Thread::Start("GuiLoading");
    }

    /*!
     * @brief Desctructor
     */
    ~GuiLoading() override
    {
      Thread::Stop();
    }

    void Update(int deltaTime) override
    {
      Component::Update(deltaTime);
      mBusyAnim.Update(deltaTime);
    }

    void Render(const Transform4x4f& parentTrans) override
    {
      Transform4x4f trans = parentTrans * getTransform();
      renderChildren(trans);

      Renderer::setMatrix(trans);
      Renderer::drawRect(0.f, 0.f, mSize.x(), mSize.y(), 0x00000011);

      if (IsRunning())
        mBusyAnim.Render(trans);
    }

    bool getHelpPrompts(Help& help) override { (void)help; return true; }
};

