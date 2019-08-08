#include "Window.h"
#include <iostream>
#include "Renderer.h"
#include "Log.h"
#include "Settings.h"
#include <algorithm>
#include <iomanip>
#include <guis/GuiMsgBoxScroll.h>
#include <guis/GuiInfoPopup.h>
#include "components/HelpComponent.h"
#include "components/ImageComponent.h"
#include "guis/GuiMsgBox.h"
#include "RecalboxConf.h"
#include "Locale.h"
#include "MenuThemeData.h"
#include "views/ViewController.h"

Window::Window()
  : mInfoPopup(nullptr),
    mFrameTimeElapsed(0),
    mFrameCountElapsed(0),
    mAverageDeltaTime(10),
		mNormalizeNextUpdate(false),
	  mAllowSleep(true),
	  mSleeping(false),
	  mTimeSinceLastInput(0),
	  mRenderedHelpPrompts(false)
{
	mHelp = new HelpComponent(this);
	mBackgroundOverlay = new ImageComponent(this);
	auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
	mBackgroundOverlay->setImage(menuTheme->menuBackground.fadePath);
}

Window::~Window() {
	delete mBackgroundOverlay;
	deleteAllGui();
	delete mHelp;
}

void Window::pushGui(GuiComponent* gui)
{
	mGuiStack.push_back(gui);
	gui->updateHelpPrompts();
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

void Window::removeGui(GuiComponent* gui)
{
	for (auto i = mGuiStack.begin(); i != mGuiStack.end(); i++)
	{
		if(*i == gui)
		{
			i = mGuiStack.erase(i);

			if(i == mGuiStack.end() && !mGuiStack.empty()) // we just popped the stack and the stack is not empty
				mGuiStack.back()->updateHelpPrompts();

			return;
		}
	}
}

GuiComponent* Window::peekGui()
{
	if(mGuiStack.empty())
		return nullptr;

	return mGuiStack.back();
}

void Window::deleteAllGui() {
	while (peekGui()) {
		delete peekGui();
	}
}

bool Window::init(unsigned int width, unsigned int height, bool initRenderer)
{
    if (initRenderer) {
        if(!Renderer::init((int)width, (int)height))
        {
            LOG(LogError) << "Renderer failed to initialize!";
            return false;
        }
    }

	InputManager::getInstance()->init();

	ResourceManager::getInstance()->reloadAll();

	//keep a reference to the default fonts, so they don't keep getting destroyed/recreated
	if(mDefaultFonts.empty())
	{
		mDefaultFonts.push_back(Font::get(FONT_SIZE_SMALL));
		mDefaultFonts.push_back(Font::get(FONT_SIZE_MEDIUM));
		mDefaultFonts.push_back(Font::get(FONT_SIZE_LARGE));
	}

	mBackgroundOverlay->setResize((float)Renderer::getScreenWidth(), (float)Renderer::getScreenHeight());

	// update our help because font sizes probably changed
	if(peekGui())
		peekGui()->updateHelpPrompts();

	return true;
}

void Window::deinit()
{
	InputManager::getInstance()->deinit();
	ResourceManager::getInstance()->unloadAll();
	Renderer::deinit();
}

void Window::textInput(const char* text)
{
	if(peekGui())
		peekGui()->textInput(text);
}

void Window::input(InputConfig* config, Input input)
{
	if(mSleeping)
	{
		// wake up
		mTimeSinceLastInput = 0;
		mSleeping = false;
		onWake();
		return;
	}

	mTimeSinceLastInput = 0;

	if(config->getDeviceId() == DEVICE_KEYBOARD && input.value && input.id == SDLK_g && SDL_GetModState() & KMOD_LCTRL && Settings::getInstance()->getBool("Debug"))
	{
		// toggle debug grid with Ctrl-G
		Settings::getInstance()->setBool("DebugGrid", !Settings::getInstance()->getBool("DebugGrid"));
	}
	else if(config->getDeviceId() == DEVICE_KEYBOARD && input.value && input.id == SDLK_t && SDL_GetModState() & KMOD_LCTRL && Settings::getInstance()->getBool("Debug"))
	{
		// toggle TextComponent debug view with Ctrl-T
		Settings::getInstance()->setBool("DebugText", !Settings::getInstance()->getBool("DebugText"));
	}
	else if(peekGui())
	{
		this->peekGui()->input(config, input);
	}

	//else
	//{

		/*if(config->isMappedTo("PageDown", input) && input.value
					&& ViewController::get()->isViewing(ViewController::SYSTEM_SELECT)
					&& mGuiStack.size() == 1 )
		{
			auto s = std::make_shared<GuiInfoPopup>(this, "Controller:\n" + config->getDeviceName(), 2);
			this->setInfoPopup(s);
		}*/
	/*	if(peekGui() && !KonamiCode(config, input, this))
		{
			this->peekGui()->input(config, input);
		}
	}*/
}

void Window::update(int deltaTime)
{
	if(!mMessages.empty()){
		std::string message = mMessages.back();
		mMessages.pop_back();
		pushGui(new GuiMsgBox(this, message));
	}
	if(!mScrollMessages.empty()){
		std::string message = mScrollMessages.back();
		std::string title = mScrollTitle.back();
		mScrollMessages.pop_back();
		mScrollTitle.pop_back();
		pushGui(new GuiMsgBoxScroll(this, title, message, _("OK"), [] {}, "", nullptr, "", nullptr, ALIGN_LEFT));
	}
	if(mNormalizeNextUpdate)
	{
		mNormalizeNextUpdate = false;
		if(deltaTime > mAverageDeltaTime)
			deltaTime = mAverageDeltaTime;
	}

	mFrameTimeElapsed += deltaTime;
	mFrameCountElapsed++;
	if(mFrameTimeElapsed > 500)
	{
		mAverageDeltaTime = mFrameTimeElapsed / mFrameCountElapsed;
		
		if(Settings::getInstance()->getBool("DrawFramerate"))
		{
			std::stringstream ss;
			
			// fps
			ss << std::fixed << std::setprecision(1) << (1000.0f * (float)mFrameCountElapsed / (float)mFrameTimeElapsed) << "fps, ";
			ss << std::fixed << std::setprecision(2) << ((float)mFrameTimeElapsed / (float)mFrameCountElapsed) << "ms";

			// vram
			float textureVramUsageMb = TextureResource::getTotalMemUsage() / (1024.0f * 1024.0f);
			float textureTotalUsageMb = TextureResource::getTotalTextureSize() / (1024.0f * 1024.0f);
			float fontVramUsageMb = Font::getTotalMemUsage() / (1024.0f * 1024.0f);
			ss << "\nFont VRAM: " << fontVramUsageMb << " Tex VRAM: " << textureVramUsageMb <<
				  " Tex Max: " << textureTotalUsageMb;

			mFrameDataText = std::unique_ptr<TextCache>(mDefaultFonts.at(1)->buildTextCache(ss.str(), 50.f, 50.f, 0xFF00FFFF));
		}

		mFrameTimeElapsed = 0;
		mFrameCountElapsed = 0;
	}

	mTimeSinceLastInput += deltaTime;

	if(peekGui())
		peekGui()->update(deltaTime);
}

void Window::render()
{
	Transform4x4f transform = Transform4x4f::Identity();

	mRenderedHelpPrompts = false;

	// draw only bottom and top of GuiStack (if they are different)
	if (!mGuiStack.empty())
	{
		auto& bottom = mGuiStack.front();
		auto& top = mGuiStack.back();

		bottom->render(transform);
		if(bottom != top)
		{
			mBackgroundOverlay->render(transform);
			top->render(transform);
		}
	}

	if(!mRenderedHelpPrompts)
		mHelp->render(transform);

	if(Settings::getInstance()->getBool("DrawFramerate") && mFrameDataText)
	{
		Renderer::setMatrix(Transform4x4f::Identity());
		mDefaultFonts.at(1)->renderTextCache(mFrameDataText.get());
	}

	unsigned int screensaverTime = (unsigned int)Settings::getInstance()->getInt("ScreenSaverTime");
	if(mTimeSinceLastInput >= screensaverTime && screensaverTime != 0)
	{

		if (!isProcessing() && mAllowSleep)
		{
			renderScreenSaver();
			// go to sleep
			mSleeping = true;
			onSleep();
		}
	}
	if(mInfoPopup)
		{
		mInfoPopup->render(transform);
		}
}

void Window::normalizeNextUpdate()
{
	mNormalizeNextUpdate = true;
}

bool Window::getAllowSleep()
{
	return mAllowSleep;
}

void Window::setAllowSleep(bool sleep)
{
	mAllowSleep = sleep;
}

void Window::renderWaitingScreen(const std::string& text)
{
	Transform4x4f trans = Transform4x4f::Identity();
	Renderer::setMatrix(trans);
	Renderer::drawRect(0, 0, (int)Renderer::getScreenWidth(), (int)Renderer::getScreenHeight(), 0xFFFFFFFF);

	ImageComponent splash(this, true);
	splash.setResize((float)Renderer::getScreenWidth() * 0.6f, 0.0f);
	splash.setImage(":/splash.svg");
	splash.setPosition(((float)Renderer::getScreenWidth() - splash.getSize().x()) / 2, ((float)Renderer::getScreenHeight() - splash.getSize().y()) / 2 * 0.6f);
	splash.render(trans);

	auto& font = mDefaultFonts.at(1);
	TextCache* cache = font->buildTextCache(text, 0, 0, 0x656565FF);
	trans = trans.translate(Vector3f(round(((float)Renderer::getScreenWidth() - cache->metrics.size.x()) / 2.0f),
											round((float)Renderer::getScreenHeight() * 0.835f), 0.0f));
	Renderer::setMatrix(trans);
	font->renderTextCache(cache);
	delete cache;

	Renderer::swapBuffers();
}
void Window::renderLoadingScreen()
{
  renderWaitingScreen(_("LOADING..."));
}

void Window::renderHelpPromptsEarly()
{
	mHelp->render(Transform4x4f::Identity());
	mRenderedHelpPrompts = true;
}

void Window::setHelpPrompts(const std::vector<HelpPrompt>& prompts, const HelpStyle& style)
{
	mHelp->clearPrompts();
	mHelp->setStyle(style);

	std::vector<HelpPrompt> addPrompts;

	std::map<std::string, bool> inputSeenMap;
	std::map<std::string, int> mappedToSeenMap;
	for (const auto& prompt : prompts)
	{
		// only add it if the same icon hasn't already been added
	  if (inputSeenMap.insert(std::make_pair<std::string, bool>(prompt.first.c_str(), true)).second)
		{
			// this symbol hasn't been seen yet, what about the action name?
			auto mappedTo = mappedToSeenMap.find(prompt.second);
			if(mappedTo != mappedToSeenMap.end())
			{
				// yes, it has!

				// can we combine? (dpad only)
			  if((strcmp(prompt.first.c_str(), "up/down") == 0 && strcmp(addPrompts.at(mappedTo->second).first.c_str(), "left/right") == 0) ||
			     (strcmp(prompt.first.c_str(), "left/right") == 0 && strcmp(addPrompts.at(mappedTo->second).first.c_str(), "up/down") == 0))
				{
					// yes!
				  addPrompts.at(mappedTo->second).first = "up/down/left/right";
					// don't need to add this to addPrompts since we just merged
				}else{
					// no, we can't combine!
					addPrompts.push_back(prompt);
				}
			}else{
				// no, it hasn't!
				mappedToSeenMap.insert(std::pair<std::string, int>(prompt.second, addPrompts.size()));
				addPrompts.push_back(prompt);
			}
		}
	}

	// sort prompts so it goes [dpad_all] [dpad_u/d] [dpad_l/r] [a/b/x/y/l/r] [start/select]
	std::sort(addPrompts.begin(), addPrompts.end(), [](const HelpPrompt& a, const HelpPrompt& b) -> bool {
		
		static const char* map[] = {
			"up/down/left/right",
			"up/down",
			"left/right",
			"a", "b", "x", "y", "l", "r", 
			"start", "select", 
			nullptr
		};
		
		int i = 0;
		int aVal = 0;
		int bVal = 0;
		while(map[i] != nullptr)
		{
			if(a.first == map[i])
				aVal = i;
			if(b.first == map[i])
				bVal = i;
			i++;
		}

		return aVal > bVal;
	});

	mHelp->setPrompts(addPrompts);
}


void Window::onSleep()
{
}


void Window::onWake()
{
}

void Window::renderShutdownScreen()
{
  renderWaitingScreen(_("PLEASE WAIT..."));
}

bool Window::isProcessing()
{
	return count_if(mGuiStack.begin(), mGuiStack.end(), [](GuiComponent* c) { return c->isProcessing(); }) > 0;
}

void Window::renderScreenSaver()
{
	Renderer::setMatrix(Transform4x4f::Identity());
	unsigned char opacity = Settings::getInstance()->getString("ScreenSaverBehavior") == "dim" ? 0xA0 : 0xFF;
	Renderer::drawRect(0, 0, (int)Renderer::getScreenWidth(), (int)Renderer::getScreenHeight(), 0x00000000 | opacity);
}

void Window::doWake()
{
	mTimeSinceLastInput = 0;
	mSleeping = false;
	onWake();
}

bool Window::KonamiCode(InputConfig* config, Input input, Window* window)
{
  (void)window;

	if (!input.value)
		return false;

	bool codeOk = false;

	for (auto valstring : mInputVals)
	{
		if (config->isMappedTo(valstring, input) && (this->mKonami[this->mKonamiCount] == valstring[0]))
		{
			this->mKonamiCount ++;
			codeOk = true;
		}
	}

	if (!codeOk)
	{
		this->mKonamiCount = 0; // current input is incorrect, reset counter
	}

	if (this->mKonamiCount == (int)(this->mKonami.length()))
	{
		auto s = std::make_shared<GuiInfoPopup>(this, "I entered Konami Code and all I get is this lame popup", 4, 50);
		this->setInfoPopup(s);
		return true;
	}
	return false;
}
