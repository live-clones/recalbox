//
// Created by gugue_U on 17/10/2020
//

#include <WindowManager.h>
#include <Renderer.h>
#include "ClippingContainer.h"
#include "themes/ThemeData.h"

ClippingContainer::ClippingContainer(WindowManager& window)
        : Component(window),
          mClippingDisplayed(false){
    mTimer.Initialize(0);
}

ClippingContainer::ClippingContainer(WindowManager& window, int timeUp, int timeDown)
  : Component(window),
    mTimeUp(timeUp),
    mTimeDown(timeDown),
    mClippingDisplayed(false){
  mTimer.Initialize(0);
}

void ClippingContainer::Render(const Transform4x4f &parentTrans) {
  UpdateClippingState();
  if (!mClippingDisplayed) {
    return;
  }

  Transform4x4f trans = parentTrans * getTransform();
  Renderer::SetMatrix(trans);
  Component::renderChildren(trans);
}

void ClippingContainer::UpdateClippingState() {
  if (mClippingDisplayed && mTimer.GetMilliSeconds() > mTimeUp) {
    mTimer.Initialize(0);
    mClippingDisplayed = false;

  }
  else if (!mClippingDisplayed && mTimer.GetMilliSeconds() > mTimeDown)
  {
    mTimer.Initialize(0);
    mClippingDisplayed = true;
  }
}