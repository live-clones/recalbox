#include "guis/GuiUpdate.h"
#include "guis/GuiMsgBox.h"

#include "Log.h"
#include "recalbox/RecalboxSystem.h"
#include "Locale.h"
#include <boost/algorithm/string/replace.hpp>
#include <recalbox/RecalboxUpgrade.h>

GuiUpdate::GuiUpdate(Window *window) : GuiComponent(window), mBusyAnim(window), mBackground(window, ":/frame.png") {
    setSize((float) Renderer::getScreenWidth(), (float) Renderer::getScreenHeight());
    mLoading = true;
    mIsProcessing = true;
    mPingHandle = new boost::thread(boost::bind(&GuiUpdate::threadPing, this));
    mBusyAnim.setSize(mSize);
    auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();

    mBackground.setImagePath(menuTheme->menuBackground.path);
    mBackground.setCenterColor(menuTheme->menuBackground.color);
    mBackground.setEdgeColor(menuTheme->menuBackground.color);
    addChild(&mBackground);

}

GuiUpdate::~GuiUpdate() {
    mPingHandle->join();
}

bool GuiUpdate::input(InputConfig *config, Input input)
{
    (void)config;
    (void)input;
    return false;
}

std::vector<HelpPrompt> GuiUpdate::getHelpPrompts() {
    return std::vector<HelpPrompt>();
}

void GuiUpdate::render(const Transform4x4f &parentTrans)
{
  Transform4x4f trans = parentTrans * getTransform();

    renderChildren(trans);

    Renderer::setMatrix(trans);
    Renderer::drawRect(0.f, 0.f, mSize.x(), mSize.y(), 0x00000011);

    if (mLoading)
        mBusyAnim.render(trans);

}

void GuiUpdate::update(int deltaTime) {
    GuiComponent::update(deltaTime);
    mBusyAnim.update(deltaTime);

    Window *window = mWindow;
    if (mState == 1) {
        window->pushGui(
                new GuiMsgBox(window, _("REALLY UPDATE?"), _("YES"),
                              [this] {
                                  mState = 2;
                                  mLoading = true;
                                  mHandle = new boost::thread(boost::bind(&GuiUpdate::threadUpdate, this));

                              }, _("NO"), [this] {
                            mState = -1;
                        })

        );
        mState = 0;
    }

    if (mState == 3) {
        window->pushGui(
                new GuiMsgBox(window, _("NETWORK CONNECTION NEEDED"), _("OK"),
                              [this] {
                                  mState = -1;
                              })
        );
        mState = 0;
    }
    if (mState == 4) {
        if (runRestartCommand() != 0) {
            LOG(LogWarning) << "Reboot terminated with non-zero result!";
        }
        mState = 0;
    }
    if (mState == 5) {
        window->pushGui(
                new GuiMsgBox(window, mResult.first, _("OK"),
                              [this] {
                                  mState = -1;
                              }
                )
        );
        mState = 0;
    }
    if (mState == 6) {
        window->pushGui(
                new GuiMsgBox(window, _("NO UPDATE AVAILABLE"), _("OK"),
                              [this] {
                                  mState = -1;
                              }));
        mState = 0;
    }
    if (mState == -1) {
        delete this;
    }
}

void GuiUpdate::threadUpdate() {
    std::pair<std::string, int> updateStatus = RecalboxUpgrade::getInstance()->updateSystem(&mBusyAnim);
    if (updateStatus.second == 0) {
        this->onUpdateOk();
    } else {
        this->onUpdateError(updateStatus);
    }
}

void GuiUpdate::threadPing() {
    if (RecalboxSystem::getInstance()->ping()) {
        if (RecalboxUpgrade::getInstance()->canUpdate()) {
            this->onUpdateAvailable();
        } else {
            this->onNoUpdateAvailable();

        }
    } else {
        this->onPingError();
    }
}

void GuiUpdate::onUpdateAvailable() {
    mLoading = false;
    LOG(LogInfo) << "update available" << "\n";
    mState = 1;
}

void GuiUpdate::onNoUpdateAvailable() {
    mLoading = false;
    LOG(LogInfo) << "no update available" << "\n";
    mState = 6;
}

void GuiUpdate::onPingError() {
    LOG(LogInfo) << "ping nok" << "\n";
    mLoading = false;
    mState = 3;
}

void GuiUpdate::onUpdateError(const std::pair<std::string, int>& result) {
    mLoading = false;
    mState = 5;
    mResult = result;
    std::string output = mResult.first;
    boost::replace_all(output, "\e[1A", "");
    mResult.first = _("AN ERROR OCCURED - DOWNLOADED") + std::string(": ") + std::string(output);
}

void GuiUpdate::onUpdateOk() {
    mLoading = false;
    mState = 4;
}
