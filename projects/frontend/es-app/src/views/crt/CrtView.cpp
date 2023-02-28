//
// Created by bkg2k on 21/11/2019.
//

#include <Renderer.h>
#include <utils/locale/LocaleHelper.h>
#include "CrtView.h"
#include "views/ViewController.h"
#include "utils/Files.h"
#include "sdl2/Sdl2Init.h"
#include "themes/MenuThemeData.h"
#include "recalbox/RecalboxSystem.h"
#include "guis/GuiMsgBox.h"

#define FONT_SIZE_LOADING ((unsigned int)(0.065f * Math::min(Renderer::Instance().DisplayHeightAsFloat(), Renderer::Instance().DisplayWidthAsFloat())))

CrtView::CrtView(WindowManager& window, CalibrationType calibrationType)
  : Gui(window)
  , mPattern(window, true, true)
  , mGrid(window, Vector2i(1, 3))
  , mEvent(*this)
  , mSequence(nullptr)
  , mSequenceIndex(0)
  , mOriginalVOffset(0)
  , mOriginalHOffset(0)
  , mOriginalViewportWidth(0)
  , mOriginalWidth(Renderer::Instance().RealDisplayWidthAsInt())
  , mOriginalHeight(Renderer::Instance().DisplayHeightAsInt())
{
  switch(calibrationType){
    case kHz31:
      mSequence = sForced31khz; break;
    case kHz15_50Hz:
      mSequence = sPALOnly; break;
    case kHz15_60Hz:
      mSequence = sNTSCOnly; break;
    case kHz15_60plus50Hz:
      mSequence = sPALNTSC; break;
  }

  SetResolution(mSequence[0]);
  Initialize();
  UpdateViewport();
}

void CrtView::Initialize()
{
  mOriginalVOffset = CrtConf::Instance().GetCrtModeOffsetVerticalOffset(mSequence[0]);
  mOriginalHOffset = CrtConf::Instance().GetCrtModeOffsetHorizontalOffset(mSequence[0]);
  mOriginalViewportWidth = CrtConf::Instance().GetCrtViewportWidth(mSequence[0]);

  mPosition.Set(0,0,0);
  mSize.Set(Renderer::Instance().DisplayWidthAsFloat(), Renderer::Instance().DisplayHeightAsFloat());

  mGrid.ClearEntries();
  mGrid.setSize(Renderer::Instance().DisplayWidthAsFloat() * (300.0f / 768.f), Renderer::Instance().DisplayHeightAsFloat() * (156.f / 576.f));
  mGrid.setPosition(Renderer::Instance().DisplayWidthAsFloat() * (140.f / 768.f), Renderer::Instance().DisplayHeightAsFloat() * (100.0f / 576.f));
  addChild(&mGrid);

  mPattern.setResize(0.0f, Renderer::Instance().DisplayHeightAsFloat());
  mPattern.setOrigin(.5f, .5f);
  mPattern.setPosition(Renderer::Instance().DisplayWidthAsFloat() / 2.f, Renderer::Instance().DisplayHeightAsFloat() / 2.f, .0f);

  auto font = Font::get(7*Math::ceil(Renderer::Instance().DisplayHeightAsFloat() / 288), Path(FONT_PATH_CRT));
  mHorizontalOffsetText = std::make_shared<TextComponent>(mWindow, "H OFFSET", font, 0xFFFFFFFF);
  mVerticalOffsetText = std::make_shared<TextComponent>(mWindow, "V OFFSET", font, 0xFFFFFFFF);
  mViewportText = std::make_shared<TextComponent>(mWindow, "WIDTH", font, 0xFFFFFFFF);

  mGrid.setEntry(mHorizontalOffsetText, { 0, 0 }, false);
  mGrid.setEntry(mVerticalOffsetText, { 0, 1 }, false);
  mGrid.setEntry(mViewportText, { 0, 2 }, false);
}

CrtView::~CrtView()
{
  SetResolution(CrtResolution::rNone);
}

void CrtView::Render(const Transform4x4f& parentTrans)
{
  Transform4x4f trans = (parentTrans * getTransform()).round();
  Renderer::SetMatrix(trans);

  Renderer::DrawRectangle(0, 0, Renderer::Instance().DisplayWidthAsInt(), Renderer::Instance().DisplayHeightAsInt(), 0x000000FF);
  mPattern.Render(trans);
  Renderer::SetMatrix(trans);
  Component::Render(trans);

  // Wake up permanently
  mWindow.DoWake();
}

bool CrtView::getHelpPrompts(Help& help)
{
  help.Set(HelpType::AllDirections, _("MOVE SCREEN"))
      .Set(Help::Valid(), _("NEXT RESOLUTION"))
      .Set(Help::Cancel(), _("QUIT"))
      .Set(HelpType::X, _("WIDER"))
      .Set(HelpType::Y, _("NARROWER"));

  return true;
}

void CrtView::SetResolution(CrtResolution resolution)
{
  WindowManager::Finalize();
  Sdl2Init::Finalize();
  Sdl2Init::Initialize();
  int w = mOriginalWidth;
  int h = mOriginalHeight;
  switch(resolution)
  {
    case CrtResolution::r224p: w = 1920; h = 224; break;
    case CrtResolution::r240p: w = 1920; h = 240; break;
    case CrtResolution::r320x240p: w = 320; h = 240; break;
    case CrtResolution::r288p: w = 1920; h = 288; break;
    case CrtResolution::r384x288p: w = 384; h = 288; break;
    case CrtResolution::r480i: w = 640; h = 480; break;
    case CrtResolution::r576i: w = 768; h = 576; break;
    case CrtResolution::r480p: w = 640; h = 480; break;
    case CrtResolution::r240p120Hz: w = 1920; h = 240; break;
    case CrtResolution::_rCount:
    case CrtResolution::rNone: // Original resolution
    default: break;
  }

  mWindow.Initialize(w, h);
  mWindow.normalizeNextUpdate();
  mWindow.UpdateHelp(true);
  mPattern.setResize(0.0f, Renderer::Instance().DisplayHeightAsFloat());
  mPattern.setImage(Path(":/crt/" + CrtConf::CrtResolutionFromEnum(resolution) + ".png"));
  mPattern.setOrigin(.5f, .5f);
  mPattern.setPosition(Renderer::Instance().DisplayWidthAsFloat() / 2.f, Renderer::Instance().DisplayHeightAsFloat() / 2.f, .0f);
  InputManager::Instance().Refresh(&mWindow, false);
}

bool CrtView::ProcessInput(const InputCompactEvent& event)
{
  CrtResolution reso = mSequence[mSequenceIndex];
  bool update = false;
  if (event.CancelReleased()) mEvent.Send(); // Synchronous quit (delete this class)
  else if (event.ValidReleased()) // Validate: go to next screen
  {
    CrtConf::Instance().Save();
    reso = mSequence[++mSequenceIndex];
    if (mSequence[mSequenceIndex] == CrtResolution::rNone)
      mEvent.Send();
    else {
      update = true;
    }
  }
  else if (event.XReleased()) // Wider
  {
    CrtConf::Instance().SetCrtViewportWidth(reso, CrtConf::Instance().GetCrtViewportWidth(reso) + 1);
  }
  else if (event.YReleased()) // Narrower
  {
    CrtConf::Instance().SetCrtViewportWidth(reso, CrtConf::Instance().GetCrtViewportWidth(reso) - 1);
  }
  else if (event.AnyUpReleased())
  {
    CrtConf::Instance().SetCrtModeOffsetVerticalOffset(reso, CrtConf::Instance().GetCrtModeOffsetVerticalOffset(reso) - 1);
    update = true;
  }
  else if (event.AnyDownReleased())
  {
    CrtConf::Instance().SetCrtModeOffsetVerticalOffset(reso, CrtConf::Instance().GetCrtModeOffsetVerticalOffset(reso) + 1);
    update = true;
  }
  else if (event.AnyLeftReleased())
  {
    CrtConf::Instance().SetCrtModeOffsetHorizontalOffset(reso, CrtConf::Instance().GetCrtModeOffsetHorizontalOffset(reso) - 1);
    update = true;
  }
  else if (event.AnyRightReleased())
  {
    CrtConf::Instance().SetCrtModeOffsetHorizontalOffset(reso, CrtConf::Instance().GetCrtModeOffsetHorizontalOffset(reso) + 1);
    update = true;
  }
  if(update)
  {
    CrtConf::Instance().Save();
    SetResolution(reso);
    Initialize();
  }
  UpdateViewport();
  return true;
}

void CrtView::ReceiveSyncMessage()
{
  ViewController::Instance().quitCrtView();
}

void CrtView::UpdateViewport()
{
  CrtResolution reso = mSequence[mSequenceIndex];

  // Reference
  int reference = ((Renderer::Instance().DisplayWidthAsInt()) * 1840) / 1920;

  mPattern.setSize((float) (reference + CrtConf::Instance().GetCrtViewportWidth(reso)), mPattern.getSize().y());
  mPattern.setPosition(Renderer::Instance().DisplayWidthAsFloat() / 2.f, Renderer::Instance().DisplayHeightAsFloat() / 2.f, .0f);

  mViewportText->setText(_("Image width:") + " " + Strings::ToString(CrtConf::Instance().GetCrtViewportWidth(reso)));
  mHorizontalOffsetText->setText(
      _("Horizontal offset:") + " " + Strings::ToString(CrtConf::Instance().GetCrtModeOffsetHorizontalOffset(reso)));
  mVerticalOffsetText->setText(
      _("Vertical offset:") + " " + Strings::ToString(CrtConf::Instance().GetCrtModeOffsetVerticalOffset(reso)));

  if (mOriginalHOffset != CrtConf::Instance().GetCrtModeOffsetHorizontalOffset(reso)) {
    mHorizontalOffsetText->setColor(0xAAAAFFFF);
  } else {
    mHorizontalOffsetText->setColor(0xFFFFFFFF);
  }
  if (mOriginalVOffset != CrtConf::Instance().GetCrtModeOffsetVerticalOffset(reso)) {
    mVerticalOffsetText->setColor(0xAAAAFFFF);
  } else {
    mVerticalOffsetText->setColor(0xFFFFFFFF);
  }
  if (mOriginalViewportWidth != CrtConf::Instance().GetCrtViewportWidth(reso)) {
    mViewportText->setColor(0xAAAAFFFF);
  } else {
    mViewportText->setColor(0xFFFFFFFF);
  }
}
