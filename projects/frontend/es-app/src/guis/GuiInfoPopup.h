//
// Created by xizor on 30/03/18.
//
#pragma once

#include "guis/Gui.h"
#include "components/NinePatchComponent.h"
#include "components/ComponentGrid.h"
#include "Window.h"
#include "utils/Log.h"
#include "utils/math/Vectors.h"

class GuiInfoPopup : public Gui, public Window::InfoPopup
{
  public:
    enum class Icon
    {
      None,     //!< No icon
      Music,    //!< Music icon
      Help,     //!< Help icon
      Netplay,  //!< Netplay logo
      Recalbox, //!< Recalbox logo
    };

    GuiInfoPopup(Window& window, const std::string& message, int duration, Icon icon);
    ~GuiInfoPopup() override = default;
    void Render(const Transform4x4f& parentTrans) override;
    inline void stop() { running = false; };

  private:
    ComponentGrid mGrid;
    NinePatchComponent mFrame;
    std::shared_ptr<TextComponent> mMsgText;
    std::shared_ptr<TextComponent> mMsgIcon;
    unsigned int mFrameColor;
    int mDuration;
    int maxAlpha;
    int mStartTime;
    bool running;

    bool updateState();
};