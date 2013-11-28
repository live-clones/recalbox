#include "GuiMenu.h"
#include "GuiSettingsMenu.h"
#include "GuiScraperStart.h"
#include "../Window.h"

GuiMenu::GuiMenu(Window* window) : GuiComponent(window), mBackground(window, ":/button.png"), mList(window)
{
	mList.add("Settings", [&] { 
		mWindow->pushGui(new GuiSettingsMenu(mWindow));
	}, 0);

	mList.add("Scrape Systems", [&] {
		mWindow->pushGui(new GuiScraperStart(mWindow));
	}, 0);

	mList.add("Restart", [] { 
		if(system("sudo shutdown -r now") != 0)
			LOG(LogWarning) << "Restart terminated with non-zero result!";
	}, 0);

	mList.add("Shutdown", [] { 
		if(system("sudo shutdown -h now") != 0)
			LOG(LogWarning) << "Shutdown terminated with non-zero result!";
	}, 1);

	mList.add("Exit", [] { 
		SDL_Event* ev = new SDL_Event();
        ev->type = SDL_QUIT;
        SDL_PushEvent(ev);
	}, 0);


	setSize((float)Renderer::getScreenWidth(), (float)Renderer::getScreenHeight());

	mList.setPosition(mSize.x() * 0.175f, mSize.y() * 0.05f);
	mList.setSize(mSize.x() * 0.65f, mSize.y() * 0.9f);

	mBackground.fitTo(Eigen::Vector2f(mList.getSize().x(), mSize.y()), Eigen::Vector3f(mList.getPosition().x(), 0, 0));
	addChild(&mBackground);

	mTheme = std::make_shared<ThemeData>();
	mTheme->setFont("listFont", FontDef(0.09f, mTheme->getFontDef("listFont").path));
	mTheme->setColor("listSelectorColor", 0xBBBBBBFF);
	mTheme->setColor("listPrimaryColor", 0x0000FFFF);
	mTheme->setColor("listSecondaryColor", 0xFF0000FF);
	mList.setTheme(mTheme);

	addChild(&mList);
}

bool GuiMenu::input(InputConfig* config, Input input)
{
	if(input.value != 0)
	{
		if(config->isMappedTo("b", input) || config->isMappedTo("menu", input))
		{
			mTheme->playSound("menuCloseSound");
			delete this;
			return true;
		}else if(config->isMappedTo("a", input) && mList.getList().size() > 0)
		{
			mList.getSelected()();
			delete this;
			return true;
		}
	}

	return GuiComponent::input(config, input);
}
