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

CrtView::CrtView(WindowManager& window)
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
  , mStep(0)
{
  if (Board::Instance().CrtBoard().GetHorizontalFrequency() == ICrtInterface::HorizontalFrequency::KHz31)
    mSequence = sForced31khz;
  else if (Board::Instance().CrtBoard().MustForce50Hz())
    mSequence = sPALOnly;
  else
  {
    mSequence = sPALNTSC;
    mWindow.pushGui(new GuiMsgBox(mWindow, _("Select standard resolution set\nto calibrate:"),
                                  _("NTSC & PAL"), [this] { },
                                  _("NTSC Only"), [this] { mSequence = sNTSCOnly; },
                                  _("PAL Only"), [this] { mSequence = sPALOnly; },
                                  TextAlignment::Center));
  }
  SetResolution(mSequence[0]);
  Initialize();
}

void CrtView::Initialize()
{
  mPosition.Set(0,0,0);
  mSize.Set(Renderer::Instance().DisplayWidthAsFloat(), Renderer::Instance().DisplayHeightAsFloat());

  mGrid.ClearEntries();
  mGrid.setSize(Renderer::Instance().DisplayWidthAsFloat() * (252.f / 768.f), Renderer::Instance().DisplayHeightAsFloat() * (156.f / 576.f));
  mGrid.setPosition(Renderer::Instance().DisplayWidthAsFloat() * (162.f / 768.f), Renderer::Instance().DisplayHeightAsFloat() * (66.f / 576.f));
  addChild(&mGrid);

  mPattern.setResize(0.0f, Renderer::Instance().DisplayHeightAsFloat());
  mPattern.setImage(Path(":/crt/" + CrtConf::CrtResolutionFromEnum(mSequence[0]) + ".png"));
  mPattern.setOrigin(.5f, .5f);
  mPattern.setPosition(Renderer::Instance().DisplayWidthAsFloat() / 2.f, Renderer::Instance().DisplayHeightAsFloat() / 2.f, .0f);

  auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
  mHorizontalOffsetText = std::make_shared<TextComponent>(mWindow, "H OFFSET", menuTheme->menuText.font, 0xFFFFFFFF);
  mVerticalOffsetText = std::make_shared<TextComponent>(mWindow, "V OFFSET", menuTheme->menuText.font, 0xFFFFFFFF);
  mViewportText = std::make_shared<TextComponent>(mWindow, "WIDTH", menuTheme->menuText.font, 0xFFFFFFFF);

  mGrid.setEntry(mHorizontalOffsetText, { 0, 0 }, false);
  mGrid.setEntry(mVerticalOffsetText, { 0, 1 }, false);
  mGrid.setEntry(mViewportText, { 0, 2 }, false);

  mOriginalVOffset = CrtConf::Instance().GetCrtModeOffsetVerticalOffset(mSequence[0]);
  mOriginalHOffset = CrtConf::Instance().GetCrtModeOffsetHorizontalOffset(mSequence[0]);
  mOriginalViewportWidth = CrtConf::Instance().GetCrtViewportWidth(mSequence[0]);

  UpdateViewport();
  UpdatePosition();
}

CrtView::~CrtView()
{
  SetResolution(CrtResolution::rNone);
  CrtConf::Instance().Save();

  if (RecalboxSystem::MakeBootReadWrite())
  {
    Path(sTimingFile).Delete();
    if (!RecalboxSystem::MakeBootReadOnly()) LOG(LogWarning) << "[CrtView] Boot partition left in writable state";
  }
  else LOG(LogError) << "[CrtView] Error removing CRT timing file.";
}

void CrtView::Render(const Transform4x4f& parentTrans)
{
  Transform4x4f trans = (parentTrans * getTransform()).round();
  Renderer::SetMatrix(trans);

  Renderer::DrawRectangle(0, 0, Renderer::Instance().DisplayWidthAsInt(), Renderer::Instance().DisplayHeightAsInt(), 0x000000FF);
  mPattern.Render(trans);
  Renderer::SetMatrix(trans);
  Renderer::DrawRectangle(Renderer::Instance().DisplayWidthAsInt() / 3, Renderer::Instance().DisplayHeightAsInt() / 3,
                          Renderer::Instance().DisplayWidthAsInt() / 3, Renderer::Instance().DisplayHeightAsInt() / 3, 0x00000080);

  Component::Render(trans);

  // Wake up permanently
  mWindow.DoWake();
}

bool CrtView::getHelpPrompts(Help& help)
{
  help.Set(Help::Cancel(), _("QUIT"))
      .Set(HelpType::AllDirections, _("MOVE SCREEN"))
      .Set(HelpType::X, _("WIDER"))
      .Set(HelpType::Y, _("NARROWER"))
      .Set(Help::Valid(), _("VALIDATE CHANGES"));

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
    case CrtResolution::r224p: w = 320; h = 224; break;
    case CrtResolution::r240p: w = 320; h = 240; break;
    case CrtResolution::r288p: w = 320; h = 288; break;
    case CrtResolution::r480i: w = 640; h = 480; break;
    case CrtResolution::r576i: w = 640; h = 576; break;
    case CrtResolution::r480p: w = 640; h = 480; break;
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

  if (event.CancelPressed()) mEvent.Send(); // Synchronous quit (delete this class)
  else if (event.ValidPressed()) // Validate: reinit SDL
  {
    mOriginalVOffset = CrtConf::Instance().GetCrtModeOffsetVerticalOffset(reso);
    mOriginalHOffset = CrtConf::Instance().GetCrtModeOffsetHorizontalOffset(reso);
    mOriginalViewportWidth = CrtConf::Instance().GetCrtViewportWidth(reso);
    UpdateViewport();
    UpdatePosition();
    SetResolution(mSequence[++mSequenceIndex]);
    Initialize();
    if (mSequence[mSequenceIndex] == CrtResolution::rNone)
      mEvent.Send();
  }
  else if (event.XPressed()) // Wider
  {
    CrtConf::Instance().SetCrtViewportWidth(reso, CrtConf::Instance().GetCrtViewportWidth(reso) + mStep);
  }
  else if (event.YPressed()) // Narrower
  {
    CrtConf::Instance().SetCrtViewportWidth(reso, CrtConf::Instance().GetCrtViewportWidth(reso) - mStep);
  }
  else if (event.AnyUpPressed())
  {
    CrtConf::Instance().SetCrtModeOffsetVerticalOffset(reso, CrtConf::Instance().GetCrtModeOffsetVerticalOffset(reso) - 1);
  }
  else if (event.AnyDownPressed())
  {
    CrtConf::Instance().SetCrtModeOffsetVerticalOffset(reso, CrtConf::Instance().GetCrtModeOffsetVerticalOffset(reso) + 1);
  }
  else if (event.AnyLeftPressed())
  {
    CrtConf::Instance().SetCrtModeOffsetHorizontalOffset(reso, CrtConf::Instance().GetCrtModeOffsetHorizontalOffset(reso) - 1);
  }
  else if (event.AnyRightPressed())
  {
    CrtConf::Instance().SetCrtModeOffsetHorizontalOffset(reso, CrtConf::Instance().GetCrtModeOffsetHorizontalOffset(reso) + 1);
  }
  // If we are in force 50HZ, then we copy the offsets to pal ones
  if(Board::Instance().CrtBoard().MustForce50Hz())
  {
    CrtConf::Instance().SetCrtModeOffsetVerticalOffset(reso, CrtConf::Instance().GetCrtModeOffsetVerticalOffset(reso));
    CrtConf::Instance().SetCrtModeOffsetHorizontalOffset(reso, CrtConf::Instance().GetCrtModeOffsetHorizontalOffset(reso));
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
  const int hOffSetMultiplier = Renderer::Instance().DisplayWidthAsInt() / 320;
  int hoffsetDiff = (CrtConf::Instance().GetCrtModeOffsetHorizontalOffset(reso) - mOriginalHOffset)*hOffSetMultiplier;
  int voffsetDiff = CrtConf::Instance().GetCrtModeOffsetVerticalOffset(reso) - mOriginalVOffset;

  mPattern.setSize((float) (reference + CrtConf::Instance().GetCrtViewportWidth(reso)), mPattern.getSize().y());
  mPattern.setPosition(Renderer::Instance().DisplayWidthAsFloat() / 2.f + (float)hoffsetDiff, Renderer::Instance().DisplayHeightAsFloat() / 2.f + (float)voffsetDiff, .0f);

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

void CrtView::UpdatePosition()
{
  static HashMap<CrtResolution, std::string> modes =
  {
    {  CrtResolution::r224p, "1920 1 80 184 312 224 1 10 3 24 0 0 0 60 0 39087360 1" },
    {  CrtResolution::r240p, "1920 1 80 184 312 240 1 1 3 16 0 0 0 60 0 38937600 1" },
    {  CrtResolution::r288p, "1920 1 80 184 312 288 1 4 3 18 0 0 0 50 0 39062400 1" },
    {  CrtResolution::r480i, "640 1 24 64 104 480 1 3 6 34 0 0 0 60 1 13054080 1" },
    {  CrtResolution::r576i, "768 1 24 72 88 576 1 6 5 38 0 0 0 50 1 14875000 1" },
    {  CrtResolution::r480p, "640 1 24 96 48 480 1 11 2 32 0 0 0 60 0 25452000 1" },
  };

  static constexpr int sHorizontalFrontPorch = 2;
  static constexpr int sHorizontalBackPorch = 4;
  static constexpr int sVerticalLinesActive = 5;
  static constexpr int sVerticalFrontPorch = 7;
  static constexpr int sVerticalBackPorch = 9;
  static constexpr int sInterlaced = 14;

  CrtResolution reso = mSequence[mSequenceIndex];

  // Get offsets
  const int hOffSetMultiplier = Renderer::Instance().RealDisplayWidthAsInt() / 320;
  int hOffset = CrtConf::Instance().GetCrtModeOffsetHorizontalOffset(reso) * hOffSetMultiplier;
  int vOffset = CrtConf::Instance().GetCrtModeOffsetVerticalOffset(reso);

  // Split and convert current modeline
  Strings::Vector items = Strings::Split(modes[reso], ' ', false);
  Array<int> values((int)items.size());
  for(int i = (int)items.size(); --i >= 0; )
    if (!Strings::ToInt(items[i], values(i)))
    { LOG(LogError) << "[CrtView] Mode " << modes[reso] << " contains invalid value at index " << i; }

  // Horizontal porch
  if (values[sHorizontalFrontPorch] - hOffset > 0 && values[sHorizontalBackPorch] + hOffset > 0)
  {
    values(sHorizontalFrontPorch) -= hOffset;
    values(sHorizontalBackPorch) += hOffset;
  } else {
    values(sHorizontalBackPorch) += values(sHorizontalFrontPorch) -1;
    values(sHorizontalFrontPorch) = 1;
  }

  // Vertical porch
  int min_voffset = 1;
  if(values[sVerticalLinesActive] == 480 && values[sInterlaced] == 1) min_voffset = 2; // Horribeul special case for 480i
  if (values[sVerticalFrontPorch] - vOffset >= min_voffset && values[sVerticalBackPorch] + vOffset >= min_voffset)
  {
    values(sVerticalFrontPorch) -= vOffset;
    values(sVerticalBackPorch) += vOffset;
  } else {
    values(sVerticalBackPorch) += values(sVerticalFrontPorch) -min_voffset;
    values(sVerticalFrontPorch) = min_voffset;
  }

  // Rebuild mode line
  for(int i = (int)items.size(); --i >= 0; )
    items[i] = Strings::ToString(values[i]);
  modes[reso] = Strings::Join(items, ' ');

  // Rebuild modeline file
  std::string result;
  for(const auto& kv : modes)
    result.append(kv.second).append(1, '\n');

  if (RecalboxSystem::MakeBootReadWrite())
  {
    Files::SaveFile(Path(sTimingFile), result);
    if (!RecalboxSystem::MakeBootReadOnly()) LOG(LogWarning) << "[CrtView] Boot partition left in writable state";
  }
  else LOG(LogError) << "[CrtView] Error writing CRT timing file.";
}

