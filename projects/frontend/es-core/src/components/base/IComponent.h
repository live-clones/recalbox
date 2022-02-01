//
// Created by bkg2k on 21/11/2019.
//
#pragma once

class InputCompactEvent;
class Transform4x4f;

class IComponent
{
  public:
    /*!
     * @brief Notification of an input event
     * @param event Compact event
     * @return Implementation must return true if it consumed the event.
     */
    virtual bool ProcessInput(const InputCompactEvent& event) = 0;

    /*!
     * @brief Called once per frame. Override to implement your own drawings.
     * Call your base::Update()
     * @param deltaTime Elapsed time from the previous frame, in millisecond
     */
    virtual void Update(int deltaTime) = 0;

    /*!
     * @brief Called once per frame, after Update.
     * @param parentTrans Transformation
     */
    virtual void Render(const Transform4x4f& parentTrans) = 0;
};

