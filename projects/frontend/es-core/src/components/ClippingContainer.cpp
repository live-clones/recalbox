//
// Created by gugue_U on 17/10/2020
//

#include <Window.h>
#include <Renderer.h>
#include "ClippingContainer.h"
#include "themes/ThemeData.h"

ClippingContainer::ClippingContainer(Window &window)
        : Component(window),
          mClippingState(false){
    mTimer.Initialize(0);
}

void ClippingContainer::init(){
    mClippingState = false;
    mTimer.Initialize(0);
}

void ClippingContainer::Render(const Transform4x4f &parentTrans) {
    if (activate && !clippingDisplay()) {
        return;
    }

    Transform4x4f trans = parentTrans * getTransform();
    Renderer::setMatrix(trans);
    Component::renderChildren(trans);
}

bool ClippingContainer::clippingDisplay() {
    if (mClippingState && mTimer.GetMilliSeconds() > mTimeUp) {
        mTimer.Initialize(0);
        mClippingState = false;

    } else if (!mClippingState && mTimer.GetMilliSeconds() > mTimeDown) {
        mTimer.Initialize(0);
        mClippingState = true;
    }

    return mClippingState;
}