#include "views/gamelist/IGameListView.h"
#include "Window.h"

bool IGameListView::ProcessInput(const InputCompactEvent& event)
{
	/*if (Settings::Instance().getBool("Debug") && config->getDeviceId() == InputEvent::sKeyboardDevice &&
     ((SDL_GetModState() & (KMOD_LCTRL | KMOD_RCTRL)) != 0) && input.Id() == SDLK_r && input.Value() != 0)
	{
		LOG(LogDebug) << "reloading view";
		ViewController::get()->reloadGameListView(this, true);
		return true;
	}*/

	return Component::ProcessInput(event);
}

void IGameListView::setTheme(const ThemeData& theme)
{
	mTheme = &theme;
	onThemeChanged(theme);
}

void IGameListView::ApplyHelpStyle()
{
  HelpItemStyle().FromTheme(*mTheme, getName());
}
