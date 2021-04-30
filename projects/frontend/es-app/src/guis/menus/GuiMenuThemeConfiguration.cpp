//
// Created by bkg2k on 31/10/2020.
//

#include <MainRunner.h>
#include "GuiMenuThemeConfiguration.h"
#include "guis/MenuMessages.h"

GuiMenuThemeConfiguration::GuiMenuThemeConfiguration(WindowManager& window, const std::string& themeName)
  : GuiMenuBase(window, _("THEME CONFIGURATION"), nullptr)
  , mThemeName(themeName)
  , mReloadRequired(false)
{
  StringMaps themeSubSets = ThemeData::getThemeSubSets(themeName);
  mColorSet     = BuildSelector(_("THEME COLORSET"    ), _(MENUMESSAGE_UI_THEME_COLORSET_MSG), RecalboxConf::Instance().GetThemeColorSet(themeName)  ,
                                ThemeData::sortThemeSubSets(themeSubSets, "colorset")    , Components::ColorSet, mOriginalColorSet);
  mIconSet      = BuildSelector(_("THEME ICONSET"     ), _(MENUMESSAGE_UI_THEME_ICONSET_MSG), RecalboxConf::Instance().GetThemeIconSet(themeName)   ,
                                ThemeData::sortThemeSubSets(themeSubSets, "iconset")     , Components::IconSet, mOriginalIconSet);
  mMenuSet      = BuildSelector(_("THEME MENU"        ), _(MENUMESSAGE_UI_THEME_MENU_MSG), RecalboxConf::Instance().GetThemeMenuSet(themeName)   ,
                                ThemeData::sortThemeSubSets(themeSubSets, "menu")        , Components::MenuSet, mOriginalMenuSet);
  mSystemView   = BuildSelector(_("THEME SYSTEMVIEW"  ), _(MENUMESSAGE_UI_THEME_SYSTEMVIEW_MSG), RecalboxConf::Instance().GetThemeSystemView(themeName),
                                ThemeData::sortThemeSubSets(themeSubSets, "systemview")  , Components::SystemView, mOriginalSystemView);
  mGameListView = BuildSelector(_("THEME GAMELISTVIEW"), _(MENUMESSAGE_UI_THEME_GAMELISTVIEW_MSG), RecalboxConf::Instance().GetThemeGamelistView(themeName)  ,
                                ThemeData::sortThemeSubSets(themeSubSets, "gamelistview"), Components::GamelistView, mOriginalGameListView);
  mGameClipView = BuildSelector(_("THEME GAMECLIPVIEW"), _(MENUMESSAGE_UI_THEME_GAMECLIPVIEW_MSG), RecalboxConf::Instance().GetThemeGameClipView(themeName)  ,
                                ThemeData::sortThemeSubSets(themeSubSets, "gameclipview"), Components::GameClipView, mOriginalGameClipView);
  mRegion       = BuildSelector(_("THEME REGION"      ), _(MENUMESSAGE_UI_THEME_REGION_MSG), RecalboxConf::Instance().GetThemeRegion(themeName)    ,
                                ThemeData::sortThemeSubSets(themeSubSets, "region")      , Components::Region, mOriginalRegion);

  // Empty?
  if (!mColorSet && !mIconSet && !mMenuSet && !mSystemView && !mGameListView && !mRegion)
  {
    Close();
    mWindow.displayMessage(_("THIS THEME HAS NO OPTION"));
  }
}

GuiMenuThemeConfiguration::~GuiMenuThemeConfiguration()
{
  if ((mColorSet     && (mColorSet->getSelected()     != mOriginalColorSet    )) ||
      (mIconSet      && (mIconSet->getSelected()      != mOriginalIconSet     )) ||
      (mMenuSet      && (mMenuSet->getSelected()      != mOriginalMenuSet     )) ||
      (mSystemView   && (mSystemView->getSelected()   != mOriginalSystemView  )) ||
      (mGameListView && (mGameListView->getSelected() != mOriginalGameListView)) ||
      (mGameClipView && (mGameClipView->getSelected() != mOriginalGameClipView)) ||
      (mRegion       && (mRegion->getSelected()       != mOriginalRegion      )))
  {
    //ThemeData::SetThemeChanged(true);
    //MenuThemeData::Reset();
    ThemeData::SetThemeChanged(true);
    RequestRelaunch();
  }
}

void GuiMenuThemeConfiguration::OptionListComponentChanged(int id, int index, const std::string& value)
{
  (void)index;
  switch((Components)id)
  {
    case Components::ColorSet: RecalboxConf::Instance().SetThemeColorSet(mThemeName, value).Save(); break;
    case Components::IconSet: RecalboxConf::Instance().SetThemeIconSet(mThemeName, value).Save(); break;
    case Components::MenuSet: RecalboxConf::Instance().SetThemeMenuSet(mThemeName, value).Save(); break;
    case Components::SystemView: RecalboxConf::Instance().SetThemeSystemView(mThemeName, value).Save(); break;
    case Components::GamelistView: RecalboxConf::Instance().SetThemeGamelistView(mThemeName, value).Save(); break;
    case Components::GameClipView: RecalboxConf::Instance().SetThemeGameClipView(mThemeName, value).Save(); break;
    case Components::Region: RecalboxConf::Instance().SetThemeRegion(mThemeName, value).Save(); break;
  }
}

GuiMenuThemeConfiguration::OptionList GuiMenuThemeConfiguration::BuildSelector(const std::string& label, const std::string& help, const std::string& selected, const StringMaps & items, Components id, std::string& original)
{
  auto selectedColorSet = items.find(selected);
  if (selectedColorSet == items.end()) selectedColorSet = items.begin();
  if (!items.empty()) original = selectedColorSet->first;

  std::vector<ListEntry<std::string>> list;
  for (const auto& it : items)
    list.push_back({ it.first, it.first, it.first == selectedColorSet->first });

  if (!items.empty())
  {
    auto optionList = AddList(label, (int)id, this, list, help);
    return optionList;
  }
  return nullptr;
}
