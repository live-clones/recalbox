//
// Created by bkg2k on 05/01/23.
//

#include "BluetoothOverlayGUI.h"
#include "Renderer.h"
#include "themes/MenuThemeData.h"

BluetoothOverlayGUI::BluetoothOverlayGUI(WindowManager &windowManager)
    : Gui(windowManager),
    mControllerIcon(windowManager),
    mBtTop(windowManager),
    mBtMiddle(windowManager),
    mBtBottom(windowManager) {
  float sw = Renderer::Instance().DisplayWidthAsFloat();
  float sh = Renderer::Instance().DisplayHeightAsFloat();

  // BT Icon
  auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
  mControllerIcon.setImage(Path(":/bluetooth/bt_controller.svg"));
  mControllerIcon.setPosition(10 + sw / 100, 10 + sh / 50);
  mControllerIcon.setMaxSize(sw / 32, sw / 32);
  mControllerIcon.setColor(menuTheme->menuBackground.color);

  mBtBottom.setImage(Path(":/bluetooth/bt_bottom.svg"));
  mBtBottom.setOrigin(0.5, 0);
  mBtBottom.setPosition(mControllerIcon.getCenter().x(), mControllerIcon.getPosition().y() - sh / 90);
  mBtBottom.setMaxSize(sw / 85, sw / 85);
  mBtBottom.setColor(menuTheme->menuBackground.color);

  mBtMiddle.setImage(Path(":/bluetooth/bt_middle.svg"));
  mBtMiddle.setOrigin(0.5, 0);
  mBtMiddle.setPosition(mControllerIcon.getCenter().x(), mControllerIcon.getPosition().y() - sh / 51);
  mBtMiddle.setMaxSize(sw / 53, sw / 53);
  mBtMiddle.setColor(menuTheme->menuBackground.color);

  mBtTop.setImage(Path(":/bluetooth/bt_top.svg"));
  mBtTop.setOrigin(0.5, 0);
  mBtTop.setPosition(mControllerIcon.getCenter().x(), mControllerIcon.getPosition().y() - sh / 36);
  mBtTop.setMaxSize(sw / 40, sw / 40);
  mBtTop.setColor(menuTheme->menuBackground.color);

  // Outlined Progress bar
  mProgressBar = Rectangle(mControllerIcon.getPosition().x() + mControllerIcon.getSize().x() + sh/200,
                           mControllerIcon.getPosition().y() + mControllerIcon.getSize().y() / 2 - (4 + sh / 100) / 2,
                           15 + sw / 12,
                           4 + sh / 100);
}

void BluetoothOverlayGUI::Update(int deltaTime) {
  Component::Update(deltaTime);

  if (mRemaining > 0)
    mRemaining -= deltaTime;
}

void BluetoothOverlayGUI::Render(const Transform4x4f &parentTrans) {
  mRemaining = 5;
  mTotal = 10;
  if (mRemaining > 0)
  {
    int percent = (SDL_GetTicks() / 7) % 100;
    mBtBottom.setOpacity(2.55 * (30 + ((90 + percent) % 100)));
    mBtMiddle.setOpacity(2.55 * (30 + ((60 + percent) % 100)));
    mBtTop.setOpacity(2.55 * (30 + ((30 + percent) % 100)));

    mBtBottom.Render(parentTrans);
    mBtMiddle.Render(parentTrans);
    mBtTop.Render(parentTrans);
    mControllerIcon.Render(parentTrans);

    Renderer::SetMatrix(parentTrans);
    Rectangle outer(mProgressBar);
    auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
    Renderer::DrawRectangle(outer, menuTheme->menuBackground.color);
    Renderer::DrawRectangle(outer.Contract(1, 1), menuTheme->menuText.color);
    outer.Contract(1, 1);
    Rectangle inner(outer.Left(), outer.Top(), outer.Width() * ((float) mRemaining / (float) mTotal), outer.Height());
    Renderer::DrawRectangle(inner, menuTheme->menuBackground.color);
  }
}
