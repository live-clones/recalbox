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
#include <themes/MenuThemeData.h>
#include <utils/os/system/Thread.h>
#include <utils/sdl2/ISynchronousEvent.h>
#include <utils/sdl2/SyncronousEvent.h>
#include "utils/network/Http.h"

class GuiUpdateRecalbox : public Gui, private Thread, private ISynchronousEvent, private Http::IDownload
{
  public:
    GuiUpdateRecalbox(WindowManager& window, const std::string& imageUrl, const std::string& sha1Url, const std::string& newVersion);

    ~GuiUpdateRecalbox() override;

    bool ProcessInput(const InputCompactEvent& event) override;

    bool getHelpPrompts(Help& help) override;

  private:
    static constexpr const char* sDownloadFolder = "/boot/update";

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

    //! Image Url to download
    std::string mImageUrl;
    //! Sha1 Url to download
    std::string mSha1Url;
    //! New version
    std::string mNewVersion;
    // texts
    std::string mRebootIn;
    std::string mError;

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
