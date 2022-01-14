//
// Created by bkg2k on 21/11/2019.
//

#include <Renderer.h>
#include <utils/locale/LocaleHelper.h>
#include "CrtView.h"
#include "views/ViewController.h"

#define FONT_SIZE_LOADING ((unsigned int)(0.065f * Math::min(Renderer::Instance().DisplayHeightAsFloat(), Renderer::Instance().DisplayWidthAsFloat())))

CrtView::CrtView(WindowManager& window)
  : Gui(window)
  , mPattern(window, true, true)
  , mEvent(this)
{
  bool lowRes = Renderer::Instance().DisplayHeightAsInt() <= 288;
  mPattern.setImage(Path(lowRes ? ":/crt/pattern240.png" : ":/crt/pattern480.png"), false);
  mPattern.setSize(Renderer::Instance().DisplayWidthAsFloat(), Renderer::Instance().DisplayHeightAsFloat());
  mPattern.setOrigin(.5f, .5f);
  mPattern.setPosition(Renderer::Instance().DisplayWidthAsFloat() / 2.f, Renderer::Instance().DisplayHeightAsFloat() / 2.f, .0f);
  UpdateViewport();
  UpdatePosition();
}

void CrtView::Render(const Transform4x4f& parentTrans)
{
  Transform4x4f trans = (parentTrans * getTransform()).round();
  Renderer::SetMatrix(trans);

  Renderer::DrawRectangle(0, 0, Renderer::Instance().DisplayWidthAsInt(), Renderer::Instance().DisplayHeightAsInt(), 0xFFFFFFFF);

  int w = (int)(Renderer::Instance().DisplayWidthAsFloat() / (Renderer::Instance().IsSmallResolution() ? 5.0f : 10.0f));
  int h = (int)(Renderer::Instance().DisplayHeightAsFloat() / (Renderer::Instance().IsSmallResolution() ? 70.0f : 80.0f));
  int x = (int)((Renderer::Instance().DisplayWidthAsFloat() - (float)w) / 2.0f);
  int y = (int)(Renderer::Instance().DisplayHeightAsFloat() * 0.9f);
  Renderer::DrawRectangle(x, y, w, h, 0xC0C0C0FF);

  mPattern.Render(trans);

  Component::Render(trans);
}

bool CrtView::getHelpPrompts(Help& help)
{
  if(RecalboxConf::Instance().GetShowGameClipHelpItems())
    help.Set(Help::Cancel(), _("QUIT"))
        .Set(HelpType::AllDirections, _("MOVE SCREEN"))
        .Set(HelpType::X, _("WIDER"))
        .Set(HelpType::Y, _("NARROWER"))
        .Set(Help::Valid(), _("VALIDATE CHANGES"));

  return true;
}

bool CrtView::ProcessInput(const InputCompactEvent& event)
{
  if (event.CancelPressed()) mEvent.Call(); // Synchroneous quit (delete this class)
  else if (event.ValidPressed()) // Validater: reinit SDL
  {
    UpdateViewport();
    UpdatePosition();
    Renderer::Instance().Finalize();
    Renderer::Instance().ReInitialize();
  }
  else if (event.XPressed()) // Wider
  {
    CrtConf::Instance().SetSystemCRTViewportWidth(CrtConf::Instance().GetSystemCRTViewportWidth() + 1);
    UpdateViewport();
  }
  else if (event.XPressed()) // Narrower
  {
    CrtConf::Instance().SetSystemCRTViewportWidth(CrtConf::Instance().GetSystemCRTViewportWidth() - 1);
    UpdateViewport();
  }
  else if (event.AnyUpPressed())
  {
    CrtConf::Instance().SetSystemCRTVerticalOffset(CrtConf::Instance().GetSystemCRTVerticalOffset() - 1);
  }
  else if (event.AnyDownPressed())
  {
    CrtConf::Instance().SetSystemCRTVerticalOffset(CrtConf::Instance().GetSystemCRTVerticalOffset() + 1);
  }
  else if (event.AnyLeftPressed())
  {
    CrtConf::Instance().SetSystemCRTHorizontalOffset(CrtConf::Instance().GetSystemCRTHorizontalOffset() - 1);
  }
  else if (event.AnyRightPressed())
  {
    CrtConf::Instance().SetSystemCRTHorizontalOffset(CrtConf::Instance().GetSystemCRTHorizontalOffset() + 1);
  }

  return true;
}

void CrtView::ReceiveSyncCallback(const SDL_Event& event)
{
  (void)event;
  ViewController::Instance().quitCrtView();
}

void CrtView::UpdateViewport()
{
  mPattern.setSize((float)CrtConf::Instance().GetSystemCRTViewportWidth(), mPattern.getSize().x());
}

void CrtView::UpdatePosition()
{

}
