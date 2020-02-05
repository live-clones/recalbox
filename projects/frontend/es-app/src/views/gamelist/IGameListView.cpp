#include "views/gamelist/IGameListView.h"
#include "Window.h"

void IGameListView::setTheme(const ThemeData& theme)
{
	mTheme = &theme;
	onThemeChanged(theme);
}

void IGameListView::ApplyHelpStyle()
{
  HelpItemStyle().FromTheme(*mTheme, getName());
}
