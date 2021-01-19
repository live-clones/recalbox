//
// Created by bkg2k on 08/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include <guis/menus/GuiMenuVirtualSystemPerGenre.h>
#include <components/SwitchComponent.h>
#include <systems/SystemManager.h>
#include <utils/locale/LocaleHelper.h>
#include <guis/GuiMsgBox.h>
#include <MainRunner.h>

GuiMenuVirtualSystemPerGenre::GuiMenuVirtualSystemPerGenre(WindowManager& window)
  : GuiMenuBase(window, _("VIRTUAL SYSTEMS PER GENRE"))
{
  // All games
  Genres::GenreMap map = Genres::GetShortNameMap();
  for(const auto & genre : Genres::GetOrderedList())
  {
    std::string shortName = map[genre];
    std::string longName = Genres::GetName(genre);
    std::string prefix = Genres::IsSubGenre(genre) ? "   \u21B3 " : "";
    Path icon = Path(Genres::GetResourcePath(genre));
    bool value = RecalboxConf::Instance().GetCollection(shortName);
    auto component = std::make_shared<SwitchComponent>(mWindow, value);
    mMenu.addWithLabel(component, icon, prefix + _S(longName), _S(longName));
    mComponents[component] = shortName;
  }
}

GuiMenuVirtualSystemPerGenre::~GuiMenuVirtualSystemPerGenre()
{
  bool relaunch = false;
  for(const auto& component : mComponents)
    if (component.first->getState() != RecalboxConf::Instance().GetCollection(component.second))
    {
      RecalboxConf::Instance().SetCollection(component.second, component.first->getState());
      relaunch = true;
    }
  if (relaunch)
  {
    RecalboxConf::Instance().Save();
    mWindow.pushGui(new GuiMsgBox(mWindow, _("EmulationStation must relaunch to apply your changes."), _("OK"), []
    { MainRunner::RequestQuit(MainRunner::ExitState::Relaunch, true); }));
  }
}
