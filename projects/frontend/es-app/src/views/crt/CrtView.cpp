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

#define FONT_SIZE_LOADING ((unsigned int)(0.065f * Math::min(Renderer::Instance().DisplayHeightAsFloat(), Renderer::Instance().DisplayWidthAsFloat())))

CrtView::CrtView(WindowManager& window)
  : Gui(window)
  , mPattern(window, true, true)
  , mGrid(window, Vector2i(1, 3))
  , mEvent(this)
  , mOriginalVOffset(CrtConf::Instance().GetSystemCRTVerticalOffset())
  , mOriginalHOffset(CrtConf::Instance().GetSystemCRTHorizontalOffset())
  , mOriginalViewportWidth(CrtConf::Instance().GetSystemCRTViewportWidth())
{
  mPosition.Set(0,0,0);
  mSize.Set(Renderer::Instance().DisplayWidthAsFloat(), Renderer::Instance().DisplayHeightAsFloat());

  bool lowRes = Renderer::Instance().DisplayHeightAsInt() <= 288;
  mStep = lowRes ? 1 : 2;
  mPattern.setResize(0.0f, Renderer::Instance().DisplayHeightAsFloat());
  mPattern.setImage(Path(lowRes ? ":/crt/pattern240.png" : ":/crt/pattern480.png"));
  mPattern.setOrigin(.5f, .5f);
  mPattern.setPosition(Renderer::Instance().DisplayWidthAsFloat() / 2.f, Renderer::Instance().DisplayHeightAsFloat() / 2.f, .0f);

  mGrid.setSize(Renderer::Instance().DisplayWidthAsFloat() / 3.f, Renderer::Instance().DisplayHeightAsFloat() / 3.f);
  mGrid.setPosition(Renderer::Instance().DisplayWidthAsFloat() / 3.f, Renderer::Instance().DisplayHeightAsFloat() / 3.f);
  addChild(&mGrid);

  auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
  mHorizontalOffsetText = std::make_shared<TextComponent>(window, "HZ OFFSET", menuTheme->menuText.font, 0xFFFFFFFF);
  mVerticalOffsetText = std::make_shared<TextComponent>(window, "HZ OFFSET", menuTheme->menuText.font, 0xFFFFFFFF);
  mViewportText = std::make_shared<TextComponent>(window, "HZ OFFSET", menuTheme->menuText.font, 0xFFFFFFFF);

  mGrid.setEntry(mHorizontalOffsetText, { 0, 0 }, false);
  mGrid.setEntry(mVerticalOffsetText, { 0, 1 }, false);
  mGrid.setEntry(mViewportText, { 0, 2 }, false);

  UpdateViewport();
  UpdatePosition();
}

CrtView::~CrtView()
{
  CrtConf::Instance().Save();

  if (system("mount -o remount,rw /boot") != 0) LOG(LogError) <<"[IniFile] Error remounting boot partition (RW)";
  Path(sTimingFile).Delete();
  if (system("mount -o remount,ro /boot") != 0) LOG(LogError) << "[IniFile] Error remounting boot partition (RW)";
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

bool CrtView::ProcessInput(const InputCompactEvent& event)
{
  if (event.CancelPressed()) mEvent.Call(); // Synchroneous quit (delete this class)
  else if (event.ValidPressed()) // Validater: reinit SDL
  {
    mOriginalVOffset = CrtConf::Instance().GetSystemCRTVerticalOffset();
    mOriginalHOffset = CrtConf::Instance().GetSystemCRTHorizontalOffset();
    mOriginalViewportWidth = CrtConf::Instance().GetSystemCRTViewportWidth();
    UpdateViewport();
    UpdatePosition();
    WindowManager::Finalize();
    Sdl2Init::Finalize();
    Sdl2Init::Initialize();
    mWindow.ReInitialize();
    mWindow.normalizeNextUpdate();
    InputManager::Instance().Refresh(&mWindow, false);
  }
  else if (event.XPressed()) // Wider
  {
    CrtConf::Instance().SetSystemCRTViewportWidth(CrtConf::Instance().GetSystemCRTViewportWidth() + mStep);
  }
  else if (event.YPressed()) // Narrower
  {
    CrtConf::Instance().SetSystemCRTViewportWidth(CrtConf::Instance().GetSystemCRTViewportWidth() - mStep);
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
  UpdateViewport();
  return true;
}

void CrtView::ReceiveSyncCallback(const SDL_Event& event)
{
  (void)event;
  ViewController::Instance().quitCrtView();
}

void CrtView::UpdateViewport() {
  // Reference
  int reference = ((Renderer::Instance().DisplayWidthAsInt()) * 1840) / 1920;
  int hoffsetDiff = CrtConf::Instance().GetSystemCRTHorizontalOffset() - mOriginalHOffset;
  int voffsetDiff = CrtConf::Instance().GetSystemCRTVerticalOffset() - mOriginalVOffset;

  mPattern.setSize((float) (reference + CrtConf::Instance().GetSystemCRTViewportWidth()), mPattern.getSize().y());
  mPattern.setPosition(Renderer::Instance().DisplayWidthAsFloat() / 2.f+hoffsetDiff, Renderer::Instance().DisplayHeightAsFloat() / 2.f +voffsetDiff, .0f);

  mViewportText->setText(_("Image width: ") + Strings::ToString(CrtConf::Instance().GetSystemCRTViewportWidth()));
  mHorizontalOffsetText->setText(
      _("Horizontal offset: ") + Strings::ToString(CrtConf::Instance().GetSystemCRTHorizontalOffset()));
  mVerticalOffsetText->setText(
      _("Vertical offset: ") + Strings::ToString(CrtConf::Instance().GetSystemCRTVerticalOffset()));

  if (mOriginalHOffset != CrtConf::Instance().GetSystemCRTHorizontalOffset()) {
    mHorizontalOffsetText->setColor(0xAAAAFFFF);
  } else {
    mHorizontalOffsetText->setColor(0xFFFFFFFF);
  }
  if (mOriginalVOffset != CrtConf::Instance().GetSystemCRTVerticalOffset()) {
    mVerticalOffsetText->setColor(0xAAAAFFFF);
  } else {
    mVerticalOffsetText->setColor(0xFFFFFFFF);
  }
  if (mOriginalViewportWidth != CrtConf::Instance().GetSystemCRTViewportWidth()) {
    mViewportText->setColor(0xAAAAFFFF);
  } else {
    mViewportText->setColor(0xFFFFFFFF);
  }
}

void CrtView::UpdatePosition()
{
  static const char* modes15khz[] =
      {
          "320 1 4 30 46 240 1 4 5 14 0 0 0 60 0 6400000 1",
          "384 1 16 32 40 288 1 3 2 19 0 0 0 50 0 7363200 1",
          "640 1 24 64 104 480 1 3 6 34 0 0 0 60 1 13054080 1",
          "768 1 24 72 88 576 1 6 5 38 0 0 0 50 1 14875000 1",
      };
  static const char* modes31khz[] =
    {
        "640 1 24 96 48 480 1 11 2 32 0 0 0 60 0 25452000 1",
    };
  static constexpr int sHorizontalFrontPorch = 2;
  static constexpr int sHorizontalBackPorch = 4;
  static constexpr int sVerticalLinesActive = 5;
  static constexpr int sVerticalFrontPorch = 7;
  static constexpr int sVerticalBackPorch = 9;
  static constexpr int sInterlaced = 14;


  int hOffset = CrtConf::Instance().GetSystemCRTHorizontalOffset();
  int vOffset = CrtConf::Instance().GetSystemCRTVerticalOffset();


  Strings::Vector result;
  bool khz31 = Board::Instance().CrtBoard().GetHorizontalFrequency() == ICrtInterface::HorizontalFrequency::KHz31;
  // Child Code
  int size =  khz31 ? sizeof(modes31khz)/sizeof(modes31khz[0]) : sizeof(modes15khz)/sizeof(modes15khz[0]);
  auto modes = khz31 ? modes31khz: modes15khz;

  for(int line = 0; line<size; line++)
  {
    Strings::Vector items = Strings::Split(modes[line], ' ', false);
    Array<int> values((int)items.size());
    for(int i = (int)items.size(); --i >= 0; )
      if (!Strings::ToInt(items[i], values(i)))
        LOG(LogError) << "[CrtView] Mode " << line << " contains invalid value at index " << i;

    if (values[sHorizontalFrontPorch] - hOffset > 0 && values[sHorizontalBackPorch] + hOffset > 0)
    {
      values(sHorizontalFrontPorch) -= hOffset;
      values(sHorizontalBackPorch) += hOffset;
    } else {
      values(sHorizontalBackPorch) += values(sHorizontalFrontPorch) -1;
      values(sHorizontalFrontPorch) = 1;
    }
    int min_voffset = 1;
    if(values[sVerticalLinesActive] == 480 && values[sInterlaced] == 1){
      // Horribeul special case for 480i
      min_voffset = 2;
    }
    if (values[sVerticalFrontPorch] - vOffset >= min_voffset && values[sVerticalBackPorch] + vOffset >= min_voffset)
    {
      values(sVerticalFrontPorch) -= vOffset;
      values(sVerticalBackPorch) += vOffset;
    } else {
      values(sVerticalBackPorch) += values(sVerticalFrontPorch) -min_voffset;
      values(sVerticalFrontPorch) = min_voffset;
    }

    for(int i = (int)items.size(); --i >= 0; )
      items[i] = Strings::ToString(values[i]);

    result.push_back(Strings::Join(items, ' '));
  }

  if (system("mount -o remount,rw /boot") != 0) LOG(LogError) <<"[IniFile] Error remounting boot partition (RW)";
  Files::SaveFile(Path(sTimingFile), Strings::Join(result, '\n').append(1, '\n'));
  if (system("mount -o remount,ro /boot") != 0) LOG(LogError) << "[IniFile] Error remounting boot partition (RW)";

}
