//
// Created by xizor on 20/05/18.
//
#pragma once

#include <components/ComponentList.h>
#include "guis/Gui.h"
#include "components/NinePatchComponent.h"
#include "components/ComponentGrid.h"
#include "components/ProgressBarComponent.h"
#include <recalbox/RecalboxSystem.h>
#include <MenuThemeData.h>
#include <utils/os/system/Thread.h>
#include <utils/sdl2/ISynchronousEvent.h>
#include <utils/sdl2/SyncronousEvent.h>
#include <utils/Http.h>

class GuiUpdateRecalbox : public Gui, private Thread, private ISynchronousEvent, private Http::IDownload
{
  public:
    GuiUpdateRecalbox(Window& window, const std::string& url, const std::string& newVersion);

    ~GuiUpdateRecalbox() override;

    bool ProcessInput(const InputCompactEvent& event) override;

    bool getHelpPrompts(Help& help) override;

  private:

    /*!
     * @brief Receive synchronous SDL2 event
     * @param event SDL event with .user populated by the sender
     */
    void ReceiveSyncCallback(const SDL_Event& event) override;

    /*
     * Thread Implementation
     */

    /*!
     * @brief Main thread routine
     */
    void Run() override;

    /*
     * Http::IDownload implementation
     */

    /*!
     * @brief Notify of download progress
     * @param http HTTP request
     * @param currentSize downloaded bytes
     * @param expectedSize total expected bytes
     */
    void DownloadProgress(const Http& http, long long currentSize, long long expectedSize) override;

    //! Http request object
    Http mRequest;

    //! Url to download
    std::string mUrl;

    //! Time reference
    DateTime mTimeReference;

    //! File length
    long long mTotalSize;
    //! Downloaded length
    long long mCurrentSize;

    SyncronousEvent mSender;
    NinePatchComponent mBackground;
    ComponentGrid mGrid;

    std::shared_ptr<TextComponent> mTitle;
    std::shared_ptr<TextComponent> mText;
    std::shared_ptr<ProgressBarComponent> mBar;
    std::shared_ptr<TextComponent> mEta;
};
