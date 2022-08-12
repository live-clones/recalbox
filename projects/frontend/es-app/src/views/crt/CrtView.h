//
// Created by bkg2k on 21/11/2019.
//
#include <WindowManager.h>
#include <components/TextComponent.h>
#include <utils/sync/SyncMessageSender.h>

#pragma once

class CrtView : public Gui
              , private ISyncMessageReceiver<void>
{
  public:
    /*!
     * @brief Constructor
     * @param window Main Window instance
     */
    explicit CrtView(WindowManager& window);

    //! Destructor
    ~CrtView() override;

    /*
     * IComponent implementation
     */

    /*!
     * @brief Called once per frame, after Update.
     * @param parentTrans Transformation
     */
    void Render(const Transform4x4f& parentTrans) override;

    //! Process input events
    bool ProcessInput(const InputCompactEvent& event) override;

    //! Help bar
    bool getHelpPrompts(Help& help) override;

  private:
    //! Timing file path
    static constexpr const char* sTimingFile = "/boot/crt/timings.txt";

    //! Pattern image
    ImageComponent mPattern;

    //! Inner grid
    ComponentGrid mGrid;
    //! Horizontal offset text
    std::shared_ptr<TextComponent> mHorizontalOffsetText;
    //! Vertical offset text
    std::shared_ptr<TextComponent> mVerticalOffsetText;
    //! Viewport text
    std::shared_ptr<TextComponent> mViewportText;

    //! Synchronous event
    SyncMessageSender<void> mEvent;

    //! Original config
    int mOriginalVOffset;
    int mOriginalHOffset;
    int mOriginalViewportWidth;

    //! Viewport ratio
    int mStep;

    //! Update viewport
    void UpdateViewport();

    //! Update position
    void UpdatePosition();

    /*
     * Synchronous event
     */

    /*!
     * @brief Receive message from the CRT thread
     */
    void ReceiveSyncMessage() override;
};


