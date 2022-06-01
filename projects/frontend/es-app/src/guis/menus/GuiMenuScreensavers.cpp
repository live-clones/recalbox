//
// Created by bkg2k on 31/10/2020.
//

#include <systems/PlatformId.h>
#include <systems/SystemManager.h>
#include "GuiMenuScreensavers.h"
#include "guis/MenuMessages.h"

GuiMenuScreensavers::GuiMenuScreensavers(WindowManager& window, SystemManager& systemManager)
  : GuiMenuBase(window, _("SCREENSAVER"), nullptr)
  , mSystemManager(systemManager)
{
  // screensaver time
  mTime = AddSlider(_("SCREENSAVER AFTER"), 0.f, 30.f, 1.f, (float)RecalboxConf::Instance().GetScreenSaverTime(), "m", (int)Components::Time, this, _(MENUMESSAGE_UI_SCREENSAVER_AFTER_HELP_MSG));

  // screensaver behavior
  mType = AddList<std::string>(_("SCREENSAVER BEHAVIOR"), (int)Components::Type, this, GetTypeEntries(), _(MENUMESSAGE_UI_SCREENSAVER_BEHAVIOR_HELP_MSG));

  // add systems
  mSystemList = AddMultiList(_("SYSTEMS TO SHOW IN DEMO"), (int)Components::SystemList, this, GetSystemEntries());
}

std::vector<GuiMenuBase::ListEntry<std::string>> GuiMenuScreensavers::GetTypeEntries()
{
  std::vector<ListEntry<std::string>> list;

  std::string type = RecalboxConf::Instance().GetScreenSaverType();
  if (Board::Instance().HasSuspendResume())
    list.push_back({ _("suspend"), "suspend", type == "suspend" });
  list.push_back({ _("dim"), "dim", type == "dim" });
  list.push_back({ _("black"), "black", type == "black" });
  list.push_back({ _("demo"), "demo", type == "demo" });
  list.push_back({ _("gameclip"), "gameclip", type == "gameclip" });

  return list;
}

std::vector<GuiMenuBase::ListEntry<std::string>> GuiMenuScreensavers::GetSystemEntries()
{
  std::vector<ListEntry<std::string>> list;

  for (const SystemData* system : mSystemManager.GetAllSystemList())
    if (system->HasVisibleGame())
      list.push_back({ system->FullName(), system->Name(), RecalboxConf::Instance().IsInScreenSaverSystemList(system->Name()) });

  return list;
}

void GuiMenuScreensavers::SliderMoved(int id, float value)
{
  if ((Components)id == Components::Time)
    RecalboxConf::Instance().SetScreenSaverTime((int)value).Save();
}

void GuiMenuScreensavers::OptionListComponentChanged(int id, int index, const std::string& value)
{
  (void)index;
  if ((Components)id == Components::Type) RecalboxConf::Instance().SetScreenSaverType(value).Save();
}

void GuiMenuScreensavers::OptionListMultiComponentChanged(int id, const std::vector<std::string>& value)
{
  if ((Components)id == Components::SystemList)
  {
    RecalboxConf::Instance().SetScreenSaverSystemList(value).Save();
  }
}

