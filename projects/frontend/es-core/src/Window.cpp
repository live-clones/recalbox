#include "Window.h"
#include "Renderer.h"
#include "utils/Log.h"
#include "Settings.h"
#include <algorithm>
#include <iomanip>
#include <guis/GuiMsgBoxScroll.h>
#include <guis/GuiInfoPopup.h>
#include "components/HelpComponent.h"
#include "components/ImageComponent.h"
#include "guis/GuiMsgBox.h"
#include "Locale.h"
#include "MenuThemeData.h"
#include "views/ViewController.h"

Window::Window()
  : mHelp(*this),
    mBackgroundOverlay(*this),
    mInfoPopup(nullptr),
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
  mGuiStack.push_back(gui);
  UpdateHelpSystem();
}

void Window::displayMessage(const std::string& message)
{
  mMessages.push_back(message);
}

void Window::displayScrollMessage(const std::string& title, const std::string& message)
{
  mScrollTitle.push_back(title);
  mScrollMessages.push_back(message);
}

Gui* Window::peekGui()
{
  if (mGuiStack.empty())
    return nullptr;

  return mGuiStack.back();
}

void Window::deleteClosePendingGui()
{
  for(auto it = mGuiStack.begin(); it != mGuiStack.end(); it++)
  {
    if ((*it)->IsPendingForDeletion())
    {
      delete *it;
      mGuiStack.erase(it--);
    }
  }
  // Refresg help system
  UpdateHelpSystem();
}

void Window::deleteAllGui()
{
  while(!mGuiStack.empty())
  {
    Gui* topGui = mGuiStack.back();
    mGuiStack.pop_back();
    delete topGui;
  }
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

  InputManager::Instance().Initialize();
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
  if (!mGuiStack.empty())
    mGuiStack.back()->textInput(text);
}

bool Window::ProcessInput(const InputCompactEvent& event)
{
  if (mSleeping)
  {
    // wake up
    mTimeSinceLastInput = 0;
    mSleeping = false;
    return false;
  }

  mTimeSinceLastInput = 0;
  if (peekGui() != nullptr)
  {
    peekGui()->ProcessInput(event);
    // Top Gui always consume events
    return true;
  }

  /*	if(peekGui() && !KonamiCode(config, input, this))
    {
      this->peekGui()->input(config, input);
    }
  */
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

      mFrameDataText = std::unique_ptr<TextCache>(mDefaultFonts.at(1)->buildTextCache(ss, 50.f, 50.f, 0xFF00FFFF));
    }

    mFrameTimeElapsed = 0;
    mFrameCountElapsed = 0;
  }

  mTimeSinceLastInput += deltaTime;

  // Process GUI pending for deletion
  deleteClosePendingGui();

  // Process highest GUI
  if (!mGuiStack.empty())
    mGuiStack.back()->Update(deltaTime);
}

void Window::Render(Transform4x4f& transform)
{
  mRenderedHelpPrompts = false;

  // draw only bottom and top of GuiStack (if they are different)
  if (!mGuiStack.empty())
  {
    auto& bottom = mGuiStack.front();
    auto& top = mGuiStack.back();

    bottom->Render(transform);
    if (bottom != top)
    {
      mBackgroundOverlay.Render(transform);
      top->Render(transform);
    }
  }

  if (!mRenderedHelpPrompts)
    mHelp.Render(transform);

  if (Settings::Instance().DrawFramerate() && mFrameDataText)
  {
    Renderer::setMatrix(Transform4x4f::Identity());
    mDefaultFonts.at(1)->renderTextCache(mFrameDataText.get());
  }

  unsigned int screensaverTime = (unsigned int) Settings::Instance().ScreenSaverTime();
  if (mTimeSinceLastInput >= screensaverTime && screensaverTime != 0)
  {
    if (!isProcessing())
    {
      renderScreenSaver();
      // go to sleep
      mSleeping = true;
    }
  }
  if (mInfoPopup)
  {
    mInfoPopup->Render(transform);
  }
}

void Window::renderHelpPromptsEarly()
{
  mHelp.Render(Transform4x4f::Identity());
  mRenderedHelpPrompts = true;
}

bool Window::isProcessing()
{
  return count_if(mGuiStack.begin(), mGuiStack.end(), [](Component* c)
  { return c->isProcessing(); }) > 0;
}

void Window::renderScreenSaver()
{
  Renderer::setMatrix(Transform4x4f::Identity());
  unsigned char opacity = Settings::Instance().ScreenSaverBehavior() == "dim" ? 0xA0 : 0xFF;
  Renderer::drawRect(0, 0, Renderer::getDisplayWidthAsInt(), Renderer::getDisplayHeightAsInt(), 0x00000000 | opacity);
}

bool Window::KonamiCode(InputDevice* config, InputEvent input, Window& window)
{
  (void) window;

  if (input.Value() == 0) return false;

  bool codeOk = false;

  for (auto valstring : mInputVals)
  {
    if (config->IsMatching(valstring, input) && (this->mKonami[this->mKonamiCount] == valstring))
    {
      this->mKonamiCount++;
      codeOk = true;
    }
  }

  if (!codeOk)
  {
    this->mKonamiCount = 0; // current input is incorrect, reset counter
  }

  if (this->mKonamiCount == sKonamiLength)
  {
    auto s = std::make_shared<GuiInfoPopup>(*this, "I entered Konami Code and all I get is this lame popup", 4, 50);
    this->setInfoPopup(s);
    return true;
  }
  return false;
}

void Window::RenderAll()
{
  Transform4x4f transform(Transform4x4f::Identity());
  Render(transform);
  Renderer::swapBuffers();
}

void Window::CloseAll()
{
  for(Gui* gui : mGuiStack)
    gui->Close();
}
