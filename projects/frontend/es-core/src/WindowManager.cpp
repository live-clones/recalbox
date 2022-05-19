#include <utils/locale/LocaleHelper.h>
#include <WindowManager.h>
#include <Renderer.h>
#include <guis/GuiInfoPopupBase.h>
#include <guis/GuiMsgBoxScroll.h>
#include <guis/GuiMsgBox.h>
#include <themes/MenuThemeData.h>
#include <views/ViewController.h>
#include <usernotifications/NotificationManager.h>

WindowManager::WindowManager()
  : mHelp(*this),
    mBackgroundOverlay(*this),
    mInfoPopups(sMaxInfoPopups),
    mGuiStack(16), // Allocate memory once for all gui
    mFrameTimeElapsed(0),
    mFrameCountElapsed(0),
    mAverageDeltaTime(10),
    mTimeSinceLastInput(0),
    mNormalizeNextUpdate(false),
    mSleeping(false),
    mRenderedHelpPrompts(false)
{
  auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
  mBackgroundOverlay.setImage(menuTheme->menuBackground.fadePath);
}

WindowManager::~WindowManager()
{
  deleteAllGui();
}

bool WindowManager::UpdateHelpSystem()
{
  Gui* gui = peekGui();
  if (gui != nullptr)
  {
    gui->updateHelpPrompts();
    return true;
  }
  return false;
}

void WindowManager::pushGui(Gui* gui)
{
  mGuiStack.Push(gui);
  UpdateHelpSystem();
}

void WindowManager::RemoveGui(Gui* gui)
{
  for(int i = mGuiStack.Count(); --i >= 0;)
    if (mGuiStack[i] == gui)
      mGuiStack.PopAt(i);
}

void WindowManager::displayMessage(const std::string& message, bool urgent)
{
  if (!urgent && !mMessages.empty()) return;

  mMessages.push_back(message);
}

void WindowManager::displayScrollMessage(const std::string& title, const std::string& message, bool urgent)
{
  if (!urgent && !mScrollMessages.empty()) return;

  mScrollTitle.push_back(title);
  mScrollMessages.push_back(message);
}

Gui* WindowManager::peekGui()
{
  if (mGuiStack.Empty())
    return nullptr;

  return mGuiStack.Peek();
}

void WindowManager::deleteClosePendingGui()
{
  bool deleted = false;
  for(int i = mGuiStack.Count(); --i >= 0;)
    if (mGuiStack[i]->IsPendingForDeletion())
    {
      delete mGuiStack.PopAt(i);
      deleted = true;
    }

  // Refresh help system
  if (deleted)
    UpdateHelpSystem();
}

void WindowManager::deleteAllGui()
{
  for(int i = mInfoPopups.Count(); --i >= 0; )
    delete mInfoPopups[i];
  mInfoPopups.Clear();

  for(int i = mGuiStack.Count(); --i >= 0;)
    delete mGuiStack.PopAt(i);
  mGuiStack.Clear();
}

bool WindowManager::ReInitialize()
{
  // Reinit video
  if (!Renderer::Instance().ReInitialize())
  {
    { LOG(LogError) << "[WindowManager] Renderer failed to initialize!"; }
    return false;
  }
  // Reload resources
  ResourceManager::getInstance()->reloadAll();
  // Update help system
  UpdateHelpSystem();

  return true;
}

bool WindowManager::Initialize(unsigned int width, unsigned int height, bool initRenderer)
{
  if (initRenderer)
  {
    if (!Renderer::Instance().Initialize((int) width, (int) height))
    {
      { LOG(LogError) << "[WindowManager] Renderer failed to initialize!"; }
      return false;
    }
  }

  std::string glExts = (const char*) glGetString(GL_EXTENSIONS);
  { LOG(LogInfo) << "[WindowManager] Checking available OpenGL extensions..."; }
  { LOG(LogInfo) << "[WindowManager] ARB_texture_non_power_of_two: "
                 << (glExts.find("ARB_texture_non_power_of_two") != std::string::npos ? "OK" : "MISSING"); }

  //InputManager::Instance().Initialize(this);
  ResourceManager::getInstance()->reloadAll();

  //keep a reference to the default fonts, so they don't keep getting destroyed/recreated
  if (mDefaultFonts.empty())
  {
    mDefaultFonts.push_back(Font::get(FONT_SIZE_SMALL));
    mDefaultFonts.push_back(Font::get(FONT_SIZE_MEDIUM));
    mDefaultFonts.push_back(Font::get(FONT_SIZE_LARGE));
  }

  mBackgroundOverlay.setResize(Renderer::Instance().DisplayWidthAsFloat(), Renderer::Instance().DisplayHeightAsFloat());

  // update our help because font sizes probably changed
  UpdateHelpSystem();

  return true;
}

void WindowManager::Finalize()
{
  //InputManager::Instance().Finalize();
  ResourceManager::getInstance()->unloadAll();
  Renderer::Instance().Finalize();
}

void WindowManager::textInput(const char* text)
{
  if (!mGuiStack.Empty())
    mGuiStack.Peek()->textInput(text);
}

bool WindowManager::ProcessInput(const InputCompactEvent& event)
{
  if (mSleeping && !GameClipView::IsGameClipEnabled())
  {
    // wake up
    DoWake();
    return true;
  }

  mTimeSinceLastInput = 0;
  if (peekGui() != nullptr)
  {
    peekGui()->ProcessInput(event);
    // Top Gui always consume events
    return true;
  }

  if (KonamiCode(event))
  {
    // Surprise!
  }

  return false;
}

void WindowManager::Update(int deltaTime)
{
  if (!mMessages.empty())
  {
    std::string message = mMessages.back();
    mMessages.pop_back();
    pushGui(new GuiMsgBox(*this, message));
  }

  if (!mScrollMessages.empty())
  {
    std::string message = mScrollMessages.back();
    std::string title = mScrollTitle.back();
    mScrollMessages.pop_back();
    mScrollTitle.pop_back();
    pushGui(new GuiMsgBoxScroll(*this, title, message, _("OK"), []
    {}, "", nullptr, "", nullptr, TextAlignment::Left));
  }

  if (mNormalizeNextUpdate)
  {
    mNormalizeNextUpdate = false;
    if (deltaTime > mAverageDeltaTime)
      deltaTime = mAverageDeltaTime;
  }

  mFrameTimeElapsed += deltaTime;
  mFrameCountElapsed++;
  if (mFrameTimeElapsed > 500)
  {
    mAverageDeltaTime = mFrameTimeElapsed / mFrameCountElapsed;

    if (RecalboxConf::Instance().GetGlobalShowFPS())
    {
      std::string ss = Strings::ToString(1000.0f * (float) mFrameCountElapsed / (float) mFrameTimeElapsed, 1) +
                       "fps, " + Strings::ToString((float) mFrameTimeElapsed / (float) mFrameCountElapsed, 2) + "ms";

      // vram
      float textureVramUsageMb = (float)TextureResource::getTotalMemUsage() / (1024.0f * 1024.0f);
      float textureTotalUsageMb = (float)TextureResource::getTotalTextureSize() / (1024.0f * 1024.0f);
      float fontVramUsageMb = (float)Font::getTotalMemUsage() / (1024.0f * 1024.0f);
      ss += "\nFont VRAM: " + Strings::ToString(fontVramUsageMb, 2) + " Tex VRAM: " +
            Strings::ToString(textureVramUsageMb, 2) + " Tex Max: " + Strings::ToString(textureTotalUsageMb, 2);

      mFrameDataText = std::unique_ptr<TextCache>(mDefaultFonts[1]->buildTextCache(ss, 50.f, 50.f, 0xFF00FFFF));
    }

    mFrameTimeElapsed = 0;
    mFrameCountElapsed = 0;
  }

  mTimeSinceLastInput += deltaTime;

  // Process GUI pending for deletion
  deleteClosePendingGui();

  mHelp.Update(deltaTime);

  // Process highest GUI
  if (!mGuiStack.Empty())
    mGuiStack.Peek()->Update(deltaTime);

  // Process popups
  InfoPopupsUpdate(deltaTime);
}

void WindowManager::Render(Transform4x4f& transform)
{
  mRenderedHelpPrompts = false;

  // draw only bottom and top of GuiStack (if they are different)
  if (!mGuiStack.Empty())
  {
    int stackSize = (int)mGuiStack.Count();
    Gui* previous = stackSize > 1 ? mGuiStack[stackSize - 2] : nullptr;
    Gui* top = mGuiStack.Peek();

    mBackgroundOverlay.Render(transform);
    if (top->IsOverlay())
      if (stackSize > 1 && previous != nullptr) previous->Render(transform);

    if (top->MustRenderOverHelpSystem())
      renderHelpPromptsEarly();

    top->Render(transform);
  }

  if (!mRenderedHelpPrompts)
    mHelp.Render(transform);

  if (RecalboxConf::Instance().GetGlobalShowFPS() && mFrameDataText)
  {
    Renderer::SetMatrix(Transform4x4f::Identity());
    mDefaultFonts[1]->renderTextCache(mFrameDataText.get());
  }

  unsigned int screensaverTime = (unsigned int) RecalboxConf::Instance().GetScreenSaverTime() * 60000;
  if (mTimeSinceLastInput >= screensaverTime && screensaverTime != 0)
  {
    if (!isProcessing())
    {
      renderScreenSaver();

      // go to sleep
      if (!isSleeping())
        DoSleep();
    }
  }

  Renderer::SetMatrix(Transform4x4f::Identity());
  DisplayBatteryState();

  InfoPopupsDisplay(transform);
}

void WindowManager::renderHelpPromptsEarly()
{
  mHelp.Render(Transform4x4f::Identity());
  mRenderedHelpPrompts = true;
}

bool WindowManager::isProcessing()
{
  for(int i = mGuiStack.Count(); --i >= 0;)
    if (mGuiStack[i]->isProcessing())
      return true;
  return false;
}

void WindowManager::exitScreenSaver()
{
  if (Board::Instance().HasBrightnessSupport())
    Board::Instance().SetBrightness(RecalboxConf::Instance().GetBrightness());
}

void WindowManager::renderScreenSaver()
{
  if (Board::Instance().HasSuspendResume() && RecalboxConf::Instance().GetScreenSaverType() == "suspend")
  {
    Board::Instance().Suspend();
    DoWake(); // Exit screensaver immediately on resume
  }
  else if (Board::Instance().HasBrightnessSupport())
  {
    std::string screenSaver = RecalboxConf::Instance().GetScreenSaverType();
    if (screenSaver == "black") Board::Instance().SetLowestBrightness();
    else if (screenSaver == "dim")
    {
      int brightness = RecalboxConf::Instance().GetBrightness();
      Board::Instance().SetBrightness(brightness >> 1);
    }
  }
  else if (RecalboxConf::Instance().GetScreenSaverType() == "gameclip")
  {
    if (mGuiStack.Empty())
      ViewController::Instance().goToGameClipView();
  }
  else
  {
    Renderer::SetMatrix(Transform4x4f::Identity());
    unsigned char opacity = RecalboxConf::Instance().GetScreenSaverType() == "dim" ? 0xA0 : 0xFF;
    Renderer::DrawRectangle(0, 0, Renderer::Instance().DisplayWidthAsInt(), Renderer::Instance().DisplayHeightAsInt(), 0x00000000 | opacity);
  }
}

bool WindowManager::KonamiCode(const InputCompactEvent& input)
{
  typedef bool (InputCompactEvent::*EventMethodPointer)() const;

  static constexpr int sKonamiLength = 10;
  static EventMethodPointer sKonami[sKonamiLength] =
  {
    &InputCompactEvent::AnyUpPressed,
    &InputCompactEvent::AnyUpPressed,
    &InputCompactEvent::AnyDownPressed,
    &InputCompactEvent::AnyDownPressed,
    &InputCompactEvent::AnyLeftPressed,
    &InputCompactEvent::AnyRightPressed,
    &InputCompactEvent::AnyLeftPressed,
    &InputCompactEvent::AnyRightPressed,
    &InputCompactEvent::BPressed,
    &InputCompactEvent::APressed,
  };
  static int KonamiCount = 0;

  if (!(input.*(sKonami[KonamiCount++]))()) KonamiCount = 0; // Reset on no matching
  if (KonamiCount == sKonamiLength)
  {
    KonamiCount = 0;
    return true; // Yes konami code!
  }

  return false;
}

void WindowManager::DisplayBatteryState()
{
  if (!Board::Instance().HasBattery() ||
      RecalboxConf::Instance().GetBatteryHidden()) return;

  int charge = Board::Instance().BatteryChargePercent();
  UnicodeChar unicodeIcon = 0xf1b4;
  if (!Board::Instance().IsBatteryCharging())
  {
    if (charge >= 66)      unicodeIcon = 0xF1ba;
    else if (charge >= 33) unicodeIcon = 0xF1b8;
    else if (charge >= 15) unicodeIcon = 0xF1b1;
    else                   unicodeIcon = 0xF1b5;
  }

  //int charge = 9; UnicodeChar unicodeIcon = 0xF1b1;
  unsigned int color = 0xFFFFFFFF;
  if (charge < 15) color = 0xFF8000FF;
  if ((charge < 10) && ((SDL_GetTicks() >> 8) & 3) == 0) color = 0xFF0000FF;

  Font* font = Font::get(Renderer::Instance().IsSmallResolution() ? (int)FONT_SIZE_LARGE : (int)FONT_SIZE_MEDIUM).get();
  Font::Glyph& glyph = font->Character(unicodeIcon);
  float glyphWidth = glyph.texSize.x() * (float) glyph.texture->textureSize.x();
  float glyphHeight = glyph.texSize.y() * (float) glyph.texture->textureSize.y();
  float x = Renderer::Instance().DisplayWidthAsFloat() - Math::round(glyphWidth * 1.25f);
  float y = Math::round(glyphHeight * 0.25f);
  font->renderCharacter(unicodeIcon, x + 1, y, 1.f, 1.f, 0xFF);
  font->renderCharacter(unicodeIcon, x - 1, y, 1.f, 1.f, 0xFF);
  font->renderCharacter(unicodeIcon, x, y + 1, 1.f, 1.f, 0xFF);
  font->renderCharacter(unicodeIcon, x, y - 1, 1.f, 1.f, 0xFF);
  font->renderCharacter(unicodeIcon, x, y, 1.f, 1.f, color);
}

void WindowManager::RenderAll(bool halfLuminosity)
{
  Transform4x4f transform(Transform4x4f::Identity());
  Render(transform);
  if (halfLuminosity)
  {
    Renderer::SetMatrix(Transform4x4f::Identity());
    Renderer::DrawRectangle(0.f, 0.f, Renderer::Instance().DisplayWidthAsFloat(), Renderer::Instance().DisplayHeightAsFloat(), 0x00000080);
  }
  Renderer::Instance().SwapBuffers();
}

void WindowManager::CloseAll()
{
  for(int i = mGuiStack.Count(); --i >= 0;)
    mGuiStack[i]->Close();
}

void WindowManager::DoWake()
{
  mTimeSinceLastInput = 0;
  if (mSleeping)
  {
    mSleeping = false;
    exitScreenSaver();
    NotificationManager::Instance().Notify(Notification::WakeUp, Strings::ToString(mTimeSinceLastInput));
  }
}

void WindowManager::DoSleep()
{
  if( RecalboxConf::Instance().GetScreenSaverType() == "gameclip" && !mGuiStack.Empty())
    return;
  
  if (!mSleeping)
  {
    mSleeping = true;
    NotificationManager::Instance().Notify(Notification::Sleep, Strings::ToString(mTimeSinceLastInput));
  }
}

void WindowManager::InfoPopupsShrink()
{
  int types = 0;
  for(int i = mInfoPopups.Count(); --i >= 0; )
    if ((types & (1 << (int)mInfoPopups[i]->Type())) == 0)
      types |= (1 << (int)mInfoPopups[i]->Type());
    else
      mInfoPopups.Delete(i);

  // Security
  while(mInfoPopups.Count() >= sMaxInfoPopups)
    InfoPopupsRemove(0);
}

void WindowManager::InfoPopupRetarget()
{
  int gap = (int)(Renderer::Instance().DisplayHeightAsFloat() * 0.01f);
  if (gap < 2) gap = 2;
  int targetOffset = 0;
  for(int i = 0; i < mInfoPopups.Count(); i++)
  {
    mInfoPopups[i]->SetOffset(targetOffset);
    targetOffset += (int)mInfoPopups[i]->getSize().y() + gap;
  }
}

void WindowManager::InfoPopupAdd(GuiInfoPopupBase* infoPopup, bool first)
{
  infoPopup->Initialize();
  if (first)mInfoPopups.Insert(infoPopup, 0);
  else mInfoPopups.Add(infoPopup);
  InfoPopupsShrink();
  InfoPopupRetarget();
}

bool WindowManager::InfoPopupIsShown(GuiInfoPopupBase* infoPopup)
{
  for(int i = mInfoPopups.Count(); --i >= 0;)
    if (mInfoPopups[i] == infoPopup)
      return true;
  return false;
}

void WindowManager::InfoPopupRemove(GuiInfoPopupBase* infoPopup)
{
  for(int i = mInfoPopups.Count(); --i >= 0;)
    if (mInfoPopups[i] == infoPopup)
      InfoPopupsRemove(i);
}

void WindowManager::InfoPopupsRemove(int index)
{
  GuiInfoPopupBase* popup = mInfoPopups[index];
  mInfoPopups.Delete(index); // Delete pointer
  if (!popup->SelfProcessed())
    delete popup; // Delete object

  // Move other popups
  InfoPopupRetarget();
}

void WindowManager::InfoPopupsUpdate(int delta)
{
  for(int i = mInfoPopups.Count(); --i >= 0;)
  {
    mInfoPopups[i]->Update(delta);
    if (mInfoPopups[i]->TimeOut() && !mInfoPopups[i]->SelfProcessed())
      InfoPopupsRemove(i);
  }
}

void WindowManager::InfoPopupsDisplay(Transform4x4f& transform)
{
  for(int i = mInfoPopups.Count(); --i >= 0;)
    mInfoPopups[i]->Render(transform);
}

