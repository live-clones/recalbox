//
// Created by bkg2k on 31/10/2020.
//

#include <MainRunner.h>
#include "GuiMenuThemeOptions.h"
#include "guis/MenuMessages.h"

GuiMenuThemeOptions::GuiMenuThemeOptions(Window& window)
  : GuiMenuBase(window, _("THEME"))
  , mReloadRequired(false)
{
  // carousel transition option
  mCarousel = std::make_shared<SwitchComponent>(mWindow, RecalboxConf::Instance().GetThemeCarousel());
  mCarousel->setChangedCallback([this](bool state) { SetCarousel(state); });
  mMenu.addWithLabel(mCarousel, _("CAROUSEL ANIMATION"), _(MENUMESSAGE_UI_CAROUSEL_HELP_MSG));

  // transition style
  std::string currentTransition = RecalboxConf::Instance().GetThemeTransition();
  mTransition = std::make_shared<OptionListComponent<std::string> >(mWindow, _("TRANSITION STYLE"), false);
  mTransition->add("fade", "fade", currentTransition == "fade");
  mTransition->add("slide", "slide", currentTransition == "slide");
  mTransition->add("instant", "instant", currentTransition == "instant");
  mTransition->setChangedCallback([this] { SetTransition(mTransition->getSelected()); });
  mMenu.addWithLabel(mTransition, _("TRANSITION STYLE"), _(MENUMESSAGE_UI_TRANSITION_HELP_MSG));

  // theme set
  auto themeSets = ThemeData::getThemeSets();
  auto selectedSet = themeSets.find(RecalboxConf::Instance().GetThemeFolder());
  if (selectedSet == themeSets.end()) selectedSet = themeSets.begin();

  mTheme = std::make_shared<OptionListComponent<std::string> >(mWindow, _("THEME SET"), false);
  for (const auto& it : themeSets) mTheme->add(it.first, it.first, it.first == selectedSet->first);
  mTheme->setChangedCallback([this] { SetTheme(mTheme->getSelected()); });
  mMenu.addWithLabel(mTheme, _("THEME SET"), _(MENUMESSAGE_UI_THEME_HELP_MSG));
}

GuiMenuThemeOptions::~GuiMenuThemeOptions()
{
  if (mReloadRequired) {
      ThemeData::SetThemeChanged(true);
      MainRunner::RequestQuit(MainRunner::ExitState::Relaunch, false);
  }
}

void GuiMenuThemeOptions::SetCarousel(bool on)
{
  RecalboxConf::Instance().SetThemeCarousel(on);
  RecalboxConf::Instance().Save();
  mReloadRequired = true;
}

void GuiMenuThemeOptions::SetTransition(const std::string& transition)
{
  RecalboxConf::Instance().SetThemeTransition(transition);
  RecalboxConf::Instance().Save();
  mReloadRequired = true;
}

void GuiMenuThemeOptions::SetTheme(const std::string& theme)
{
  RecalboxConf::Instance().SetThemeFolder(theme);
  RecalboxConf::Instance().Save();
  mReloadRequired = true;
}
