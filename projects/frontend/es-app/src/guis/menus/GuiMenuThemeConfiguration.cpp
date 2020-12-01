//
// Created by bkg2k on 31/10/2020.
//

#include <MainRunner.h>
#include "GuiMenuThemeConfiguration.h"
#include "guis/MenuMessages.h"

GuiMenuThemeConfiguration::GuiMenuThemeConfiguration(Window& window, const std::string& themeName)
  : GuiMenuBase(window, _("THEME CONFIGURATION"))
  , mThemeName(themeName)
  , mReloadRequired(false)
{
  auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();

  StringMaps themeSubSets = ThemeData::getThemeSubSets(themeName);
  mColorSet     = BuildSelector(_("THEME COLORSET"    ), RecalboxConf::Instance().GetThemeColorSet(themeName)  ,
                                ThemeData::sortThemeSubSets(themeSubSets, "colorset")    , [this] { SetColorSet(mColorSet->getSelected()); });
  mIconSet      = BuildSelector(_("THEME ICONSET"     ), RecalboxConf::Instance().GetThemeIconSet(themeName)   ,
                                ThemeData::sortThemeSubSets(themeSubSets, "iconset")     , [this] { SetIconSet(mIconSet->getSelected()); });
  mMenuSet      = BuildSelector(_("THEME MENU"        ), RecalboxConf::Instance().GetThemeMenuSet(themeName)   ,
                                ThemeData::sortThemeSubSets(themeSubSets, "menu")        , [this] { SetMenuSet(mMenuSet->getSelected()); });
  mSystemView   = BuildSelector(_("THEME SYSTEMVIEW"  ), RecalboxConf::Instance().GetThemeSystemView(themeName),
                                ThemeData::sortThemeSubSets(themeSubSets, "systemview")  , [this] { SetSystemView(mSystemView->getSelected()); });
  mGameListView = BuildSelector(_("THEME GAMELISTVIEW"), RecalboxConf::Instance().GetThemeGamelistView(themeName)  ,
                                ThemeData::sortThemeSubSets(themeSubSets, "gamelistview"), [this] { SetGameListView(mGameListView->getSelected()); });
  mGameClipView = BuildSelector(_("THEME GAMECLIPVIEW"), RecalboxConf::Instance().GetThemeGameClipView(themeName)  ,
                                ThemeData::sortThemeSubSets(themeSubSets, "gameclipview"), [this] { SetGameClipView(mGameClipView->getSelected()); });
  mRegion       = BuildSelector(_("THEME REGION"      ), RecalboxConf::Instance().GetThemeRegion(themeName)    ,
                                ThemeData::sortThemeSubSets(themeSubSets, "region")      , [this] { SetRegion(mRegion->getSelected()); });

  // Empty?
  if (!mColorSet && !mIconSet && !mMenuSet && !mSystemView && !mGameListView && !mRegion)
  {
    Close();
    mWindow.displayMessage(_("THIS THEME HAS NO OPTION"));
  }
}

void GuiMenuThemeConfiguration::SetColorSet(const std::string& colorSet)
{
  RecalboxConf::Instance().SetThemeColorSet(mThemeName, colorSet);
  RecalboxConf::Instance().Save();
  mReloadRequired = true;
}

void GuiMenuThemeConfiguration::SetIconSet(const std::string& iconSet)
{
  RecalboxConf::Instance().SetThemeIconSet(mThemeName, iconSet);
  RecalboxConf::Instance().Save();
  mReloadRequired = true;
}

void GuiMenuThemeConfiguration::SetMenuSet(const std::string& menuSet)
{
  RecalboxConf::Instance().SetThemeMenuSet(mThemeName, menuSet);
  RecalboxConf::Instance().Save();
  mReloadRequired = true;
}

void GuiMenuThemeConfiguration::SetSystemView(const std::string& systemView)
{
  RecalboxConf::Instance().SetThemeSystemView(mThemeName, systemView);
  RecalboxConf::Instance().Save();
  mReloadRequired = true;
}

void GuiMenuThemeConfiguration::SetGameListView(const std::string& gameListView)
{
  RecalboxConf::Instance().SetThemeGamelistView(mThemeName, gameListView);
  RecalboxConf::Instance().Save();
  mReloadRequired = true;
}

void GuiMenuThemeConfiguration::SetGameClipView(const std::string& gameClipView)
{
    RecalboxConf::Instance().SetThemeGameClipView(mThemeName, gameClipView);
    RecalboxConf::Instance().Save();
    mReloadRequired = true;
}

void GuiMenuThemeConfiguration::SetRegion(const std::string& region)
{
  RecalboxConf::Instance().SetThemeRegion(mThemeName, region);
  RecalboxConf::Instance().Save();
  mReloadRequired = true;
}

GuiMenuThemeConfiguration::OptionList GuiMenuThemeConfiguration::BuildSelector(const std::string& label, const std::string& selected, const StringMaps & items, const Callback& callback)
{
  auto selectedColorSet = items.find(selected);
  if (selectedColorSet == items.end()) selectedColorSet = items.begin();

  auto optionList = std::make_shared<OptionListComponent<std::string> >(mWindow, label, false);
  for (const auto& it : items) optionList->add(it.first, it.first, it.first == selectedColorSet->first);
  optionList->setChangedCallback(callback);

  if (!items.empty())
  {
    mMenu.addWithLabel(optionList, label, _(MENUMESSAGE_UI_THEME_COLORSET_MSG));
    return optionList;
  }
  return nullptr;
}

GuiMenuThemeConfiguration::~GuiMenuThemeConfiguration()
{
  if (mReloadRequired)
  {
    ThemeData::SetThemeChanged(true);
    MenuThemeData::Reset();
    ThemeData::SetThemeChanged(true);
    MainRunner::RequestQuit(MainRunner::ExitState::Relaunch, false);
  }
}
