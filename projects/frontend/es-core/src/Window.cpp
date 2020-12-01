#include <utils/locale/LocaleHelper.h>
#include <Window.h>
#include <Renderer.h>
#include <Settings.h>
#include <guis/GuiInfoPopup.h>
#include <guis/GuiMsgBoxScroll.h>
#include <guis/GuiMsgBox.h>
#include <themes/MenuThemeData.h>
#include <views/ViewController.h>
#include <usernotifications/NotificationManager.h>

Window::Window()
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

Window::~Window()
{
  deleteAllGui();
}

bool Window::UpdateHelpSystem()
{
  Gui* gui = peekGui();
  if (gui != nullptr)
  {
    gui->updateHelpPrompts();
    return true;
  }
  return false;
}

void Window::pushGui(Gui* gui)
{
  mGuiStack.Push(gui);
  UpdateHelpSystem();
}

void Window::displayMessage(const std::string& message, bool urgent)
{
  if (!urgent && !mMessages.empty()) return;

  mMessages.push_back(message);
}

void Window::displayScrollMessage(const std::string& title, const std::string& message, bool urgent)
{
  if (!urgent && !mScrollMessages.empty()) return;

  mScrollTitle.push_back(title);
  mScrollMessages.push_back(message);
}

Gui* Window::peekGui()
{
  if (mGuiStack.Empty())
    return nullptr;

  return mGuiStack.Peek();
}

void Window::deleteClosePendingGui()
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

void Window::deleteAllGui()
{
  for(int i = mInfoPopups.Count(); --i >= 0; )
    delete mInfoPopups[i];
  mInfoPopups.Clear();

  for(int i = mGuiStack.Count(); --i >= 0;)
    delete mGuiStack.PopAt(i);
  mGuiStack.Clear();
}

bool Window::Initialize(unsigned int width, unsigned int height, bool initRenderer)
{
  if (initRenderer)
  {
    if (!Renderer::initialize((int) width, (int) height))
    {
      LOG(LogError) << "Renderer failed to initialize!";
      return false;
    }
  }

  std::string glExts = (const char*) glGetString(GL_EXTENSIONS);
  LOG(LogInfo) << "Checking available OpenGL extensions...";
  LOG(LogInfo) << " ARB_texture_non_power_of_two: "
               << (glExts.find("ARB_texture_non_power_of_two") != std::string::npos ? "OK" : "MISSING");

  InputManager::Instance().Initialize(this);
  ResourceManager::getInstance()->reloadAll();

  //keep a reference to the default fonts, so they don't keep getting destroyed/recreated
  if (mDefaultFonts.empty())
  {
    mDefaultFonts.push_back(Font::get(FONT_SIZE_SMALL));
    mDefaultFonts.push_back(Font::get(FONT_SIZE_MEDIUM));
    mDefaultFonts.push_back(Font::get(FONT_SIZE_LARGE));
  }

  mBackgroundOverlay.setResize(Renderer::getDisplayWidthAsFloat(), Renderer::getDisplayHeightAsFloat());

  // update our help because font sizes probably changed
  UpdateHelpSystem();

  return true;
}

void Window::Finalize()
{
  InputManager::Instance().Finalize();
  ResourceManager::getInstance()->unloadAll();
  Renderer::finalize();
}

void Window::textInput(const char* text)
{
  if (!mGuiStack.Empty())
    mGuiStack.Peek()->textInput(text);
}

bool Window::ProcessInput(const InputCompactEvent& event)
{
  if (mSleeping)
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

void Window::Update(int deltaTime)
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

    if (Settings::Instance().DrawFramerate())
    {
      std::string ss = Strings::ToString(1000.0f * (float) mFrameCountElapsed / (float) mFrameTimeElapsed, 1) +
                       "fps, " + Strings::ToString((float) mFrameTimeElapsed / (float) mFrameCountElapsed, 2) + "ms";

      // vram
      float textureVramUsageMb = TextureResource::getTotalMemUsage() / (1024.0f * 1024.0f);
      float textureTotalUsageMb = TextureResource::getTotalTextureSize() / (1024.0f * 1024.0f);
      float fontVramUsageMb = Font::getTotalMemUsage() / (1024.0f * 1024.0f);
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

void Window::Render(Transform4x4f& transform)
{
  mRenderedHelpPrompts = false;
  bool gameClipEnabled = GameClipView::IsGameClipEnabled();

  // draw only bottom and top of GuiStack (if they are different)
  if (!mGuiStack.Empty())
  {
    int stackSize = (int)mGuiStack.Count();
    auto* previous = stackSize > 1 ? mGuiStack[stackSize - 2] : nullptr;
    auto* top = mGuiStack.Peek();

    mBackgroundOverlay.Render(transform);
    if (top->IsOverlay())
      if (stackSize > 1 && previous != nullptr) previous->Render(transform);

    if (!mRenderedHelpPrompts && top->MustRenderOverHelpSystem())
      renderHelpPromptsEarly();

    top->Render(transform);
  }

  if (!mRenderedHelpPrompts)
    mHelp.Render(transform);

  if (Settings::Instance().DrawFramerate() && mFrameDataText)
  {
    Renderer::setMatrix(Transform4x4f::Identity());
    mDefaultFonts[1]->renderTextCache(mFrameDataText.get());
  }

    if (gameClipEnabled){
        InfoPopupsDisplay(transform);
    }

  unsigned int screensaverTime = (unsigned int) RecalboxConf::Instance().GetScreenSaverTime() * 60000;
  if (mTimeSinceLastInput >= screensaverTime && screensaverTime != 0)
  {
    if (!isProcessing())
    {
        if(gameClipEnabled)
        {
            // do not play game clips if menu opened
            if (mGuiStack.Empty()) {
                ViewController::Instance().goToGameClipView();
            }
            return;
        }



      renderScreenSaver();
      // go to sleep
      if (!isSleeping())
        DoSleep();
    }
  }

  InfoPopupsDisplay(transform);
}

void Window::renderHelpPromptsEarly()
{
  mHelp.Render(Transform4x4f::Identity());
  mRenderedHelpPrompts = true;
}

bool Window::isProcessing()
{
  for(int i = mGuiStack.Count(); --i >= 0;)
    if (mGuiStack[i]->isProcessing())
      return true;
  return false;
}

void Window::exitScreenSaver()
{
  if (Board::Instance().HasBrightnessSupport())
    Board::Instance().SetBrightness(RecalboxConf::Instance().GetBrightness());
}

void Window::renderScreenSaver()
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
  else
  {
    Renderer::setMatrix(Transform4x4f::Identity());
    unsigned char opacity = RecalboxConf::Instance().GetScreenSaverType() == "dim" ? 0xA0 : 0xFF;
    Renderer::drawRect(0, 0, Renderer::getDisplayWidthAsInt(), Renderer::getDisplayHeightAsInt(), 0x00000000 | opacity);
  }
}

bool Window::KonamiCode(const InputCompactEvent& input)
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

void Window::RenderAll(bool halfLuminosity)
{
  Transform4x4f transform(Transform4x4f::Identity());
  Render(transform);
  if (halfLuminosity)
  {
    transform = Transform4x4f::Identity();
    Renderer::setMatrix(transform);
    Renderer::drawRect(0.f, 0.f, Renderer::getDisplayWidthAsFloat(), Renderer::getDisplayHeightAsFloat(), 0x00000080);
  }
  Renderer::swapBuffers();
}

void Window::CloseAll()
{
  for(int i = mGuiStack.Count(); --i >= 0;)
    mGuiStack[i]->Close();
}

void Window::DoWake()
{
  if (mSleeping)
  {
    mTimeSinceLastInput = 0;
    mSleeping = false;
    exitScreenSaver();
    NotificationManager::Instance().Notify(Notification::WakeUp, Strings::ToString(mTimeSinceLastInput));
  }
}

void Window::DoSleep()
{
  if (!mSleeping)
  {
    mSleeping = true;
    NotificationManager::Instance().Notify(Notification::Sleep, Strings::ToString(mTimeSinceLastInput));
  }
}

void Window::InfoPopupsShrink()
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

void Window::InfoPopupRetarget()
{
  int gap = (int)(Renderer::getDisplayHeightAsFloat() * 0.01f);
  if (gap < 2) gap = 2;
  int targetOffset = 0;
  for(int i = 0; i < mInfoPopups.Count(); i++)
  {
    mInfoPopups[i]->SetOffset(targetOffset);
    targetOffset += (int)mInfoPopups[i]->getSize().y() + gap;
  }
}

void Window::InfoPopupAdd(GuiInfoPopup* infoPopup)
{
  mInfoPopups.Add(infoPopup);
  InfoPopupsShrink();
  InfoPopupRetarget();
}

void Window::InfoPopupsRemove(int index)
{
  GuiInfoPopup* popup = mInfoPopups[index];
  mInfoPopups.Delete(index); // Delete pointer
  delete popup; // Delete object

  // Move other popups
  InfoPopupRetarget();
}

void Window::InfoPopupsUpdate(int delta)
{
  for(int i = mInfoPopups.Count(); --i >= 0;)
  {
    mInfoPopups[i]->Update(delta);
    if (mInfoPopups[i]->TimeOut())
      InfoPopupsRemove(i);
  }
}

void Window::InfoPopupsDisplay(Transform4x4f& transform)
{
  for(int i = mInfoPopups.Count(); --i >= 0;)
    mInfoPopups[i]->Render(transform);
}
