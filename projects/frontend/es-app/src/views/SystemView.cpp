#include "views/SystemView.h"
#include "SystemData.h"
#include "Renderer.h"
#include "Log.h"
#include "Window.h"
#include "views/ViewController.h"
#include "animations/LambdaAnimation.h"
#include "SystemData.h"
#include "Settings.h"
#include "Util.h"
#include <guis/GuiMsgBox.h>
#include <recalbox/RecalboxSystem.h>
#include <components/ComponentList.h>
#include <guis/GuiSettings.h>
#include <RecalboxConf.h>
#include <guis/GuiNetPlay.h>
#include <guis/GuiLoading.h>
#include "guis/GuiMenu.h"
#include <NetPlayThread.h>
#include "ThemeData.h"
#include "MenuThemeData.h"
#include "AudioManager.h"
#include "Locale.h"

// buffer values for scrolling velocity (left, stopped, right)
const int logoBuffersLeft[] = { -5, -2, -1 };
const int logoBuffersRight[] = { 1, 2, 5 };

SystemView::SystemView(Window* window)
  : IList<SystemViewData, SystemData*>(window, LIST_SCROLL_STYLE_SLOW, LIST_ALWAYS_LOOP),
		mSystemInfo(window, "SYSTEM INFO", Font::get(FONT_SIZE_SMALL), 0x33333300, ALIGN_CENTER),
		mViewNeedsReload(true),
		launchKodi(false)
{
	mCamOffset = 0;
	mExtrasCamOffset = 0;
	mExtrasFadeOpacity = 0.0f;

	setSize((float)Renderer::getScreenWidth(), (float)Renderer::getScreenHeight());


	populate();
}

void SystemView::addSystem(SystemData * it){
	if(!(it)->getRootFolder()->hasChildren()){
		return;
	}
	const std::shared_ptr<ThemeData>& theme = (it)->getTheme();
	
	if(mViewNeedsReload)
			getViewElements(theme);

	Entry e;
	e.name = (it)->getName();
	e.object = it;

	// make logo
	if(theme->getElement("system", "logo", "image"))
	{
		ImageComponent* logo = new ImageComponent(mWindow, false, false);
		logo->setMaxSize(mCarousel.logoSize * mCarousel.logoScale);
		logo->applyTheme((it)->getTheme(), "system", "logo", ThemeFlags::PATH);
		e.data.logo = std::shared_ptr<GuiComponent>(logo);
		if ((it)->getThemeFolder() == "default")
		{
			TextComponent* text = new TextComponent(mWindow,
			                                        (it)->getName(),
			                                        Font::get(FONT_SIZE_MEDIUM),
			                                        0x1A1A1AFF,
			                                        ALIGN_CENTER);
			text->setSize(mCarousel.logoSize * mCarousel.logoScale);
			e.data.logotext = std::shared_ptr<GuiComponent>(text);
			if (mCarousel.type == VERTICAL || mCarousel.type == VERTICAL_WHEEL)
				text->setHorizontalAlignment(mCarousel.logoAlignment);
			else
				text->setVerticalAlignment(mCarousel.logoAlignment);
		}

	}else{
		// no logo in theme; use text
		TextComponent* text = new TextComponent(mWindow,
												(it)->getName(),
												Font::get(FONT_SIZE_LARGE),
												0x000000FF,
												ALIGN_CENTER);
		text->setSize(mCarousel.logoSize * mCarousel.logoScale);
		e.data.logo = std::shared_ptr<GuiComponent>(text);
		if (mCarousel.type == VERTICAL || mCarousel.type == VERTICAL_WHEEL)
			text->setHorizontalAlignment(mCarousel.logoAlignment);
		else
			text->setVerticalAlignment(mCarousel.logoAlignment);
	}

	if (mCarousel.type == VERTICAL || mCarousel.type == VERTICAL_WHEEL)
	{
		if (mCarousel.logoAlignment == ALIGN_LEFT)
			e.data.logo->setOrigin(0, 0.5);
		else if (mCarousel.logoAlignment == ALIGN_RIGHT)
			e.data.logo->setOrigin(1.0, 0.5);
		else
			e.data.logo->setOrigin(0.5, 0.5);
	} else {
		if (mCarousel.logoAlignment == ALIGN_TOP)
			e.data.logo->setOrigin(0.5, 0);
		else if (mCarousel.logoAlignment == ALIGN_BOTTOM)
			e.data.logo->setOrigin(0.5, 1);
		else
			e.data.logo->setOrigin(0.5, 0.5);
	}

	Vector2f denormalized = mCarousel.logoSize * e.data.logo->getOrigin();
	e.data.logo->setPosition(denormalized.x(), denormalized.y(), 0.0);

	if (e.data.logotext)
	{
		if (mCarousel.type == VERTICAL || mCarousel.type == VERTICAL_WHEEL)
		{
			if (mCarousel.logoAlignment == ALIGN_LEFT)
				e.data.logotext->setOrigin(0, 0.5);
			else if (mCarousel.logoAlignment == ALIGN_RIGHT)
				e.data.logotext->setOrigin(1.0, 0.5);
			else
				e.data.logotext->setOrigin(0.5, 0.5);
		} else {
			if (mCarousel.logoAlignment == ALIGN_TOP)
				e.data.logotext->setOrigin(0.5, 0);
			else if (mCarousel.logoAlignment == ALIGN_BOTTOM)
				e.data.logotext->setOrigin(0.5, 1);
			else
				e.data.logotext->setOrigin(0.5, 0.5);
		}

		Vector2f denormalized = mCarousel.logoSize * e.data.logotext->getOrigin();
		e.data.logotext->setPosition(denormalized.x(), denormalized.y(), 0.0);
	}

	e.data.backgroundExtras = std::shared_ptr<ThemeExtras>(new ThemeExtras(mWindow));
	e.data.backgroundExtras->setExtras(ThemeData::makeExtras((it)->getTheme(), "system", mWindow));


	// sort the extras by z-index
	e.data.backgroundExtras->sortExtrasByZIndex();

	this->add(e);
}
void SystemView::populate()
{
	mEntries.clear();

	for (auto& it : SystemData::sSystemVector)
	{
		addSystem(it);
	}
}

void SystemView::goToSystem(SystemData* system, bool animate)
{
	if (!setCursor(system)) // When deleting last favorite from favorite view, favorite system is no longer available
	  setCursor(SystemData::getFirstSystemWithGame());

	if(!animate)
		finishAnimation(0);
	onCursorChanged(CURSOR_STOPPED);
}

bool SystemView::input(InputConfig* config, Input input)
{
	if(input.value != 0)
	{
		if(config->getDeviceId() == DEVICE_KEYBOARD && input.id == SDLK_r && SDL_GetModState() & KMOD_LCTRL && Settings::getInstance()->getBool("Debug"))
		{
			LOG(LogInfo) << " Reloading all";
			ViewController::get()->reloadAll();
			return true;
		}
		switch (mCarousel.type)
		{
		case VERTICAL:
		case VERTICAL_WHEEL:
			if (config->isMappedTo("up", input))
			{
				listInput(-1);
				return true;
			}
			if (config->isMappedTo("down", input))
			{
				listInput(1);
				return true;
			}
			break;
		case HORIZONTAL:
		default:
			if (config->isMappedTo("left", input))
			{
				listInput(-1);
				return true;
			}
			if (config->isMappedTo("right", input))
			{
				listInput(1);
				return true;
			}
			break;	
		}
		if (config->isMappedTo("b", input))
		{
			stopScrolling();
			ViewController::get()->goToGameList(getSelected());
			return true;
		}
        if (config->isMappedTo("x", input))
        {
		    bool kodiEnabled = RecalboxConf::getInstance()->get("kodi.enabled") == "1";
		    bool kodiX = RecalboxConf::getInstance()->get("kodi.xbutton") == "1";
		    bool netplay = RecalboxConf::getInstance()->get("global.netplay") == "1";

		    if (kodiEnabled && kodiX && !launchKodi && !mWindow->isShowingPopup()) {
		        if (netplay) {
                    auto s = new GuiSettings(mWindow, _("KODI/NETPLAY").c_str());
                    auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
                    ComponentListRow row;
                    row.makeAcceptInputHandler([this, s] {
                        launchKodi = true;
                        if( ! RecalboxSystem::getInstance()->launchKodi(mWindow)) {
                            LOG(LogWarning) << "Shutdown terminated with non-zero result!";
                        }
                        launchKodi = false;
                        delete s;
                    });
                    auto lbl = std::make_shared<TextComponent>(mWindow, "\uF1c3 " + _("KODI MEDIA CENTER"), menuTheme->menuText.font, menuTheme->menuText.color);
                    row.addElement(lbl, true); // label
                    s->addRow(row);
                    row.elements.clear();
                    row.makeAcceptInputHandler([this, s] {
                        auto netplay = new GuiNetPlay(mWindow);
                        mWindow->pushGui(netplay);
                        delete s;
                    });
                    auto lbl2 = std::make_shared<TextComponent>(mWindow, "\uF1c4 " + _("NETPLAY LOBBY"), menuTheme->menuText.font, menuTheme->menuText.color);
                    row.addElement(lbl2, true); // label
                    s->addRow(row);
                    mWindow->pushGui(s);
                } else {
                    launchKodi = true;
                    if( ! RecalboxSystem::getInstance()->launchKodi(mWindow)) {
                        LOG(LogWarning) << "Shutdown terminated with non-zero result!";
                    }
                    launchKodi = false;
		        }
		    } else if (netplay && !mWindow->isShowingPopup()) {
                auto netplay = new GuiNetPlay(mWindow);
                mWindow->pushGui(netplay);
		    }


        }
		if (config->isMappedTo("select", input) && RecalboxConf::getInstance()->get("emulationstation.menu") != "none")
		{
		  auto s = new GuiSettings(mWindow, _("QUIT").c_str());
			auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
			Window *window = mWindow;
			ComponentListRow row;



			row.makeAcceptInputHandler([window] {
			    window->pushGui(new GuiMsgBox(window, _("REALLY SHUTDOWN?"), _("YES"),
											  [] {
												  if (RecalboxSystem::getInstance()->shutdown() != 0)  {
													  LOG(LogWarning) <<
																	  "Shutdown terminated with non-zero result!";
												  }
							  }, _("NO"), nullptr));
			});
			auto icon2 = std::make_shared<ImageComponent>(mWindow);
			icon2->setImage(menuTheme->menuIconSet.shutdown);
			icon2->setColorShift(menuTheme->menuText.color);
			icon2->setResize(0, menuTheme->menuText.font->getLetterHeight() * 1.25f);
			row.addElement(icon2, false);
			// spacer between icon and text
			auto spacer = std::make_shared<GuiComponent>(mWindow);
			row.addElement(spacer, false);
			row.addElement(std::make_shared<TextComponent>(window, _("SHUTDOWN SYSTEM"), menuTheme->menuText.font, menuTheme->menuText.color), true);
			s->addRow(row);
			row.elements.clear();
			row.makeAcceptInputHandler([window] {
				window->pushGui(new GuiMsgBox(window, _("REALLY SHUTDOWN WITHOUT SAVING METADATAS?"), _("YES"),
											  [] {
												  if (RecalboxSystem::getInstance()->fastShutdown() != 0)  {
													  LOG(LogWarning) <<
																	  "Shutdown terminated with non-zero result!";
												  }
											  }, _("NO"), nullptr));
			});
			auto icon3 = std::make_shared<ImageComponent>(mWindow);
			icon3->setImage(menuTheme->menuIconSet.fastshutdown);
			icon3->setColorShift(menuTheme->menuText.color);
			icon3->setResize(0, menuTheme->menuText.font->getLetterHeight() * 1.25f);
			row.addElement(icon3, false);
			// spacer between icon and text
			row.addElement(spacer, false);
			row.addElement(std::make_shared<TextComponent>(window, _("FAST SHUTDOWN SYSTEM"), menuTheme->menuText.font, menuTheme->menuText.color), true);
			s->addRow(row);
			row.elements.clear();
			row.makeAcceptInputHandler([window] {
				window->pushGui(new GuiMsgBox(window, _("REALLY RESTART?"), _("YES"),
				                              [] {
					                              if (RecalboxSystem::getInstance()->reboot() != 0)  {
						                              LOG(LogWarning) << "Restart terminated with non-zero result!";
					                              }
				                              }, _("NO"), nullptr));
			});
			// icon
			auto icon1 = std::make_shared<ImageComponent>(mWindow);
			icon1->setImage(menuTheme->menuIconSet.restart);
			icon1->setColorShift(menuTheme->menuText.color);
			icon1->setResize(0, menuTheme->menuText.font->getLetterHeight() * 1.25f);
			row.addElement(icon1, false);


			spacer->setSize(10, 0);
			row.addElement(spacer, false);
			row.addElement(std::make_shared<TextComponent>(window, _("RESTART SYSTEM"), menuTheme->menuText.font, menuTheme->menuText.color), true);
			s->addRow(row);
			mWindow->pushGui(s);
		}

		if (config->isMappedTo("start", input) && RecalboxConf::getInstance()->get("emulationstation.menu") != "none")
		{
			mWindow->pushGui(new GuiMenu(mWindow));
			return true;
		}

	} else {
		if (config->isMappedTo("left", input) ||
				config->isMappedTo("right", input) ||
				config->isMappedTo("up", input) || 
				config->isMappedTo("down", input))
				listInput(0);
	}

	return GuiComponent::input(config, input);
}

void SystemView::update(int deltaTime)
{
	listUpdate(deltaTime);
	GuiComponent::update(deltaTime);
}

void SystemView::onCursorChanged(const CursorState& state)
{
  (void)state;

	if(lastSystem != getSelected()){
		lastSystem = getSelected();
		AudioManager::getInstance()->themeChanged(getSelected()->getTheme());
	}
	// update help style
	updateHelpPrompts();

	// update externs
  RecalboxSystem::getInstance()->NotifySystem(*getSelected());

	float startPos = mCamOffset;

	float posMax = (float)mEntries.size();
	float target = (float)mCursor;

	// what's the shortest way to get to our target?
	// it's one of these...

	float endPos = target; // directly
    float dist = std::abs(endPos - startPos);
	
    if(std::abs(target + posMax - startPos) < dist)
		endPos = target + posMax; // loop around the end (0 -> max)
    if(std::abs(target - posMax - startPos) < dist)
		endPos = target - posMax; // loop around the start (max - 1 -> -1)

	
	// animate mSystemInfo's opacity (fade out, wait, fade back in)

	cancelAnimation(1);
	cancelAnimation(2);

	const float infoStartOpacity = mSystemInfo.getOpacity() / 255.f;

	Animation* infoFadeOut = new LambdaAnimation(
		[infoStartOpacity, this] (float t)
	{
		mSystemInfo.setOpacity((unsigned char)(lerp<float>(infoStartOpacity, 0.f, t) * 255));
	}, (int)(infoStartOpacity * 150));

	unsigned int gameCount = getSelected()->getGameCount();
	unsigned int favoritesCount = getSelected()->getFavoritesCount();
	unsigned int hiddenCount = getSelected()->getHiddenCount();
	unsigned int gameNoHiddenCount = gameCount - hiddenCount;

	// also change the text after we've fully faded out
	setAnimation(infoFadeOut, 0, [this, gameCount, favoritesCount, gameNoHiddenCount, hiddenCount] {
		char strbuf[256];
		if(favoritesCount == 0 && hiddenCount == 0) {
			snprintf(strbuf, 256, ngettext("%i GAME AVAILABLE", "%i GAMES AVAILABLE", gameNoHiddenCount).c_str(), gameNoHiddenCount);
		}else if (favoritesCount != 0 && hiddenCount == 0) {
			snprintf(strbuf, 256,
				(ngettext("%i GAME AVAILABLE", "%i GAMES AVAILABLE", gameNoHiddenCount) + ", " +
				 ngettext("%i FAVORITE", "%i FAVORITES", favoritesCount)).c_str(), gameNoHiddenCount, favoritesCount);
		}else if (favoritesCount == 0 && hiddenCount != 0) {
			snprintf(strbuf, 256,
				(ngettext("%i GAME AVAILABLE", "%i GAMES AVAILABLE", gameNoHiddenCount) + ", " +
				 ngettext("%i GAME HIDDEN", "%i GAMES HIDDEN", hiddenCount)).c_str(), gameNoHiddenCount, hiddenCount);
		}else {
			snprintf(strbuf, 256,
				(ngettext("%i GAME AVAILABLE", "%i GAMES AVAILABLE", gameNoHiddenCount) + ", " +
				 ngettext("%i GAME HIDDEN", "%i GAMES HIDDEN", hiddenCount) + ", " +
				 ngettext("%i FAVORITE", "%i FAVORITES", favoritesCount)).c_str(), gameNoHiddenCount, hiddenCount, favoritesCount);
		}
		mSystemInfo.setText(strbuf);
	}, false, 1);

	Animation* infoFadeIn = new LambdaAnimation(
		[this](float t)
	{
		mSystemInfo.setOpacity((unsigned char)(lerp<float>(0.f, 1.f, t) * 255));
	}, 300);

	// wait ms to fade in
	setAnimation(infoFadeIn, 800, nullptr, false, 2);

	// no need to animate transition, we're not going anywhere (probably mEntries.size() == 1)
	if(endPos == mCamOffset && endPos == mExtrasCamOffset)
		return;

	Animation* anim;
    bool move_carousel = Settings::getInstance()->getBool("MoveCarousel");
	std::string transition_style = Settings::getInstance()->getString("TransitionStyle");
	if(transition_style == "fade")
	{
		float startExtrasFade = mExtrasFadeOpacity;
		anim = new LambdaAnimation(
			[startExtrasFade, startPos, endPos, posMax, this, move_carousel](float t)
		{
			t -= 1;
			float f = lerp<float>(startPos, endPos, t*t*t + 1);
			if(f < 0)
				f += posMax;
			if(f >= posMax)
				f -= posMax;

            this->mCamOffset = move_carousel ? f : endPos;

			t += 1;
			if(t < 0.3f)
				this->mExtrasFadeOpacity = lerp<float>(0.0f, 1.0f, t / 0.3f + startExtrasFade);
			else if(t < 0.7f)
				this->mExtrasFadeOpacity = 1.0f;
			else
				this->mExtrasFadeOpacity = lerp<float>(1.0f, 0.0f, (t - 0.7f) / 0.3f);

			if(t > 0.5f)
				this->mExtrasCamOffset = endPos;

		}, 500);
	}
	else if (transition_style == "slide"){ // slide
		anim = new LambdaAnimation(
			[startPos, endPos, posMax, this, move_carousel](float t)
		{
			t -= 1;
			float f = lerp<float>(startPos, endPos, t*t*t + 1);
			if(f < 0)
				f += posMax;
			if(f >= posMax)
				f -= posMax;

            this->mCamOffset = move_carousel ? f : endPos;
			this->mExtrasCamOffset = f;
		}, 500);

	} else {
	// instant
		anim = new LambdaAnimation(
		[this, startPos, endPos, posMax, move_carousel](float t)
		{
            t -= 1;
            float f = lerp<float>(startPos, endPos, t*t*t + 1);
            if(f < 0)
                f += posMax;
            if(f >= posMax)
                f -= posMax;
            this->mCamOffset = move_carousel ? f : endPos;
		    this->mExtrasCamOffset = endPos;
		}, move_carousel ? 500 : 1);
	}

	setAnimation(anim, 0, nullptr, false, 0);


}

void SystemView::render(const Transform4x4f& parentTrans)
{
	if(size() == 0)
		return;  // nothing to render
	
	// draw the list elements (titles, backgrounds, logos)
	Transform4x4f trans = getTransform() * parentTrans;

	auto systemInfoZIndex = mSystemInfo.getZIndex();
	auto minMax = std::minmax(mCarousel.zIndex, systemInfoZIndex);
	
	renderExtras(trans, INT16_MIN, minMax.first);
	renderFade(trans);
	
	if (mCarousel.zIndex > mSystemInfo.getZIndex()) {
		renderInfoBar(trans);
	} else {
		renderCarousel(trans);
	}
	
	renderExtras(trans, minMax.first, minMax.second);
	
	if (mCarousel.zIndex > mSystemInfo.getZIndex()) {
		renderCarousel(trans);
	} else {
		renderInfoBar(trans);
	}
	renderExtras(trans, minMax.second, INT16_MAX);
}

std::vector<HelpPrompt> SystemView::getHelpPrompts()
{
	std::vector<HelpPrompt> prompts;
	if (mCarousel.type == VERTICAL)
			prompts.push_back(HelpPrompt("up/down", _("CHOOSE")));
	else
			prompts.push_back(HelpPrompt("left/right", _("CHOOSE")));
	prompts.push_back(HelpPrompt("b", _("SELECT")));

	if (RecalboxConf::getInstance()->get("kodi.enabled") == "1" && RecalboxConf::getInstance()->get("kodi.xbutton") == "1")
	{
	    if (RecalboxConf::getInstance()->get("global.netplay") == "1")
	        prompts.push_back(HelpPrompt("x", _("KODI/NETPLAY")));
	    else
            prompts.push_back(HelpPrompt("x", _("START KODI")));

	} else if (RecalboxConf::getInstance()->get("global.netplay") == "1")
	    prompts.push_back(HelpPrompt("x", _("NETPLAY")));

	prompts.push_back(HelpPrompt("select", _("QUIT")));
	prompts.push_back(HelpPrompt("start", _("MENU")));

	return prompts;
}	

HelpStyle SystemView::getHelpStyle()
{
	HelpStyle style;
	style.applyTheme(mEntries.at(mCursor).object->getTheme(), "system");
	return style;
	}	

void  SystemView::onThemeChanged(const std::shared_ptr<ThemeData>& theme)
{
	(void)theme; // TODO: Log theme name
	LOG(LogDebug) << "SystemView::onThemeChanged()";
	mViewNeedsReload = true;
	populate();
}	

//  Get the ThemeElements that make up the SystemView.
void  SystemView::getViewElements(const std::shared_ptr<ThemeData>& theme)
{
		LOG(LogDebug) << "SystemView::getViewElements()";
		getDefaultElements();
		
		const ThemeData::ThemeElement* carouselElem = theme->getElement("system", "systemcarousel", "carousel");
		if (carouselElem)
			getCarouselFromTheme(carouselElem);
		
		const ThemeData::ThemeElement* sysInfoElem = theme->getElement("system", "systemInfo", "text");
		if (sysInfoElem)
			mSystemInfo.applyTheme(theme, "system", "systemInfo", ThemeFlags::ALL);
		
		mViewNeedsReload = false;
		}
		
//  Render system carousel
void SystemView::renderCarousel(const Transform4x4f& trans)
{
	// background box behind logos
	Transform4x4f carouselTrans = trans;
	carouselTrans.translate(Vector3f(mCarousel.pos.x(), mCarousel.pos.y(), 0.0));
	carouselTrans.translate(Vector3f(mCarousel.origin.x() * mCarousel.size.x() * -1, mCarousel.origin.y() * mCarousel.size.y() * -1, 0.0f));

	Vector2f clipPos(carouselTrans.translation().x(), carouselTrans.translation().y());
	Renderer::pushClipRect(clipPos.toInt(), mCarousel.size.toInt());

	Renderer::setMatrix(carouselTrans);
	Renderer::drawRect(0.0, 0.0, mCarousel.size.x(), mCarousel.size.y(), mCarousel.color);
	
	// draw logos
	Vector2f logoSpacing(0.0, 0.0); // NB: logoSpacing will include the size of the logo itself as well!
	float xOff = 0.0;
	float yOff = 0.0;
	
	switch (mCarousel.type)
	{
		case VERTICAL_WHEEL:
			yOff = (mCarousel.size.y() - mCarousel.logoSize.y()) / 2 - (mCamOffset * logoSpacing[1]);
			if (mCarousel.logoAlignment == ALIGN_LEFT)
				xOff = mCarousel.logoSize.x() / 10;
			else if (mCarousel.logoAlignment == ALIGN_RIGHT)
				xOff = mCarousel.size.x() - (mCarousel.logoSize.x() * 1.1);
			else
				xOff = (mCarousel.size.x() - mCarousel.logoSize.x()) / 2;
			break;
		case VERTICAL:
			logoSpacing[1] = ((mCarousel.size.y() - (mCarousel.logoSize.y() * mCarousel.maxLogoCount)) / (mCarousel.maxLogoCount)) + mCarousel.logoSize.y();
			yOff = (mCarousel.size.y() - mCarousel.logoSize.y()) / 2 - (mCamOffset * logoSpacing[1]);

			if (mCarousel.logoAlignment == ALIGN_LEFT)
				xOff = mCarousel.logoSize.x() / 10;
			else if (mCarousel.logoAlignment == ALIGN_RIGHT)
				xOff = mCarousel.size.x() - (mCarousel.logoSize.x() * 1.1);
			else
				xOff = (mCarousel.size.x() - mCarousel.logoSize.x()) / 2;
			break;
		case HORIZONTAL:
		default:
			logoSpacing[0] = ((mCarousel.size.x() - (mCarousel.logoSize.x() * mCarousel.maxLogoCount)) / (mCarousel.maxLogoCount)) + mCarousel.logoSize.x();
			xOff = (mCarousel.size.x() - mCarousel.logoSize.x()) / 2 - (mCamOffset * logoSpacing[0]);

			if (mCarousel.logoAlignment == ALIGN_TOP)
				yOff = mCarousel.logoSize.y() / 10;
			else if (mCarousel.logoAlignment == ALIGN_BOTTOM)
				yOff = mCarousel.size.y() - (mCarousel.logoSize.y() * 1.1);
			else
				yOff = (mCarousel.size.y() - mCarousel.logoSize.y()) / 2;
			break;
	}

	int center = (int)(mCamOffset);
	int logoCount = std::min(mCarousel.maxLogoCount, (int)mEntries.size());
	
	// Adding texture loading buffers depending on scrolling speed and status
	int bufferIndex = getScrollingVelocity() + 1;

	int bufferLeft = logoBuffersLeft[bufferIndex];
	int bufferRight = logoBuffersRight[bufferIndex];
	if (logoCount == 1)
	{
		bufferLeft = 0;
		bufferRight = 0;
	}

	for (int i = center - logoCount / 2 + bufferLeft; i <= center + logoCount / 2 + bufferRight; i++)
	{
		int index = i;
		while (index < 0)
			index += mEntries.size();
		while (index >= (int)mEntries.size())
			index -= mEntries.size();

		Transform4x4f logoTrans = carouselTrans;
		logoTrans.translate(Vector3f(i * logoSpacing[0] + xOff, i * logoSpacing[1] + yOff, 0));

		float distance = i - mCamOffset;

		float scale = 1.0 + ((mCarousel.logoScale - 1.0) * (1 - fabs(distance)));
		scale = std::min(mCarousel.logoScale, std::max(1.0f, scale));
		scale /= mCarousel.logoScale;

		int opacity = round(0x80 + ((0xFF - 0x80) * (1 - fabs(distance))));
		opacity = std::max((int) 0x80, opacity);

		const std::shared_ptr<GuiComponent> &comp = mEntries.at(index).data.logo;
		if (mCarousel.type == VERTICAL_WHEEL) {
			comp->setRotationDegrees(mCarousel.logoRotation * distance);
			comp->setRotationOrigin(mCarousel.logoRotationOrigin);
		}
		comp->setScale(scale);
		comp->setOpacity(opacity);
		comp->render(logoTrans);

		if (mEntries.at(index).data.logotext)
		{
			const std::shared_ptr<GuiComponent> &comp2 = mEntries.at(index).data.logotext;
			if (mCarousel.type == VERTICAL_WHEEL) {
				comp2->setRotationDegrees(mCarousel.logoRotation * distance);
				comp2->setRotationOrigin(mCarousel.logoRotationOrigin);
			}
			comp2->setScale(scale);
			comp2->setOpacity(opacity);
			comp2->render(logoTrans);
		}
	}
	Renderer::popClipRect();
}

void SystemView::renderInfoBar(const Transform4x4f& trans)
{
	Renderer::setMatrix(trans);
	mSystemInfo.render(trans);
}


// Draw background extras
void SystemView::renderExtras(const Transform4x4f& trans, float lower, float upper)
{	
	int extrasCenter = (int)mExtrasCamOffset;

	Renderer::pushClipRect(Vector2i::Zero(), mSize.toInt());
	
	// Adding texture loading buffers depending on scrolling speed and status
	int bufferIndex = getScrollingVelocity() + 1;
	
	for (int i = extrasCenter + logoBuffersLeft[bufferIndex]; i <= extrasCenter + logoBuffersRight[bufferIndex]; i++)
	{
		int index = i;
		while (index < 0)
			index += mEntries.size();
		while (index >= (int)mEntries.size())
			index -= mEntries.size();

		//Only render selected system when not showing
		if (mShowing || index == mCursor)
		{
			Transform4x4f extrasTrans = trans;
			if (mCarousel.type == HORIZONTAL)
				extrasTrans.translate(Vector3f((i - mExtrasCamOffset) * mSize.x(), 0, 0));
			else
				extrasTrans.translate(Vector3f(0, (i - mExtrasCamOffset) * mSize.y(), 0));

			Renderer::pushClipRect(Vector2i(extrasTrans.translation()[0], extrasTrans.translation()[1]),
								   mSize.toInt());
			SystemViewData data = mEntries.at(index).data;
			for (unsigned int j = 0; j < data.backgroundExtras->getmExtras().size(); j++) {
				GuiComponent *extra = data.backgroundExtras->getmExtras()[j];
				if (extra->getZIndex() >= lower && extra->getZIndex() < upper) {
					extra->render(extrasTrans);
				}
			}
			Renderer::popClipRect();
		}
	}
	Renderer::popClipRect();
}

void SystemView::renderFade(const Transform4x4f& trans)
{
	// fade extras if necessary
	if (mExtrasFadeOpacity)
	{
			Renderer::setMatrix(trans);
			Renderer::drawRect(0.0f, 0.0f, mSize.x(), mSize.y(), 0x00000000 | (unsigned char)(mExtrasFadeOpacity * 255));
	}
}

// Populate the system carousel with the legacy values
void  SystemView::getDefaultElements(void)
{
	// Carousel
	mCarousel.type = HORIZONTAL;
	mCarousel.logoAlignment = ALIGN_CENTER;
	mCarousel.size.x() = mSize.x();
	mCarousel.size.y() = 0.2325f * mSize.y();
	mCarousel.pos.x() = 0.0f;
	mCarousel.pos.y() = 0.5f * (mSize.y() - mCarousel.size.y());
	mCarousel.origin.x() = 0.0f;
	mCarousel.origin.y() = 0.0f;
	mCarousel.color = 0xFFFFFFD8;
	mCarousel.logoScale = 1.2f;
	mCarousel.logoRotation = 7.5;
	mCarousel.logoRotationOrigin.x() = -5;
	mCarousel.logoRotationOrigin.y() = 0.5;
	mCarousel.logoSize.x() = 0.25f * (std::max(mSize.y(), mSize.x()));
	mCarousel.logoSize.y() = 0.155f * (std::min(mSize.y(), mSize.x()));
	mCarousel.maxLogoCount = 3;
	mCarousel.zIndex = 40;

	// System Info Bar
	mSystemInfo.setSize(mSize.x(), mSystemInfo.getFont()->getLetterHeight()*2.2f);
	mSystemInfo.setPosition(0, (mCarousel.pos.y() + mCarousel.size.y() - 0.2f));
	mSystemInfo.setBackgroundColor(0xDDDDDDD8);
	mSystemInfo.setRenderBackground(true);
	mSystemInfo.setFont(Font::get((int)(0.035f * (std::min(mSize.y(), mSize.x()))), Font::getDefaultPath()));
	mSystemInfo.setHorizontalAlignment(ALIGN_CENTER);
	mSystemInfo.setColor(0x000000FF);
	mSystemInfo.setZIndex(50);
	mSystemInfo.setDefaultZIndex(50);
}

void SystemView::getCarouselFromTheme(const ThemeData::ThemeElement* elem)
{
	if (elem->has("type"))
	{
		if (!(elem->get<std::string>("type").compare("vertical")))
			mCarousel.type = VERTICAL;
		else if (!(elem->get<std::string>("type").compare("vertical_wheel")))
			mCarousel.type = VERTICAL_WHEEL;
		else
			mCarousel.type = HORIZONTAL;
	}
	if (elem->has("size"))
		mCarousel.size = elem->get<Vector2f>("size") * mSize;
	if (elem->has("pos"))
		mCarousel.pos = elem->get<Vector2f>("pos") * mSize;
	if (elem->has("origin"))
		mCarousel.origin = elem->get<Vector2f>("origin");
	if (elem->has("color"))
		mCarousel.color = elem->get<unsigned int>("color");
	if (elem->has("logoScale"))
		mCarousel.logoScale = elem->get<float>("logoScale");
	if (elem->has("logoSize"))
		mCarousel.logoSize = elem->get<Vector2f>("logoSize") * mSize;
	if (elem->has("maxLogoCount"))
		mCarousel.maxLogoCount = std::round(elem->get<float>("maxLogoCount"));
	if (elem->has("zIndex"))
		mCarousel.zIndex = elem->get<float>("zIndex");
	if (elem->has("logoRotation"))
		mCarousel.logoRotation = elem->get<float>("logoRotation");
	if (elem->has("logoRotationOrigin"))
		mCarousel.logoRotationOrigin = elem->get<Vector2f>("logoRotationOrigin");
	if (elem->has("logoAlignment"))
	{
		if (!(elem->get<std::string>("logoAlignment").compare("left")))
			mCarousel.logoAlignment = ALIGN_LEFT;
		else if (!(elem->get<std::string>("logoAlignment").compare("right")))
			mCarousel.logoAlignment = ALIGN_RIGHT;
		else if (!(elem->get<std::string>("logoAlignment").compare("top")))
			mCarousel.logoAlignment = ALIGN_TOP;
		else if (!(elem->get<std::string>("logoAlignment").compare("bottom")))
			mCarousel.logoAlignment = ALIGN_BOTTOM;
		else
			mCarousel.logoAlignment = ALIGN_CENTER;
	}
}

void SystemView::onShow()
{
	mShowing = true;
}

void SystemView::onHide()
{
	mShowing = false;
}

void SystemView::removeFavoriteSystem(){
	for (auto it = mEntries.begin(); it != mEntries.end(); it++)
		if(it->object->isFavorite()){
			mEntries.erase(it);
			break;
		}
}

void SystemView::manageFavorite(){
	bool hasFavorite = false;
	for (auto& mEntrie : mEntries)
		if(mEntrie.object->isFavorite()){
			hasFavorite = true;
			break;
		}
	SystemData *favorite = SystemData::getFavoriteSystem();
	if(hasFavorite) {
		if (favorite->getFavoritesCount() == 0) {
			removeFavoriteSystem();
		}
	}else {
		if (favorite->getFavoritesCount() > 0) {
			addSystem(favorite);
		}
	}
}
