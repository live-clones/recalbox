#include <systems/SystemManager.h>
#include <RecalboxConf.h>
#include "guis/GuiMenuArcadeVirtualSystem.h"

#include "components/OptionListComponent.h"
#include "components/SwitchComponent.h"
#include "utils/locale/LocaleHelper.h"
#include "Settings.h"
#include <MainRunner.h>

GuiMenuArcadeVirtualSystem::GuiMenuArcadeVirtualSystem(Window& window, SystemManager& systemManager)
  : Gui(window),
    mSystemManager(systemManager),
    mMenu(window, _("ARCADE VIRTUAL SYSTEM"))
{
	addChild(&mMenu);

  bool arcade = RecalboxConf::Instance().AsBool("emulationstation.arcade", false);
  mArcadeOnOff = std::make_shared<SwitchComponent>(mWindow, arcade);
  mMenu.addWithLabel(mArcadeOnOff, _("ENABLE ARCADE VIRTUAL SYSTEM"));

  bool neogeo = RecalboxConf::Instance().AsBool("emulationstation.arcade.includeneogeo", false);
  mIncludeNeoGeo = std::make_shared<SwitchComponent>(mWindow, neogeo);
  mMenu.addWithLabel(mIncludeNeoGeo, _("INCLUDE NEO-GEO"));

  bool originals = RecalboxConf::Instance().AsBool("emulationstation.arcade.hideoriginals", false);
  mHideOriginals = std::make_shared<SwitchComponent>(mWindow, originals);
  mMenu.addWithLabel(mHideOriginals, _("HIDE ORIGINAL SYSTEMS"));

  int position = RecalboxConf::Instance().AsInt("emulationstation.arcade.position", 0);
  mPosition = std::make_shared< OptionListComponent<int> >(mWindow, _("POSITION"), 0);
  const SystemManager::SystemList& systemList = systemManager.GetVisibleSystemList();
  for(int i = 0; i < (int)systemList.size(); ++i)
  {
    std::string before = ((i == 0) ? systemList[systemList.size() - 1]->getFullName() : systemList[i - 1]->getFullName());
    std::string after = systemList[i]->getFullName();
    std::string displayString = _("BETWEEN %1 AND %2");
    Strings::ReplaceAllIn(displayString, "%1", before);
    Strings::ReplaceAllIn(displayString, "%2", after);
    mPosition->add(displayString, i, i == position);
  }
  mMenu.addWithLabel(mPosition, _("POSITION"));

	mMenu.addButton(_("OK"), "OK", [&]
	{
	  RecalboxConf::Instance().SetBool("emulationstation.arcade", mArcadeOnOff->getState());
    RecalboxConf::Instance().SetBool("emulationstation.arcade.includeneogeo", mIncludeNeoGeo->getState());
    RecalboxConf::Instance().SetBool("emulationstation.arcade.hideoriginals", mHideOriginals->getState());
    RecalboxConf::Instance().SetInt("emulationstation.arcade.position", mPosition->getSelected());

    // If Arcade on/off changed, relaunch
    if (mArcadeOnOff->changed())
      MainRunner::RequestQuit(MainRunner::ExitState::Relaunch, false);
    // If any other property changed, relaunch only if arcade is on
    if ((mIncludeNeoGeo->changed() || mHideOriginals->changed() || mPosition->changed()) && mArcadeOnOff->getState())
      MainRunner::RequestQuit(MainRunner::ExitState::Relaunch, false);

    Close();
	});
	mMenu.setPosition((Renderer::getDisplayWidthAsFloat() - mMenu.getSize().x()) / 2, (Renderer::getDisplayHeightAsFloat() - mMenu.getSize().y()) / 2);
}

bool GuiMenuArcadeVirtualSystem::ProcessInput(const InputCompactEvent& event)
{
  if (event.APressed())
  {
    Close();
    return true;
  }

  return Component::ProcessInput(event);
}

bool GuiMenuArcadeVirtualSystem::getHelpPrompts(Help& help)
{
	mMenu.getHelpPrompts(help);
	help.Set(HelpType::A, _("BACK"));
	return true;
}
