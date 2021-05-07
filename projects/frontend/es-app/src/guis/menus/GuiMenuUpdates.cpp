//
// Created by bkg2k on 08/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include <guis/menus/GuiMenuUpdates.h>
#include <components/SwitchComponent.h>
#include <components/OptionListComponent.h>
#include <utils/locale/LocaleHelper.h>
#include <guis/MenuMessages.h>
#include <Upgrade.h>
#include <guis/GuiUpdateRecalbox.h>

GuiMenuUpdates::GuiMenuUpdates(WindowManager& window)
  : GuiMenuBase(window, _("UPDATES"), this)
{
  // Enable updates
  mEnableUpdate = AddSwitch(_("CHECK UPDATES"), RecalboxConf::Instance().GetUpdatesEnabled(), (int)Components::Enable, this, _(MENUMESSAGE_UPDATE_CHECK_HELP_MSG));

  // Display available update version
  bool update = Upgrade::PendingUpdate();
  mAvailable = AddText(_("AVAILABLE UPDATE"), update ? _("YES") : _("NO"), _(MENUMESSAGE_UPDATE_VERSION_HELP_MSG));

  if (update)
  {
    // Display available update changelog
    AddSubMenu(_("UPDATE CHANGELOG"), (int)Components::Changelog, _(MENUMESSAGE_UPDATE_CHANGELOG_HELP_MSG));
    // Start update
    AddSubMenu(_("START UPDATE"), (int)Components::StartUpdate, _(MENUMESSAGE_START_UPDATE_HELP_MSG));
  }

  // Enable updates
  // mType = AddList(_("UPDATE TYPE"), (int)Components::UpdateType, this, GetUpdateTypeEntries(), _(MENUMESSAGE_UPDATE_TYPE_HELP_MSG));
}

std::vector<GuiMenuBase::ListEntry<std::string>> GuiMenuUpdates::GetUpdateTypeEntries()
{
  std::vector<ListEntry<std::string>> list;

  std::string updatesType = RecalboxConf::Instance().GetUpdatesType();
  if (updatesType != "stable" && updatesType != "custom") updatesType = "stable";
  list.push_back({ "stable", "stable", updatesType == "stable" });
  list.push_back({ "custom", "custom", updatesType != "stable" });

  return list;
}

void GuiMenuUpdates::SwitchComponentChanged(int id, bool status)
{
  if ((Components)id == Components::Enable)
    RecalboxConf::Instance().SetUpdatesEnabled(status).Save();
}

void GuiMenuUpdates::SubMenuSelected(int id)
{
  if ((Components)id == Components::Changelog)
  {
    std::string changelog = Upgrade::NewReleaseNote();
    if (!changelog.empty())
    {
      const std::string& message = changelog;
      std::string updateVersion = Upgrade::NewVersion();
      mWindow.displayScrollMessage(_("AN UPDATE IS AVAILABLE FOR YOUR RECALBOX"),
                                   _("NEW VERSION:") + ' ' + updateVersion + "\n" +
                                   _("UPDATE CHANGELOG:") + "\n" + message);
    }
    else
      mWindow.displayMessage(_("AN UPDATE IS AVAILABLE FOR YOUR RECALBOX"));
  }
  else if ((Components)id == Components::StartUpdate)
  {
    mWindow.pushGui(new GuiUpdateRecalbox(mWindow, Upgrade::ImageUrl(), Upgrade::HashUrl(), Upgrade::NewVersion()));
  }
}

void GuiMenuUpdates::OptionListComponentChanged(int id, int index, const std::string& value)
{
  (void)index;
  if ((Components)id == Components::UpdateType)
    RecalboxConf::Instance().SetUpdatesType(value);
}
