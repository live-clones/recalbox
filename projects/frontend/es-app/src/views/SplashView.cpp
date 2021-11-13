//
// Created by bkg2k on 21/11/2019.
//

#include <Renderer.h>
#include <utils/locale/LocaleHelper.h>
#include "SplashView.h"

#define FONT_SIZE_LOADING ((unsigned int)(0.065f * Math::min(Renderer::Instance().DisplayHeightAsFloat(), Renderer::Instance().DisplayWidthAsFloat())))

SplashView::SplashView(WindowManager& window)
  : Gui(window),
    mLogo(window, true, true),
    mLoading(window, _("LOADING..."), Font::get(Renderer::Instance().IsSmallResolution() ? (int)(FONT_SIZE_LOADING) : (int)(FONT_SIZE_MEDIUM)), 0),
    mSystemCount(0),
    mSystemLoaded(0)
{
  mPosition.Set(0,0,0);
  mSize.Set(Renderer::Instance().DisplayWidthAsFloat(), Renderer::Instance().DisplayHeightAsFloat());

  mLogo.setResize(Renderer::Instance().DisplayWidthAsFloat() * 0.3f, 0.0f);
  mLogo.setImage(Path(":/splash.svg"));
  mLogo.setPosition((Renderer::Instance().DisplayWidthAsFloat() - mLogo.getSize().x()) / 2,
                     (Renderer::Instance().DisplayHeightAsFloat() - mLogo.getSize().y()) / 2 * 0.6f);

  mLoading.setHorizontalAlignment(TextAlignment::Center);
  mLoading.setSize(Renderer::Instance().DisplayWidthAsFloat(), Renderer::Instance().DisplayHeightAsFloat() / 10.0f);
  mLoading.setPosition(0.0f, Renderer::Instance().DisplayHeightAsFloat() * 0.8f, 0.0f);
  mLoading.setColor(0x656565FF);
}

void SplashView::Render(const Transform4x4f& parentTrans)
{
  Transform4x4f trans = (parentTrans * getTransform()).round();
  Renderer::SetMatrix(trans);

  Renderer::DrawRectangle(0, 0, Renderer::Instance().DisplayWidthAsInt(), Renderer::Instance().DisplayHeightAsInt(), 0xFFFFFFFF);

  int w = (int)(Renderer::Instance().DisplayWidthAsFloat() / (Renderer::Instance().IsSmallResolution() ? 5.0f : 10.0f));
  int h = (int)(Renderer::Instance().DisplayHeightAsFloat() / (Renderer::Instance().IsSmallResolution() ? 70.0f : 80.0f));
  int x = (int)((Renderer::Instance().DisplayWidthAsFloat() - (float)w) / 2.0f);
  int y = (int)(Renderer::Instance().DisplayHeightAsFloat() * 0.9f);
  Renderer::DrawRectangle(x, y, w, h, 0xC0C0C0FF);
  if (mSystemCount != 0)
  {
    w = (w * mSystemLoaded) / mSystemCount;
    Renderer::DrawRectangle(x, y, w, h, 0x606060FF);
  }

  mLogo.Render(trans);
  mLoading.Render(trans);

  Component::Render(trans);
}

void SplashView::SetMaximum(int maximum)
{
  mSystemCount = maximum;
  mWindow.DoWake(); // Kill screensaver
  mWindow.RenderAll();
}

void SplashView::SetProgress(int value)
{
  mSystemLoaded = value;
  mWindow.DoWake(); // Kill screensaver
  mWindow.RenderAll();
}

void SplashView::Increment()
{
  if (++mSystemLoaded >= mSystemCount) mSystemLoaded = mSystemCount - 1;
  mWindow.DoWake(); // Kill screensaver
  mWindow.RenderAll();
}

void SplashView::Quit()
{
  mSystemLoaded = mSystemCount = 0;
  mLoading.setText(_("PLEASE WAIT..."));
}

