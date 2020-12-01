//
// Created by bkg2k on 31/10/2020.
//

#include <systems/PlatformId.h>
#include <systems/SystemManager.h>
#include "GuiMenuScreensavers.h"
#include "guis/MenuMessages.h"

GuiMenuScreensavers::GuiMenuScreensavers(Window& window, SystemManager& systemManager)
  : GuiMenuBase(window, _("SCREENSAVER"))
{
  auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();

  // screensaver time
  mTime = std::make_shared<SliderComponent>(mWindow, 0.f, 30.f, 1.f, "m");
  mTime->setSlider((float)RecalboxConf::Instance().GetScreenSaverTime());
  mTime->setSelectedChangedCallback(SetTime);
  mMenu.addWithLabel(mTime, _("SCREENSAVER AFTER"), _(MENUMESSAGE_UI_SCREENSAVER_AFTER_HELP_MSG));

  // screensaver behavior
  mType = std::make_shared<OptionListComponent<std::string> >(mWindow, _("SCREENSAVER BEHAVIOR"), false);
  std::vector<std::string> screensavers;
  std::string type = RecalboxConf::Instance().GetScreenSaverType();
  if (Board::Instance().HasSuspendResume())
    mType->add(_("suspend"), "suspend", type == "suspend");
  mType->add(_("dim"), "dim", type == "dim");
  mType->add(_("black"), "black", type == "black");
  mType->add(_("demo"), "demo", type == "demo");
  mType->add(_("gameclip"), "gameclip", type == "gameclip");
  mType->setChangedCallback([this] { SetType(); });
  mMenu.addWithLabel(mType, _("SCREENSAVER BEHAVIOR"), _(MENUMESSAGE_UI_SCREENSAVER_BEHAVIOR_HELP_MSG));

  // add systems
  mSystemList = std::make_shared<OptionListComponent<std::string> >(mWindow, _("SYSTEMS TO SHOW IN DEMO"), true);
  for (auto* it : systemManager.GetAllSystemList())
    if (!it->hasPlatformId(PlatformIds::PlatformId::PLATFORM_IGNORE))
      mSystemList->add(it->getFullName(), it->getName(), RecalboxConf::Instance().isInList(RecalboxConf::sScreenSaverSystemList, it->getName()));
  mSystemList->setChangedCallback([this] { SetSystemList(); });
  mMenu.addWithLabel(mSystemList, _("SYSTEMS TO SHOW IN DEMO"));
}

void GuiMenuScreensavers::SetTime(float time)
{
  RecalboxConf::Instance().SetScreenSaverTime((int)time);
  RecalboxConf::Instance().Save();
}

void GuiMenuScreensavers::SetType()
{
  RecalboxConf::Instance().SetScreenSaverType(mType->getSelected());
  RecalboxConf::Instance().Save();
}

void GuiMenuScreensavers::SetSystemList()
{
  std::vector<std::string> names = mSystemList->getSelectedObjects();
  RecalboxConf::Instance().SetList(RecalboxConf::sScreenSaverSystemList, names);
  RecalboxConf::Instance().Save();
}

