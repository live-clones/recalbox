#include "guis/GuiSettings.h"
#include "Window.h"
#include "Settings.h"
#include "views/ViewController.h"
#include "Locale.h"

GuiSettings::GuiSettings(Window* window, const char* title)
  : GuiComponent(window),
    mMenu(window, title)
{
	addChild(&mMenu);

	mMenu.addButton(_("CLOSE"), _("CLOSE"), [this] { delete this; });

	setSize(Renderer::getDisplayWidthAsFloat(), Renderer::getDisplayHeightAsFloat());
}

GuiSettings::~GuiSettings()
{
	if(doSave) save();
}

void GuiSettings::save()
{
	if(mSaveFuncs.empty())
		return;

	for (auto& saveFunc : mSaveFuncs)
		saveFunc();

	Settings::getInstance()->saveFile();
}

bool GuiSettings::ProcessInput(const InputCompactEvent& event)
{
	if (event.APressed())
	{
		delete this;
		return true;
	}

	if (event.StartPressed())
	{
		// close everything
		Window* window = mWindow;
		while((window->peekGui() != nullptr) && window->peekGui() != ViewController::get())
			delete window->peekGui();
		return true;
	}
	
	return GuiComponent::ProcessInput(event);
}

std::vector<HelpPrompt> GuiSettings::getHelpPrompts()
{
	std::vector<HelpPrompt> prompts = mMenu.getHelpPrompts();

	prompts.push_back(HelpPrompt("a", _("BACK")));
	prompts.push_back(HelpPrompt("start", _("CLOSE")));

	return prompts;
}
