//
// Created by bkg2k on 21/11/2019.
//
#include <WindowManager.h>
#include <components/TextComponent.h>

#pragma once

class CrtView : public Gui, private ISynchronousEvent
{
  public:
    /*!
     * @brief Constructor
     * @param window Main Window instance
     */
    explicit CrtView(WindowManager& window);

    /*
     * IComponent implementation
     */

    /*!
     * @brief Called once per frame, after Update.
     * @param parentTrans Transformation
     */
    void Render(const Transform4x4f& parentTrans) override;

  private:
    //! Pattern image
    ImageComponent mPattern;

    //! Synchronous event
    SyncronousEvent mEvent;

    //! Update viewport
    void UpdateViewport();

    //! Update position
    void UpdatePosition();

    /*
     *  Component overriding
     */

    bool getHelpPrompts(Help& help) override;

    bool ProcessInput(const InputCompactEvent& event) override;


    /*
     * Synchronous event
     */

    /*!
     * @brief Receive SDL event from the main thread
     * @param event SDL event
     */
    void ReceiveSyncCallback(const SDL_Event& event) override;
};


