#include "views/gamelist/IGameListView.h"
#include "Window.h"
#include "views/ViewController.h"
#include "Settings.h"
#include "utils/Log.h"
#include "Sound.h"

bool IGameListView::ProcessInput(const InputCompactEvent& event)
{
	/*if (Settings::getInstance()->getBool("Debug") && config->getDeviceId() == InputEvent::sKeyboardDevice &&
     ((SDL_GetModState() & (KMOD_LCTRL | KMOD_RCTRL)) != 0) && input.Id() == SDLK_r && input.Value() != 0)
	{
		LOG(LogDebug) << "reloading view";
		ViewController::get()->reloadGameListView(this, true);
		return true;
	}*/

	return GuiComponent::ProcessInput(event);
}

void IGameListView::setTheme(const std::shared_ptr<ThemeData>& theme)
{
	mTheme = theme;
	onThemeChanged(theme);
}

HelpStyle IGameListView::getHelpStyle()
{
	HelpStyle style;
	style.applyTheme(mTheme, getName());
	return style;
}
