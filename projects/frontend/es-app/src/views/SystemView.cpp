#include "views/SystemView.h"
#include "views/ViewController.h"
#include "animations/LambdaAnimation.h"
#include <guis/GuiMsgBox.h>
#include <RecalboxConf.h>
#include <guis/GuiNetPlay.h>
#include <systems/SystemManager.h>
#include <MainRunner.h>
#include "guis/GuiMenu.h"
#include "AudioManager.h"

// buffer values for scrolling velocity (left, stopped, right)
const int logoBuffersLeft[] = { -5, -2, -1 };
const int logoBuffersRight[] = { 1, 2, 5 };

SystemView::SystemView(Window& window, SystemManager& systemManager)
  : IList<SystemViewData, SystemData*>(window, LIST_SCROLL_STYLE_SLOW, LoopType::Always),
    mSystemManager(systemManager),
    mCarousel(),
    mSystemInfo(window, "SYSTEM INFO", Font::get(FONT_SIZE_SMALL), 0x33333300, TextAlignment::Center),
    mCamOffset(0),
    mExtrasCamOffset(0),
    mExtrasFadeOpacity(0.0f),
    lastSystem(nullptr),
    mViewNeedsReload(true),
    mShowing(false),
		launchKodi(false)
{
	setSize(Renderer::getDisplayWidthAsFloat(), Renderer::getDisplayHeightAsFloat());
	populate();
}

void SystemView::addSystem(SystemData * it){
	if(!(it)->getRootFolder()->hasChildren()){
		return;
	}
	const ThemeData& theme = (it)->getTheme();
	
	if(mViewNeedsReload)
			getViewElements(theme);

	Entry e;
	e.name = (it)->getName();
	e.object = it;

	// make logo
	if(theme.getElement("system", "logo", "image") != nullptr)
	{
		ImageComponent* logo = new ImageComponent(mWindow, false, false);
		logo->setMaxSize(mCarousel.logoSize * mCarousel.logoScale);
		logo->applyTheme((it)->getTheme(), "system", "logo", ThemeProperties::Path);
		e.data.logo = std::shared_ptr<GuiComponent>(logo);
		if ((it)->ThemeFolder() == "default")
		{
			TextComponent* text = new TextComponent(mWindow,
                                              (it)->getName(),
                                              Font::get(FONT_SIZE_MEDIUM),
                                              0x1A1A1AFF,
                                              TextAlignment::Center);
			text->setSize(mCarousel.logoSize * mCarousel.logoScale);
			e.data.logotext = std::shared_ptr<GuiComponent>(text);
			if (mCarousel.type == CarouselType::Vertical || mCarousel.type == CarouselType::VerticalWheel)
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
                                            TextAlignment::Center);
		text->setSize(mCarousel.logoSize * mCarousel.logoScale);
		e.data.logo = std::shared_ptr<GuiComponent>(text);
    if (mCarousel.type == CarouselType::Vertical || mCarousel.type == CarouselType::VerticalWheel)
			text->setHorizontalAlignment(mCarousel.logoAlignment);
    else
			text->setVerticalAlignment(mCarousel.logoAlignment);
	}

  if (mCarousel.type == CarouselType::Vertical || mCarousel.type == CarouselType::VerticalWheel)
	{
		if (mCarousel.logoAlignment == TextAlignment::Left)
			e.data.logo->setOrigin(0, 0.5);
		else if (mCarousel.logoAlignment == TextAlignment::Right)
			e.data.logo->setOrigin(1.0, 0.5);
		else
			e.data.logo->setOrigin(0.5, 0.5);
	} else {
		if (mCarousel.logoAlignment == TextAlignment::Top)
			e.data.logo->setOrigin(0.5, 0);
		else if (mCarousel.logoAlignment == TextAlignment::Bottom)
			e.data.logo->setOrigin(0.5, 1);
		else
			e.data.logo->setOrigin(0.5, 0.5);
	}

	Vector2f denormalized = mCarousel.logoSize * e.data.logo->getOrigin();
	e.data.logo->setPosition(denormalized.x(), denormalized.y(), 0.0);

	if (e.data.logotext)
	{
    if (mCarousel.type == CarouselType::Vertical || mCarousel.type == CarouselType::VerticalWheel)
		{
			if (mCarousel.logoAlignment == TextAlignment::Left)
				e.data.logotext->setOrigin(0, 0.5);
			else if (mCarousel.logoAlignment == TextAlignment::Right)
				e.data.logotext->setOrigin(1.0, 0.5);
			else
				e.data.logotext->setOrigin(0.5, 0.5);
		} else {
			if (mCarousel.logoAlignment == TextAlignment::Top)
				e.data.logotext->setOrigin(0.5, 0);
			else if (mCarousel.logoAlignment == TextAlignment::Bottom)
				e.data.logotext->setOrigin(0.5, 1);
			else
				e.data.logotext->setOrigin(0.5, 0.5);
		}

		denormalized = mCarousel.logoSize * e.data.logotext->getOrigin();
		e.data.logotext->setPosition(denormalized.x(), denormalized.y(), 0.0);
	}

	e.data.backgroundExtras = std::make_shared<ThemeExtras>(mWindow);
	e.data.backgroundExtras->setExtras(ThemeData::makeExtras((it)->getTheme(), "system", mWindow));


	// sort the extras by z-index
	e.data.backgroundExtras->sortExtrasByZIndex();

	this->add(e);
}
void SystemView::populate()
{
	mEntries.clear();

	for (auto& it : mSystemManager.GetVisibleSystemList())
	{
		addSystem(it);
	}
}

void SystemView::goToSystem(SystemData* system, bool animate)
{
	if (!setCursor(system)) // When deleting last favorite from favorite view, favorite system is no longer available
	  setCursor(mSystemManager.FirstNonEmptySystem());

	if(!animate)
		finishAnimation(0);
	onCursorChanged(CursorState::Stopped);
}

bool SystemView::ProcessInput(const InputCompactEvent& event)
{
	if (event.AnythingPressed())
	{
		/*if(config->getDeviceId() == InputEvent::sKeyboardDevice && input.Id() == SDLK_r && ((SDL_GetModState() & KMOD_LCTRL) != 0) && Settings::Instance().getBool("Debug"))
		{
			LOG(LogInfo) << " Reloading all";
			ViewController::get()->reloadAll();
			return true;
		}*/
		switch (mCarousel.type)
		{
		case CarouselType::Vertical:
		case CarouselType::VerticalWheel:
			if (event.AnyUpPressed())
			{
				listInput(-1);
				return true;
			}
			if (event.AnyDownPressed())
			{
				listInput(1);
				return true;
			}
			break;
		case CarouselType::Horizontal:
		default:
			if (event.AnyLeftPressed())
			{
				listInput(-1);
				return true;
			}
			if (event.AnyRightPressed())
			{
				listInput(1);
				return true;
			}
			break;	
		}
		if (event.BPressed())
		{
			stopScrolling();
      ViewController::Instance().goToGameList(getSelected());
			return true;
		}
    if (event.XPressed())
    {
    bool kodiEnabled = RecalboxConf::Instance().AsBool("kodi.enabled");
    bool kodiX = RecalboxConf::Instance().AsBool("kodi.xbutton");
    bool netplay = RecalboxConf::Instance().AsBool("global.netplay");

    if (kodiEnabled && kodiX && !launchKodi && !mWindow.HasGui())
    {
      if (netplay)
      {
        auto s = new GuiSettings(mWindow, _("KODI/NETPLAY").c_str());
        auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
        ComponentListRow row;
        row.makeAcceptInputHandler([this, s] {
            launchKodi = true;
            if( ! RecalboxSystem::launchKodi(mWindow)) {
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
            auto netplay = new GuiNetPlay(mWindow, mSystemManager);
            mWindow.pushGui(netplay);
            delete s;
        });
        auto lbl2 = std::make_shared<TextComponent>(mWindow, "\uF1c4 " + _("NETPLAY LOBBY"), menuTheme->menuText.font, menuTheme->menuText.color);
        row.addElement(lbl2, true); // label
        s->addRow(row);
        mWindow.pushGui(s);
      }
      else
      {
        launchKodi = true;
        if( ! RecalboxSystem::launchKodi(mWindow))
        {
            LOG(LogWarning) << "Shutdown terminated with non-zero result!";
        }
        launchKodi = false;
      }
    } else if (netplay && !mWindow.HasGui()) {
            auto netplayGui = new GuiNetPlay(mWindow, mSystemManager);
            mWindow.pushGui(netplayGui);
    }


    }
		if (event.SelectPressed() && RecalboxConf::Instance().AsString("emulationstation.menu") != "none")
		{
		  auto s = new GuiSettings(mWindow, _("QUIT").c_str());
			auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
			ComponentListRow row;

			row.makeAcceptInputHandler([this] {
        mWindow.pushGui(new GuiMsgBox(mWindow, _("REALLY SHUTDOWN?"), _("YES"),
											  [] { MainRunner::RequestQuit(MainRunner::ExitState::Shutdown); }, _("NO"), nullptr));
			});
			auto icon2 = std::make_shared<ImageComponent>(mWindow);
			icon2->setImage(menuTheme->menuIconSet.shutdown);
			icon2->setColorShift(menuTheme->menuText.color);
			icon2->setResize(0, menuTheme->menuText.font->getLetterHeight() * 1.25f);
			row.addElement(icon2, false);
			// spacer between icon and text
			auto spacer = std::make_shared<GuiComponent>(mWindow);
			row.addElement(spacer, false);
			row.addElement(std::make_shared<TextComponent>(mWindow, _("SHUTDOWN SYSTEM"), menuTheme->menuText.font, menuTheme->menuText.color), true);
			s->addRow(row);
			row.elements.clear();
			row.makeAcceptInputHandler([this] {
        mWindow.pushGui(new GuiMsgBox(mWindow, _("REALLY SHUTDOWN WITHOUT SAVING METADATAS?"), _("YES"),
											  [] { MainRunner::RequestQuit(MainRunner::ExitState::FastShutdown);; }, _("NO"), nullptr));
			});
			auto icon3 = std::make_shared<ImageComponent>(mWindow);
			icon3->setImage(menuTheme->menuIconSet.fastshutdown);
			icon3->setColorShift(menuTheme->menuText.color);
			icon3->setResize(0, menuTheme->menuText.font->getLetterHeight() * 1.25f);
			row.addElement(icon3, false);
			// spacer between icon and text
			row.addElement(spacer, false);
			row.addElement(std::make_shared<TextComponent>(mWindow, _("FAST SHUTDOWN SYSTEM"), menuTheme->menuText.font, menuTheme->menuText.color), true);
			s->addRow(row);
			row.elements.clear();
			row.makeAcceptInputHandler([this] {
        mWindow.pushGui(new GuiMsgBox(mWindow, _("REALLY RESTART?"), _("YES"),
				                              [] { MainRunner::RequestQuit(MainRunner::ExitState::NormalReboot); }, _("NO"), nullptr));
			});
			// icon
			auto icon1 = std::make_shared<ImageComponent>(mWindow);
			icon1->setImage(menuTheme->menuIconSet.restart);
			icon1->setColorShift(menuTheme->menuText.color);
			icon1->setResize(0, menuTheme->menuText.font->getLetterHeight() * 1.25f);
			row.addElement(icon1, false);


			spacer->setSize(10, 0);
			row.addElement(spacer, false);
			row.addElement(std::make_shared<TextComponent>(mWindow, _("RESTART SYSTEM"), menuTheme->menuText.font, menuTheme->menuText.color), true);
			s->addRow(row);
			mWindow.pushGui(s);
		}

		if (event.StartPressed() && RecalboxConf::Instance().AsString("emulationstation.menu") != "none")
		{
			mWindow.pushGui(new GuiMenu(mWindow, mSystemManager));
			return true;
		}

	}
	else if (event.AnyLeftReleased() || event.AnyRightReleased() || event.AnyUpReleased() || event.AnyDownReleased())
	  listInput(0);

	return GuiComponent::ProcessInput(event);
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
    AudioManager::Instance().StartPlaying(getSelected()->getTheme());
	}
	// update help style
	updateHelpPrompts();

	// update externs
  RecalboxSystem::NotifySystem(*getSelected());

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

	const float infoStartOpacity = (float)mSystemInfo.getOpacity() / 255.f;

	Animation* infoFadeOut = new LambdaAnimation(
		[infoStartOpacity, this] (float t)
	{
		mSystemInfo.setOpacity((unsigned char)(lerp<float>(infoStartOpacity, 0.f, t) * 255));
	}, (int)(infoStartOpacity * 150));

	unsigned int gameCount = getSelected()->GameCount();
	unsigned int favoritesCount = getSelected()->FavoritesCount();
	unsigned int hiddenCount = getSelected()->HiddenCount();
	unsigned int gameNoHiddenCount = gameCount - hiddenCount;

	// also change the text after we've fully faded out
	setAnimation(infoFadeOut, 0, [this, favoritesCount, gameNoHiddenCount, hiddenCount] {
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
    bool move_carousel = Settings::Instance().MoveCarousel();
	std::string transition_style = Settings::Instance().TransitionStyle();
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

bool SystemView::getHelpPrompts(Help& help)
{
	help.Set(mCarousel.type == CarouselType::Vertical ? HelpType::UpDown : HelpType::LeftRight, _("CHOOSE"))
	    .Set(HelpType::B, _("SELECT"));

	if (RecalboxConf::Instance().AsBool("kodi.enabled") && RecalboxConf::Instance().AsBool("kodi.xbutton"))
    help.Set(HelpType::X, RecalboxConf::Instance().AsBool("global.netplay") ? _("KODI/NETPLAY") : _("START KODI"));
	else if (RecalboxConf::Instance().AsBool("global.netplay"))
	  help.Set(HelpType::X, _("NETPLAY"));

	help.Set(HelpType::Select, _("QUIT"))
	    .Set(HelpType::Start, _("MENU"));

	return true;
}	

void SystemView::ApplyHelpStyle()
{
  HelpItemStyle().FromTheme(mEntries.at(mCursor).object->getTheme(), "system");
}

void  SystemView::onThemeChanged(const ThemeData& theme)
{
	(void)theme; // TODO: Log theme name
	LOG(LogDebug) << "SystemView::onThemeChanged()";
	mViewNeedsReload = true;
	populate();
}	

//  Get the ThemeElements that make up the SystemView.
void  SystemView::getViewElements(const ThemeData& theme)
{
		LOG(LogDebug) << "SystemView::getViewElements()";
		getDefaultElements();
		
		const ThemeElement* carouselElem = theme.getElement("system", "systemcarousel", "carousel");
		if (carouselElem != nullptr)
			getCarouselFromTheme(carouselElem);
		
		const ThemeElement* sysInfoElem = theme.getElement("system", "systemInfo", "text");
		if (sysInfoElem != nullptr)
			mSystemInfo.applyTheme(theme, "system", "systemInfo", ThemeProperties::All);
		
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
	float xOff;
	float yOff;
	
	switch (mCarousel.type)
	{
		case CarouselType::VerticalWheel:
			yOff = (mCarousel.size.y() - mCarousel.logoSize.y()) / 2 - (mCamOffset * logoSpacing[1]);
			if (mCarousel.logoAlignment == TextAlignment::Left)
				xOff = mCarousel.logoSize.x() / 10;
			else if (mCarousel.logoAlignment == TextAlignment::Right)
				xOff = (float)(mCarousel.size.x() - (mCarousel.logoSize.x() * 1.1));
			else
				xOff = (mCarousel.size.x() - mCarousel.logoSize.x()) / 2;
			break;
		case CarouselType::Vertical:
			logoSpacing[1] = ((mCarousel.size.y() - (mCarousel.logoSize.y() * (float)mCarousel.maxLogoCount)) / (float)(mCarousel.maxLogoCount)) + mCarousel.logoSize.y();
			yOff = (mCarousel.size.y() - mCarousel.logoSize.y()) / 2 - (mCamOffset * logoSpacing[1]);

			if (mCarousel.logoAlignment == TextAlignment::Left)
				xOff = mCarousel.logoSize.x() / 10;
			else if (mCarousel.logoAlignment == TextAlignment::Right)
				xOff = (float)(mCarousel.size.x() - (mCarousel.logoSize.x() * 1.1));
			else
				xOff = (mCarousel.size.x() - mCarousel.logoSize.x()) / 2;
			break;
		case CarouselType::Horizontal:
		default:
			logoSpacing[0] = ((mCarousel.size.x() - (mCarousel.logoSize.x() * (float)mCarousel.maxLogoCount)) / (float)(mCarousel.maxLogoCount)) + mCarousel.logoSize.x();
			xOff = (mCarousel.size.x() - mCarousel.logoSize.x()) / 2 - (mCamOffset * logoSpacing[0]);

			if (mCarousel.logoAlignment == TextAlignment::Top)
				yOff = mCarousel.logoSize.y() / 10;
			else if (mCarousel.logoAlignment == TextAlignment::Bottom)
				yOff = (float)(mCarousel.size.y() - (mCarousel.logoSize.y() * 1.1));
			else
				yOff = (mCarousel.size.y() - mCarousel.logoSize.y()) / 2;
			break;
	}

	int center = (int)(mCamOffset);
	int logoCount = Math::min(mCarousel.maxLogoCount, (int)mEntries.size());
	
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
		logoTrans.translate(Vector3f((float)i * logoSpacing[0] + xOff, (float)i * logoSpacing[1] + yOff, 0));

		float distance = (float)i - mCamOffset;

		float scale = 1.0f + ((mCarousel.logoScale - 1.0f) * (1.0f - fabs(distance)));
		scale = Math::min(mCarousel.logoScale, Math::max(1.0f, scale));
		scale /= mCarousel.logoScale;

		int opacity = Math::roundi(0x80 + ((0xFF - 0x80) * (1 - fabs(distance))));
		opacity = Math::max((int) 0x80, opacity);

		const std::shared_ptr<GuiComponent> &comp = mEntries.at(index).data.logo;
		if (mCarousel.type == CarouselType::VerticalWheel) {
			comp->setRotationDegrees(mCarousel.logoRotation * distance);
			comp->setRotationOrigin(mCarousel.logoRotationOrigin);
		}
		comp->setScale(scale);
		comp->setOpacity(opacity);
		comp->render(logoTrans);

		if (mEntries.at(index).data.logotext)
		{
			const std::shared_ptr<GuiComponent> &comp2 = mEntries.at(index).data.logotext;
			if (mCarousel.type == CarouselType::VerticalWheel) {
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
			if (mCarousel.type == CarouselType::Horizontal)
				extrasTrans.translate(Vector3f(((float)i - mExtrasCamOffset) * mSize.x(), 0, 0));
			else
				extrasTrans.translate(Vector3f(0, ((float)i - mExtrasCamOffset) * mSize.y(), 0));

			Renderer::pushClipRect(Vector2i((int)extrasTrans.translation()[0], (int)extrasTrans.translation()[1]),
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
	if (mExtrasFadeOpacity != 0.0f)
	{
			Renderer::setMatrix(trans);
			Renderer::drawRect(0.0f, 0.0f, mSize.x(), mSize.y(), 0x00000000 | (unsigned char)(mExtrasFadeOpacity * 255));
	}
}

// Populate the system carousel with the legacy values
void  SystemView::getDefaultElements()
{
	// Carousel
	mCarousel.type = CarouselType::Horizontal;
	mCarousel.logoAlignment = TextAlignment::Center;
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
	mCarousel.logoSize.x() = 0.25f * (Math::max(mSize.y(), mSize.x()));
	mCarousel.logoSize.y() = 0.155f * (Math::min(mSize.y(), mSize.x()));
	mCarousel.maxLogoCount = 3;
	mCarousel.zIndex = 40;

	// System Info Bar
	mSystemInfo.setSize(mSize.x(), mSystemInfo.getFont()->getLetterHeight()*2.2f);
	mSystemInfo.setPosition(0, (mCarousel.pos.y() + mCarousel.size.y() - 0.2f));
	mSystemInfo.setBackgroundColor(0xDDDDDDD8);
	mSystemInfo.setRenderBackground(true);
	mSystemInfo.setFont(Font::get((int)(0.035f * (Math::min(mSize.y(), mSize.x()))), Font::getDefaultPath()));
	mSystemInfo.setHorizontalAlignment(TextAlignment::Center);
	mSystemInfo.setColor(0x000000FF);
	mSystemInfo.setZIndex(50);
	mSystemInfo.setDefaultZIndex(50);
}

void SystemView::getCarouselFromTheme(const ThemeElement* elem)
{
	if (elem->HasProperty("type"))
	{
		if (elem->AsString("type") == "vertical")
			mCarousel.type = CarouselType::Vertical;
		else if (elem->AsString("type") == "vertical_wheel")
			mCarousel.type = CarouselType::VerticalWheel;
		else
			mCarousel.type = CarouselType::Horizontal;
	}
	if (elem->HasProperty("size"))
		mCarousel.size = elem->AsVector("size") * mSize;
	if (elem->HasProperty("pos"))
		mCarousel.pos = elem->AsVector("pos") * mSize;
	if (elem->HasProperty("origin"))
		mCarousel.origin = elem->AsVector("origin");
	if (elem->HasProperty("color"))
		mCarousel.color = (unsigned int)elem->AsInt("color");
	if (elem->HasProperty("logoScale"))
		mCarousel.logoScale = elem->AsFloat("logoScale");
	if (elem->HasProperty("logoSize"))
		mCarousel.logoSize = elem->AsVector("logoSize") * mSize;
	if (elem->HasProperty("maxLogoCount"))
		mCarousel.maxLogoCount = Math::roundi(elem->AsFloat("maxLogoCount"));
	if (elem->HasProperty("zIndex"))
		mCarousel.zIndex = elem->AsFloat("zIndex");
	if (elem->HasProperty("logoRotation"))
		mCarousel.logoRotation = elem->AsFloat("logoRotation");
	if (elem->HasProperty("logoRotationOrigin"))
		mCarousel.logoRotationOrigin = elem->AsVector("logoRotationOrigin");
	if (elem->HasProperty("logoAlignment"))
	{
		if (elem->AsString("logoAlignment") == "left")
			mCarousel.logoAlignment = TextAlignment::Left;
		else if (elem->AsString("logoAlignment") == "right")
			mCarousel.logoAlignment = TextAlignment::Right;
		else if (elem->AsString("logoAlignment") == "top")
			mCarousel.logoAlignment = TextAlignment::Top;
		else if (elem->AsString("logoAlignment") == "bottom")
			mCarousel.logoAlignment = TextAlignment::Bottom;
		else
			mCarousel.logoAlignment = TextAlignment::Center;
	}
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
	SystemData *favorite = mSystemManager.FavoriteSystem();
	if(hasFavorite) {
		if (favorite->FavoritesCount() == 0) {
			removeFavoriteSystem();
		}
	}else {
		if (favorite->FavoritesCount() > 0) {
			addSystem(favorite);
		}
	}
}
