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
  : GuiMenuBase(window, _("UPDATES"))
{
  // Enable updates
  mEnableUpdate = std::make_shared<SwitchComponent>(mWindow, RecalboxConf::Instance().GetUpdatesEnabled());
  mEnableUpdate->setChangedCallback(SetEnabled);
  mMenu.addWithLabel(mEnableUpdate, _("CHECK UPDATES"), _(MENUMESSAGE_UPDATE_CHECK_HELP_MSG));

  // Display available update version
  bool update = Upgrade::PendingUpdate();
  mAvailable = std::make_shared<TextComponent>(mWindow, update ? _("YES") : _("NO"), mTheme.menuText.font, mTheme.menuText.color);
  mMenu.addWithLabel(mAvailable, _("AVAILABLE UPDATE"), _(MENUMESSAGE_UPDATE_VERSION_HELP_MSG));

  if (update)
  {
    // Display available update changelog
    AddSubMenu(_("UPDATE CHANGELOG"), [this] { ShowChangelog(); }, _(MENUMESSAGE_UPDATE_CHANGELOG_HELP_MSG));
    // Start update
    AddSubMenu(_("START UPDATE"), [this] { StartUpdate(); }, _(MENUMESSAGE_START_UPDATE_HELP_MSG));
  }

  // Enable updates
  mType = std::make_shared<OptionListComponent<std::string>>(mWindow, _("UPDATE TYPE"), false);
  mType->setChangedCallback([this] { SetUpdateType(mType->getSelected()); });
  std::string updatesType = RecalboxConf::Instance().GetUpdatesType();
  if (updatesType != "stable" && updatesType != "custom") updatesType = "stable";
  mType->add("stable", "stable", updatesType == "stable");
  mType->add("custom", "custom", updatesType != "stable");

  mMenu.addWithLabel(mType, _("UPDATE TYPE"), _(MENUMESSAGE_UPDATE_TYPE_HELP_MSG));
}

void GuiMenuUpdates::SetEnabled(bool on)
{
  RecalboxConf::Instance().SetUpdatesEnabled(on).Save();
}

void GuiMenuUpdates::SetUpdateType(const std::string& type)
{
  RecalboxConf::Instance().SetUpdatesType(type);
}

void GuiMenuUpdates::ShowChangelog()
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

void GuiMenuUpdates::StartUpdate()
{
  mWindow.pushGui(new GuiUpdateRecalbox(mWindow, Upgrade::DownloadUrl(), Upgrade::NewVersion()));
}
