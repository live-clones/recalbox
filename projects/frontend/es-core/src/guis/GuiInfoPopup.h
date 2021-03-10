//
// Created by xizor on 30/03/18.
//
#pragma once

#include "guis/Gui.h"
#include "components/NinePatchComponent.h"
#include "components/ComponentGrid.h"
#include "WindowManager.h"
#include "utils/Log.h"
#include "utils/math/Vectors.h"

class TextComponent;

class GuiInfoPopup : public Gui
{
  public:
    enum class Corner
    {
      TopLeft,
      TopRight,
      BottomRight,
      BottomLeft,
    };

    enum class PopupType
    {
      None,     //!< No icon
      Music,    //!< Music icon
      Help,     //!< Help icon
      Netplay,  //!< Netplay logo
      Recalbox, //!< Recalbox logo
      Pads,     //!< Joystick logo
      Reboot,   //!< Pending reboot
    };

    GuiInfoPopup(WindowManager& window, const std::string& message, int duration, PopupType icon);
    ~GuiInfoPopup() override = default;
    void Update(int delta) override;
    void Render(const Transform4x4f& parentTrans) override;
    inline void stop() { mRunning = false; };

    /*!
     * @brief Set initial target offset, relative to the selected corner
     * @param offset target offset
     */
    void SetOffset(int offset) { mTargetOffset = offset; }

    /*!
     * @brief Slide target offset from the given amount of pixels
     * @param offset
     */
    void SlideOffset(int size);

    //! Popup must be closed?
    bool TimeOut() const { return !mRunning; }

    PopupType Type() const { return mType; }

  private:
    ComponentGrid mGrid;
    NinePatchComponent mFrame;
    std::shared_ptr<TextComponent> mMsgText;
    std::shared_ptr<TextComponent> mMsgIcon;
    unsigned int mFrameColor;
    PopupType mType;
    Corner mCorner;
    int mTargetOffset;
    int mDuration;
    int maxAlpha;
    int mStartTime;
    bool mRunning;
};