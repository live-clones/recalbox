//
// Created by bkg2k on 31/10/2020.
//

#include <MainRunner.h>
#include "GuiMenuThemeOptions.h"
#include "guis/MenuMessages.h"

GuiMenuThemeOptions::GuiMenuThemeOptions(WindowManager& window)
  : GuiMenuBase(window, _("THEME"), nullptr)
{
  // carousel transition option
  mCarousel = AddSwitch(_("CAROUSEL ANIMATION"), mOriginalCaroussel = RecalboxConf::Instance().GetThemeCarousel(), (int)Components::Carousel, this, _(MENUMESSAGE_UI_CAROUSEL_HELP_MSG));

  // transition style
  mTransition = AddList(_("TRANSITION STYLE"), (int)Components::Transition, this, GetTransitionEntries(), _(MENUMESSAGE_UI_TRANSITION_HELP_MSG));

  // theme set
  mTheme = AddList(_("THEME SET"), (int)Components::Theme, this, GetThemeEntries(), _(MENUMESSAGE_UI_THEME_HELP_MSG));
}

GuiMenuThemeOptions::~GuiMenuThemeOptions()
{
  if ((mCarousel->getState() != mOriginalCaroussel) ||
      (mTransition->getSelected() != mOriginalTransition) ||
      (mTheme->getSelected() != mOriginalTheme))
  {
    ThemeData::SetThemeChanged(true);
    RequestRelaunch();
  }
}

std::vector<GuiMenuBase::ListEntry<std::string>> GuiMenuThemeOptions::GetTransitionEntries()
{
  std::vector<ListEntry<std::string>> list;

  mOriginalTransition = RecalboxConf::Instance().GetThemeTransition();
  list.push_back({ "fade", "fade", mOriginalTransition == "fade" });
  list.push_back({ "slide", "slide", mOriginalTransition == "slide" });
  list.push_back({ "instant", "instant", mOriginalTransition == "instant" });

  return list;
}

std::vector<GuiMenuBase::ListEntry<std::string>> GuiMenuThemeOptions::GetThemeEntries()
{
  std::vector<ListEntry<std::string>> list;

  auto themeSets = ThemeData::getThemeSets();
  auto selectedSet = themeSets.find(RecalboxConf::Instance().GetThemeFolder());
  if (selectedSet == themeSets.end()) selectedSet = themeSets.begin();
  mOriginalTheme = selectedSet->first;
  for (const auto& it : themeSets)
    list.push_back({ it.first, it.first, it.first == mOriginalTheme });

  return list;
}

void GuiMenuThemeOptions::OptionListComponentChanged(int id, int index, const std::string& value)
{
  (void)index;
  if ((Components)id == Components::Transition) RecalboxConf::Instance().SetThemeTransition(value).Save();
  else if ((Components)id == Components::Theme) RecalboxConf::Instance().SetThemeFolder(value).Save();
}

void GuiMenuThemeOptions::SwitchComponentChanged(int id, bool status)
{
  if ((Components)id == Components::Carousel)
    RecalboxConf::Instance().SetThemeCarousel(status).Save();
}
