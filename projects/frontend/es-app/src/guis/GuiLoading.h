//
// Created by matthieu on 03/08/15.
//
#pragma once

#include "GuiComponent.h"
#include "components/MenuComponent.h"
#include "components/BusyComponent.h"
#include "components/NinePatchComponent.h"


#include <boost/thread.hpp>

class GuiLoading  : public GuiComponent {
public:
    GuiLoading(Window *window, const std::function<void *()> &mFunc, const std::function<void(void *)> &mFunc2);
    GuiLoading(Window *window, const std::function<void *()> &mFunc);

    ~GuiLoading() override;

    void render(const Transform4x4f& parentTrans) override;

    bool input(InputConfig *config, Input input) override;

    std::vector<HelpPrompt> getHelpPrompts() override;

    void update(int deltaTime) override;

private:
    BusyComponent mBusyAnim;
    NinePatchComponent mBackground;
    boost::thread *mHandle;
    bool mRunning;
    const std::function<void*()>& mFunc;
    const std::function<void(void *)>& mFunc2;
    void threadLoading();
    void * result;
};
