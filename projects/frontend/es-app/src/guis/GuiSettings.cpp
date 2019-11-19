#include "guis/GuiSettings.h"
#include "Window.h"
#include "Settings.h"
#include "views/ViewController.h"
#include "Locale.h"

GuiSettings::GuiSettings(Window&window, const char* title)
  : Gui(window),
    mMenu(window, title)
{
	addChild(&mMenu);

	mMenu.addButton(_("CLOSE"), _("CLOSE"), [this] { Close(); });

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

	Settings::Instance().saveFile();
}

bool GuiSettings::ProcessInput(const InputCompactEvent& event)
{
	if (event.APressed())
	{
		Close();
		return true;
	}

	if (event.StartPressed())
	{
		// close everything
		while((mWindow.peekGui() != nullptr) && mWindow.peekGui() != &ViewController::Instance())
			delete mWindow.peekGui();
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
