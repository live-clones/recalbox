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
#include "ThemeData.h"
#include "MenuThemeData.h"
#include "AudioManager.h"
#include "Locale.h"

// buffer values for scrolling velocity (left, stopped, right)
const int logoBuffersLeft[] = { -5, -2, -1 };
const int logoBuffersRight[] = { 1, 2, 5 };

SystemView::SystemView(Window* window) : IList<SystemViewData, SystemData*>(window, LIST_SCROLL_STYLE_SLOW, LIST_ALWAYS_LOOP),
																				mViewNeedsReload(true),
																				mSystemInfo(window, "SYSTEM INFO", Font::get(FONT_SIZE_SMALL), 0x33333300, ALIGN_CENTER)
{
	mCamOffset = 0;
	mExtrasCamOffset = 0;
	mExtrasFadeOpacity = 0.0f;

	setSize((float)Renderer::getScreenWidth(), (float)Renderer::getScreenHeight());


	populate();
}

void SystemView::addSystem(SystemData * it){
	if((it)->getRootFolder()->getChildren().size() == 0){
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
		logo->setMaxSize(Eigen::Vector2f(mCarousel.logoSize.x(), mCarousel.logoSize.y()));
		logo->applyTheme((it)->getTheme(), "system", "logo", ThemeFlags::PATH);
		logo->setPosition((mCarousel.logoSize.x() - logo->getSize().x()) / 2,
				(mCarousel.logoSize.y() - logo->getSize().y()) / 2); // center
		e.data.logo = std::shared_ptr<GuiComponent>(logo);

		ImageComponent* logoSelected = new ImageComponent(mWindow, false, false);
		logoSelected->setMaxSize(Eigen::Vector2f(mCarousel.logoSize.x() * mCarousel.logoScale, mCarousel.logoSize.y() * mCarousel.logoScale));
		logoSelected->applyTheme((it)->getTheme(), "system", "logo", ThemeFlags::PATH | ThemeFlags::COLOR);
		logoSelected->setPosition((mCarousel.logoSize.x() - logoSelected->getSize().x()) / 2,
				(mCarousel.logoSize.y() - logoSelected->getSize().y()) / 2); // center
		e.data.logoSelected = std::shared_ptr<GuiComponent>(logoSelected);
	}else{
		// no logo in theme; use text
		TextComponent* text = new TextComponent(mWindow,
												(it)->getName(),
												Font::get(FONT_SIZE_LARGE),
												0x000000FF,
												ALIGN_CENTER);
		text->setSize(mCarousel.logoSize);
		e.data.logo = std::shared_ptr<GuiComponent>(text);

		TextComponent* textSelected = new TextComponent(mWindow,
														(it)->getName(),
														Font::get((int)(FONT_SIZE_LARGE * 1.5)),
														0x000000FF,
														ALIGN_CENTER);
		textSelected->setSize(mCarousel.logoSize);
		e.data.logoSelected = std::shared_ptr<GuiComponent>(textSelected);
	}
	
	// delete any existing extras
		for (auto extra : e.data.backgroundExtras)
			delete extra;
		e.data.backgroundExtras.clear();

	// make background extras
	e.data.backgroundExtras = ThemeData::makeExtras((it)->getTheme(), "system", mWindow);
	
		// sort the extras by z-index
		std:stable_sort(e.data.backgroundExtras.begin(), e.data.backgroundExtras.end(),  [](GuiComponent* a, GuiComponent* b) {
			return b->getZIndex() > a->getZIndex();
		});

	this->add(e);
}
void SystemView::populate()
{
	mEntries.clear();

	for(auto it = SystemData::sSystemVector.begin(); it != SystemData::sSystemVector.end(); it++)
	{
		addSystem((*it));
	}
}

void SystemView::goToSystem(SystemData* system, bool animate)
{

        
	setCursor(system);

	if(!animate)
		finishAnimation(0);
	onCursorChanged(CURSOR_STOPPED);
}

bool SystemView::input(InputConfig* config, Input input)
{
	if(input.value != 0)
	{
		if(config->getDeviceId() == DEVICE_KEYBOARD && input.value && input.id == SDLK_r && SDL_GetModState() & KMOD_LCTRL && Settings::getInstance()->getBool("Debug"))
		{
			LOG(LogInfo) << " Reloading all";
			ViewController::get()->reloadAll();
			return true;
		}
		switch (mCarousel.type)
		{
		case VERTICAL:
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
		if(config->isMappedTo("b", input))
		{
			stopScrolling();
			ViewController::get()->goToGameList(getSelected());
			return true;
		}
		if(config->isMappedTo("select", input) && RecalboxConf::getInstance()->get("emulationstation.menu") != "none")
		{
		  auto s = new GuiSettings(mWindow, _("QUIT").c_str());
			auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
			Window *window = mWindow;
			ComponentListRow row;
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

			// spacer between icon and text
			auto spacer = std::make_shared<GuiComponent>(mWindow);
			spacer->setSize(10, 0);
			row.addElement(spacer, false);
			row.addElement(std::make_shared<TextComponent>(window, _("RESTART SYSTEM"), menuTheme->menuText.font, menuTheme->menuText.color), true);
			s->addRow(row);

			row.elements.clear();
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
			mWindow->pushGui(s);
		}

	}else{
		if(config->isMappedTo("left", input) || 
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
    
	if(lastSystem != getSelected()){
		lastSystem = getSelected();
		AudioManager::getInstance()->themeChanged(getSelected()->getTheme());
	}
	// update help style
	updateHelpPrompts();

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

void SystemView::render(const Eigen::Affine3f& parentTrans)
{
	if(size() == 0)
		return;  // nothing to render
	
	// draw the list elements (titles, backgrounds, logos)
	Eigen::Affine3f trans = getTransform() * parentTrans;

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
void SystemView::renderCarousel(const Eigen::Affine3f& trans)
{
	Eigen::Vector2i clipPos((int)mCarousel.pos.x(), (int)mCarousel.pos.y());
	Eigen::Vector2i clipSize((int)mCarousel.size.x(), (int)mCarousel.size.y());
	Renderer::pushClipRect(clipPos, clipSize);
	
	// background box behind logos
	Renderer::setMatrix(trans);
	Renderer::drawRect(mCarousel.pos.x(), mCarousel.pos.y(), mCarousel.size.x(), mCarousel.size.y(), mCarousel.color);
	
	// draw logos
	Eigen::Vector2f logoSpacing(0.0, 0.0); // NB: logoSpacing will include the size of the logo itself as well!
	float xOff = 0.0;
	float yOff = 0.0;
	
	switch (mCarousel.type)
	{
		case VERTICAL:
			logoSpacing[1] = ((mCarousel.size.y() - (mCarousel.logoSize.y() * mCarousel.maxLogoCount)) / (mCarousel.maxLogoCount)) + mCarousel.logoSize.y();
			xOff = mCarousel.pos.x() + (mCarousel.size.x() / 2) - (mCarousel.logoSize.x() / 2);
			yOff = mCarousel.pos.y() + (mCarousel.size.y() - mCarousel.logoSize.y()) / 2 - (mCamOffset * logoSpacing[1]);
			break;
		case HORIZONTAL:
		default:
			logoSpacing[0] = ((mCarousel.size.x() - (mCarousel.logoSize.x() * mCarousel.maxLogoCount)) / (mCarousel.maxLogoCount)) + mCarousel.logoSize.x();
			xOff = mCarousel.pos.x() + (mCarousel.size.x() - mCarousel.logoSize.x()) / 2 - (mCamOffset * logoSpacing[0]);
			yOff = mCarousel.pos.y() + (mCarousel.size.y() / 2) - (mCarousel.logoSize.y() / 2);
			break;
	}

	Eigen::Affine3f logoTrans = trans;
	int center = (int)(mCamOffset);
	int logoCount = std::min(mCarousel.maxLogoCount, (int)mEntries.size());
	
	// Adding texture loading buffers depending on scrolling speed and status
	int bufferIndex = getScrollingVelocity() + 1;

	int bufferLeft = logoBuffersLeft[bufferIndex];
	int bufferRight = logoBuffersRight[bufferIndex];
	if (mCarousel.maxLogoCount >= logoCount)
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

		logoTrans.translation() = trans.translation() + Eigen::Vector3f(i * logoSpacing[0] + xOff, i * logoSpacing [1] + yOff, 0);

		if (index == mCursor) //Selected System
		{
			const std::shared_ptr<GuiComponent>& comp = mEntries.at(index).data.logoSelected;
			comp->setOpacity(0xFF);
			comp->render(logoTrans);
		}
		else { // not selected systems
			const std::shared_ptr<GuiComponent>& comp = mEntries.at(index).data.logo;
			comp->setOpacity(0x80);
			comp->render(logoTrans);
		}
	}
	Renderer::popClipRect();
}

void SystemView::renderInfoBar(const Eigen::Affine3f& trans)
{
	Renderer::setMatrix(trans);
	mSystemInfo.render(trans);
}


// Draw background extras
void SystemView::renderExtras(const Eigen::Affine3f& trans, float lower, float upper)
{	
	int extrasCenter = (int)mExtrasCamOffset;
	
	Renderer::pushClipRect(Eigen::Vector2i(0, 0), mSize.cast<int>());
	
	// Adding texture loading buffers depending on scrolling speed and status
	int bufferIndex = getScrollingVelocity() + 1;
	
	for (int i = extrasCenter + logoBuffersLeft[bufferIndex]; i <= extrasCenter + logoBuffersRight[bufferIndex]; i++)
	{
		int index = i;
		while (index < 0)
			index += mEntries.size();
		while (index >= (int)mEntries.size())
			index -= mEntries.size();

		Eigen::Affine3f extrasTrans = trans;
		if (mCarousel.type == HORIZONTAL)
			extrasTrans.translate(Eigen::Vector3f((i - mExtrasCamOffset) * mSize.x(), 0, 0));
		else
			extrasTrans.translate(Eigen::Vector3f(0, (i - mExtrasCamOffset) * mSize.y(), 0));
		
		Renderer::pushClipRect(Eigen::Vector2i(extrasTrans.translation()[0], extrasTrans.translation()[1]), mSize.cast<int>());
		
		SystemViewData data = mEntries.at(index).data;
		for(unsigned int j = 0; j < data.backgroundExtras.size(); j++)
		{
			GuiComponent* extra = data.backgroundExtras[j];
			if (extra->getZIndex() >= lower && extra->getZIndex() < upper) {
				extra->render(extrasTrans);
			}
		}
		Renderer::popClipRect();
	}
	Renderer::popClipRect();
}

void SystemView::renderFade(const Eigen::Affine3f& trans)
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
	mCarousel.size.x() = mSize.x();
	mCarousel.size.y() = 0.2325f * mSize.y();
	mCarousel.pos.x() = 0.0f;
	mCarousel.pos.y() = 0.5f * (mSize.y() - mCarousel.size.y());
	mCarousel.color = 0xFFFFFFD8;
	mCarousel.logoScale = 1.2f;
	mCarousel.logoSize.x() = 0.25f * mSize.x();
	mCarousel.logoSize.y() = 0.155f * mSize.y();
	mCarousel.maxLogoCount = 3;
	mCarousel.zIndex = 40;

	// System Info Bar
	mSystemInfo.setSize(mSize.x(), mSystemInfo.getFont()->getLetterHeight()*2.2f);
	mSystemInfo.setPosition(0, (mCarousel.pos.y() + mCarousel.size.y() - 0.2f));
	mSystemInfo.setBackgroundColor(0xDDDDDDD8);
	mSystemInfo.setRenderBackground(true);
	mSystemInfo.setFont(Font::get((int)(0.035f * mSize.y()), Font::getDefaultPath()));
	mSystemInfo.setColor(0x000000FF);
	mSystemInfo.setZIndex(50);
	mSystemInfo.setDefaultZIndex(50);
}

void SystemView::getCarouselFromTheme(const ThemeData::ThemeElement* elem)
{
	if (elem->has("type"))
		mCarousel.type = !(elem->get<std::string>("type").compare("vertical")) ? VERTICAL : HORIZONTAL;
	if (elem->has("size"))
		mCarousel.size = elem->get<Eigen::Vector2f>("size").cwiseProduct(mSize);
	if (elem->has("pos"))
		mCarousel.pos = elem->get<Eigen::Vector2f>("pos").cwiseProduct(mSize);
	if (elem->has("color"))
		mCarousel.color = elem->get<unsigned int>("color");
	if (elem->has("logoScale"))
		mCarousel.logoScale = elem->get<float>("logoScale");
	if (elem->has("logoSize"))
		mCarousel.logoSize = elem->get<Eigen::Vector2f>("logoSize").cwiseProduct(mSize);
	if (elem->has("maxLogoCount"))
		mCarousel.maxLogoCount = std::round(elem->get<float>("maxLogoCount"));
	if (elem->has("zIndex"))
		mCarousel.zIndex = elem->get<float>("zIndex");	
}

void SystemView::removeFavoriteSystem(){
	for(auto it = mEntries.begin(); it != mEntries.end(); it++)
		if(it->object->isFavorite()){
			mEntries.erase(it);
			break;
		}
}

void SystemView::manageFavorite(){
	bool hasFavorite = false;
	for(auto it = mEntries.begin(); it != mEntries.end(); it++)
		if(it->object->isFavorite()){
			hasFavorite = true;
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
