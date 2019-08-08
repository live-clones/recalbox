//
// Created by xizor on 01/06/18.
//
#pragma once

#include <boost/thread.hpp>
#include "GuiComponent.h"
#include "SystemData.h"
#include "components/MenuComponent.h"
#include "components/BusyComponent.h"

template<typename T>
class OptionListComponent;


class GuiHashStart : public GuiComponent
{
public:
    explicit GuiHashStart(Window* window);

    bool input(InputConfig* config, Input input) override;

    std::vector<HelpPrompt> getHelpPrompts() override;

    void update(int deltaTime) override;

    void render(const Transform4x4f &parentTrans) override;

private:
    void start();

    BusyComponent mBusyAnim;

    std::shared_ptr< OptionListComponent<SystemData*> > mSystems;

    std::shared_ptr< OptionListComponent<std::string> > mFilter;

    MenuComponent mMenu;

    bool mLoading;

	boost::thread *mHandle;

	int mState;
};
