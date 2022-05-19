#include <usernotifications/NotificationManager.h>
#include "views/gamelist/IGameListView.h"

void IGameListView::setTheme(const ThemeData& theme)
{
	mTheme = &theme;
	onThemeChanged(theme);
}

void IGameListView::updateInfoPanel()
{
  if (IsEmpty()) return;

  NotificationManager::Instance().Notify(*getCursor(), Notification::GamelistBrowsing);
  OnGameSelected();
}

void IGameListView::ApplyHelpStyle()
{
  HelpItemStyle().FromTheme(*mTheme, getName());
}
