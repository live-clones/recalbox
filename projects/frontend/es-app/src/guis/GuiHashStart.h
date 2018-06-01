//
// Created by xizor on 01/06/18.
//

#ifndef EMULATIONSTATION_ALL_GUIHASHSTART_H
#define EMULATIONSTATION_ALL_GUIHASHSTART_H

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
    GuiHashStart(Window* window);

    bool input(InputConfig* config, Input input) override;

    virtual std::vector<HelpPrompt> getHelpPrompts() override;

    void update(int deltaTime) override;

    void render(const Eigen::Affine3f &parentTrans) override;

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

#endif //EMULATIONSTATION_ALL_GUIHASHSTART_H
