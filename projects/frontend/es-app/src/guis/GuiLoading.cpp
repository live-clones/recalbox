//
// Created by matthieu on 03/08/15.
//

#include "GuiLoading.h"
#include "guis/GuiMsgBox.h"

#include "Window.h"
#include <boost/thread.hpp>
#include <string>
#include "Log.h"
#include "Settings.h"
#include "recalbox/RecalboxSystem.h"

GuiLoading::GuiLoading(Window *window, const std::function<void*()> &mFunc) : GuiComponent(window), mBusyAnim(window), mFunc(mFunc),mFunc2(NULL), mBackground(window, ":/frame.png") {
    setSize((float) Renderer::getScreenWidth(), (float) Renderer::getScreenHeight());
    mRunning = true;
    mHandle = new boost::thread(boost::bind(&GuiLoading::threadLoading, this));
    mBusyAnim.setSize(mSize);
    auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();

    mBackground.setImagePath(menuTheme->menuBackground.path);
    mBackground.setCenterColor(menuTheme->menuBackground.color);
    mBackground.setEdgeColor(menuTheme->menuBackground.color);
    addChild(&mBackground);

}

GuiLoading::GuiLoading(Window *window, const std::function<void*()> &mFunc, const std::function<void(void *)> &mFunc2) : GuiComponent(window), mBusyAnim(window), mFunc(mFunc),mFunc2(mFunc2), mBackground(window, ":/frame.png") {
    setSize((float) Renderer::getScreenWidth(), (float) Renderer::getScreenHeight());
    mRunning = true;
    mHandle = new boost::thread(boost::bind(&GuiLoading::threadLoading, this));
    mBusyAnim.setSize(mSize);
    auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();

    mBackground.setImagePath(menuTheme->menuBackground.path);
    mBackground.setCenterColor(menuTheme->menuBackground.color);
    mBackground.setEdgeColor(menuTheme->menuBackground.color);
}

GuiLoading::~GuiLoading() {
    mRunning = false;
    mHandle->join();
}

bool GuiLoading::input(InputConfig *config, Input input) {
    return false;
}

std::vector<HelpPrompt> GuiLoading::getHelpPrompts() {
    return std::vector<HelpPrompt>();
}

void GuiLoading::render(const Eigen::Affine3f &parentTrans) {
    Eigen::Affine3f trans = parentTrans * getTransform();

    renderChildren(trans);

    Renderer::setMatrix(trans);
    Renderer::drawRect(0.f, 0.f, mSize.x(), mSize.y(), 0x00000011);

    if (mRunning)
        mBusyAnim.render(trans);

}

void GuiLoading::update(int deltaTime) {
    GuiComponent::update(deltaTime);
    mBusyAnim.update(deltaTime);

    if (!mRunning) {
        if(mFunc2 != NULL)
            mFunc2(result);
        delete this;
    }
}

void GuiLoading::threadLoading() {
    result = mFunc();
    mRunning = false;
}