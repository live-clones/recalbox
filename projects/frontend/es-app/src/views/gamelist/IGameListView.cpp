#include <usernotifications/NotificationManager.h>
#include "views/gamelist/IGameListView.h"
#include "Window.h"

void IGameListView::setTheme(const ThemeData& theme)
{
	mTheme = &theme;
	onThemeChanged(theme);
}

void IGameListView::updateInfoPanel()
{
  NotificationManager::Instance().Notify(*getCursor(), Notification::GamelistBrowsing);
  DoUpdateGameInformation();
}

void IGameListView::ApplyHelpStyle()
{
  HelpItemStyle().FromTheme(*mTheme, getName());
}
