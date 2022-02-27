#include <RecalboxConf.h>
#include "guis/GuiNetPlayHostPasswords.h"

#include <guis/GuiArcadeVirtualKeyboard.h>
#include "components/SwitchComponent.h"
#include <components/OptionListComponent.h>
#include <utils/cplusplus/StaticLifeCycleControler.h>
#include <systems/NetPlayData.h>
#include "utils/locale/LocaleHelper.h"
#include "GuiNetPlayEditPasswords.h"
#include <views/ViewController.h>

GuiNetPlayHostPasswords::GuiNetPlayHostPasswords(WindowManager& window, FileData& game)
  : Gui(window),
    mGame(game),
    mMenu(window, _("GAME PROTECTION"))
{
	addChild(&mMenu);

  auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();

  mPlayerPasswordOnOff = std::make_shared<SwitchComponent>(mWindow, RecalboxConf::Instance().AsBool("netplay.password.useforplayer", false));
  mMenu.addWithLabel(mPlayerPasswordOnOff, _("SET PLAYER PASSWORD"));
  mViewerPasswordOnOff = std::make_shared<SwitchComponent>(mWindow, RecalboxConf::Instance().AsBool("netplay.password.useforviewer", false));
  mMenu.addWithLabel(mViewerPasswordOnOff, _("SET VIEWER PASSWORD"));

  int lastplayer = RecalboxConf::Instance().AsInt("netplay.password.lastforplayer");
  int lastviewer = RecalboxConf::Instance().AsInt("netplay.password.lastforviewer");

  mPlayerPassword = std::make_shared<OptionListComponent<int>>(mWindow, "playerpassword", false, FONT_SIZE_MEDIUM);
  mViewerPassword = std::make_shared<OptionListComponent<int>>(mWindow, "viewerpassword", false, FONT_SIZE_MEDIUM);
  for(int i = 0; i < DefaultPasswords::sPasswordCount; i++)
  {
    std::string password = RecalboxConf::Instance().AsString("netplay.password." + Strings::ToString(i), DefaultPasswords::sDefaultPassword[i]);
    mPlayerPassword->add(password, i, lastplayer == i);
    mViewerPassword->add(password, i, lastviewer == i);
  }
  mMenu.addWithLabel(mPlayerPassword, _("CHOOSE PLAYER PASSWORD"));
  mMenu.addWithLabel(mViewerPassword, _("CHOOSE VIEWER-ONLY PASSWORD"));

	mMenu.addButton(_("START"), "", [this]
	{
	  // Save
    RecalboxConf::Instance().SetBool("netplay.password.useforplayer", mPlayerPasswordOnOff->getState());
    RecalboxConf::Instance().SetBool("netplay.password.useforviewer", mViewerPasswordOnOff->getState());
    RecalboxConf::Instance().SetInt("netplay.password.lastforplayer", mPlayerPassword->getSelected());
    RecalboxConf::Instance().SetInt("netplay.password.lastforviewer", mViewerPassword->getSelected());
    RecalboxConf::Instance().Save();

    // Run game
    if(mGame.IsGame())
    {
      std::string playerPassword, viewerPassword;
      if (mPlayerPasswordOnOff->getState())
        playerPassword = RecalboxConf::Instance().AsString("netplay.password." + Strings::ToString(mPlayerPassword->getSelected()), DefaultPasswords::sDefaultPassword[mPlayerPassword->getSelected()]);
      if (mViewerPasswordOnOff->getState())
        viewerPassword = RecalboxConf::Instance().AsString("netplay.password." + Strings::ToString(mViewerPassword->getSelected()), DefaultPasswords::sDefaultPassword[mPlayerPassword->getSelected()]);

      GameLinkedData data(RecalboxConf::Instance().GetNetplayPort(), playerPassword, viewerPassword);
      ViewController::Instance().Launch(&mGame, data, Vector3f());
    }
    // Close window
    Close();
	});
  mMenu.addButton(_("EDIT PASSWORDS"), "", [this]
  {
    mWindow.pushGui(new GuiNetPlayEditPasswords(mWindow));
  });

	mMenu.setPosition((Renderer::Instance().DisplayWidthAsFloat() - mMenu.getSize().x()) / 2, (Renderer::Instance().DisplayHeightAsFloat() - mMenu.getSize().y()) / 2);
  mMenu.setCursorToButtons();
}

bool GuiNetPlayHostPasswords::ProcessInput(const InputCompactEvent& event)
{
  if (event.CancelPressed())
  {
    Close();
    return true;
  }

  return Component::ProcessInput(event);
}

bool GuiNetPlayHostPasswords::getHelpPrompts(Help& help)
{
	mMenu.getHelpPrompts(help);
	help.Set(Help::Cancel(), _("BACK"));
	return true;
}
