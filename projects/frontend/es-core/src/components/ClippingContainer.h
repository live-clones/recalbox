//
// Created by gugue_U on 17/10/2020
//

#ifndef EMULATIONSTATION_ALL_CLIPPINGCONTAINER_H
#define EMULATIONSTATION_ALL_CLIPPINGCONTAINER_H


#include <utils/datetime/HighResolutionTimer.h>

class ClippingContainer : public Component {

public:
    ClippingContainer(WindowManager& window);
    ClippingContainer(WindowManager& window, int timeUp, int timedown);

    void Render(const Transform4x4f &parentTrans) override;

private:
    int mTimeUp = 500;
    int mTimeDown = 1000;
    bool mClippingDisplayed;
    HighResolutionTimer mTimer;
    void UpdateClippingState();
};



#endif //EMULATIONSTATION_ALL_CLIPPINGCONTAINER_H
