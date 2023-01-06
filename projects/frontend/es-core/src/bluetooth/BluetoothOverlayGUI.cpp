//
// Created by bkg2k on 05/01/23.
//

#include "BluetoothOverlayGUI.h"
#include "Renderer.h"
#include "themes/MenuThemeData.h"

BluetoothOverlayGUI::BluetoothOverlayGUI(WindowManager& windowManager)
  : Gui(windowManager)
  , mBluetoothIcon(windowManager)
{
  float sw = Renderer::Instance().DisplayWidthAsFloat();
  float sh = Renderer::Instance().DisplayHeightAsFloat();

  // BT Icon
  mBluetoothIcon.setImage(Path(":/bluetooth.svg"));
  mBluetoothIcon.setPosition(10 + sw / 100, 10 + sh / 100);
  mBluetoothIcon.setMaxSize(sh / 20, sh / 10);

  // Outlined Progress bar
  mProgressBar = Rectangle(10 + sh / 20 + sw / 50, 10 + sh / 100, 15 + sw / 12, 4 + sh / 100);
}

void BluetoothOverlayGUI::Update(int deltaTime)
{
  Component::Update(deltaTime);

  if (mRemaining > 0)
    mRemaining -= deltaTime;
}

void BluetoothOverlayGUI::Render(const Transform4x4f& parentTrans)
{
  if (mRemaining > 0)
  {
    if (((SDL_GetTicks() >> 7) & 3) != 0)
      mBluetoothIcon.Render(parentTrans);
    Renderer::SetMatrix(parentTrans);
    Rectangle outer(mProgressBar);
    auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
    Renderer::DrawRectangle(outer, menuTheme->menuBackground.color);
    Renderer::DrawRectangle(outer.Contract(1, 1), menuTheme->menuText.color);
    outer.Contract(1, 1);
    Rectangle inner(outer.Left(), outer.Top(), outer.Width() * ((float)mRemaining / (float)mTotal), outer.Height());
    Renderer::DrawRectangle(inner, menuTheme->menuBackground.color);
  }
}
