#pragma once

#include <utils/os/system/Thread.h>
#include <utils/sdl2/ISyncronousEvent.h>
#include <utils/sdl2/SyncronousEvent.h>
#include "GuiComponent.h"
#include "components/MenuComponent.h"
#include "components/BusyComponent.h"
#include "components/NinePatchComponent.h"

class GuiUpdate : public GuiComponent, private Thread, private ISyncronousEvent
{
  public:
    explicit GuiUpdate(Window *window);

    ~GuiUpdate() override;

    void render(const Transform4x4f& parentTrans) override;

    bool ProcessInput(const InputCompactEvent&) override { return false; }

    std::vector<HelpPrompt> getHelpPrompts() override { return std::vector<HelpPrompt>(); }

    void update(int deltaTime) override;

  private:
    enum State
    {
      Exit = -1,
      Wait = 0,
      AskForUpdate = 1,
      DoUpdate = 2,
      NeedNetwork = 3,
      Reboot = 4,
      Error = 5,
      NoUpdate = 6,
      CheckForUpdate = 7,
    };

    BusyComponent mBusyAnim;
    NinePatchComponent mBackground;
    std::string mResult;
    volatile State mState;

    /*
     * Thread implementation
     */

    void Run() override;
};
