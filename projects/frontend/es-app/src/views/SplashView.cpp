//
// Created by bkg2k on 21/11/2019.
//

#include <Renderer.h>
#include <utils/locale/LocaleHelper.h>
#include "SplashView.h"

SplashView::SplashView(Window& window)
  : Gui(window),
    mLogo(window, true, true),
    mLoading(window, _("LOADING..."), Font::get(FONT_SIZE_MEDIUM), 0),
    mSystemCount(0),
    mSystemLoaded(0)
{
  mPosition.Set(0,0,0);
  mSize.Set(Renderer::getDisplayWidthAsFloat(), Renderer::getDisplayHeightAsFloat());

  mLogo.setResize(Renderer::getDisplayWidthAsFloat() * 0.3f, 0.0f);
  mLogo.setImage(Path(":/splash.svg"));
  mLogo.setPosition((Renderer::getDisplayWidthAsFloat() - mLogo.getSize().x()) / 2,
                     (Renderer::getDisplayHeightAsFloat() - mLogo.getSize().y()) / 2 * 0.6f);

  mLoading.setHorizontalAlignment(TextAlignment::Center);
  mLoading.setSize(Renderer::getDisplayWidthAsFloat(), Renderer::getDisplayHeightAsFloat() / 10.0f);
  mLoading.setPosition(0.0f, Renderer::getDisplayHeightAsFloat() * 0.8f, 0.0f);
  mLoading.setColor(0x656565FF);
}

void SplashView::Render(const Transform4x4f& parentTrans)
{
  Transform4x4f trans = (parentTrans * getTransform()).round();
  Renderer::setMatrix(trans);

  Renderer::drawRect(0, 0, Renderer::getDisplayWidthAsInt(), Renderer::getDisplayHeightAsInt(), 0xFFFFFFFF);

  int w = (int)(Renderer::getDisplayWidthAsFloat() / 10.0f);
  int h = (int)(Renderer::getDisplayHeightAsFloat() / 80.0f);
  int x = (int)((Renderer::getDisplayWidthAsFloat() - (float)w) / 2.0f);
  int y = (int)(Renderer::getDisplayHeightAsFloat() * 0.9f);
  Renderer::drawRect(x, y, w, h, 0xC0C0C0FF);
  if (mSystemCount != 0)
  {
    w = (w * mSystemLoaded) / mSystemCount;
    Renderer::drawRect(x, y, w, h, 0x606060FF);
  }

  mLogo.Render(trans);
  mLoading.Render(trans);

  Component::Render(trans);
}

void SplashView::SetMaximum(int maximum)
{
  mSystemCount = maximum;
  mWindow.RenderAll();
}

void SplashView::SetProgress(int value)
{
  mSystemLoaded = value;
  mWindow.RenderAll();
}

void SplashView::Increment()
{
  if (++mSystemLoaded >= mSystemCount) mSystemLoaded = mSystemCount - 1;
  mWindow.RenderAll();
}

void SplashView::Quit()
{
  mSystemLoaded = mSystemCount = 0;
  mLoading.setText(_("PLEASE WAIT..."));
}

