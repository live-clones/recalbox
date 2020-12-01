//
// Created by gugue_U on 17/10/2020
//

#ifndef EMULATIONSTATION_ALL_CLIPPINGCONTAINER_H
#define EMULATIONSTATION_ALL_CLIPPINGCONTAINER_H


#include <utils/datetime/HighResolutionTimer.h>

class ClippingContainer : public Component {

public:
    ClippingContainer(Window &window);

    void Render(const Transform4x4f &parentTrans) override;

    void init();

private:
    bool activate = true;
    int mTimeUp = 800;
    int mTimeDown = 500;
    bool mClippingState;
    HighResolutionTimer mTimer;
    bool clippingDisplay();
};



#endif //EMULATIONSTATION_ALL_CLIPPINGCONTAINER_H
