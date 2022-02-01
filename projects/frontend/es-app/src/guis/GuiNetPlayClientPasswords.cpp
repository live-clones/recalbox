#include <RecalboxConf.h>
#include "guis/GuiNetPlayClientPasswords.h"

#include "components/SwitchComponent.h"
#include <components/OptionListComponent.h>
#include <utils/cplusplus/StaticLifeCycleControler.h>
#include <systems/NetPlayData.h>
#include "utils/locale/LocaleHelper.h"
#include "GuiNetPlayEditPasswords.h"
#include <views/ViewController.h>

GuiNetPlayClientPasswords::GuiNetPlayClientPasswords(WindowManager& window, LobbyGame& lobby)
  : Gui(window),
    mLobbyGame(lobby),
    mMenu(window, _("JOIN NETPLAY GAME"))
{
	addChild(&mMenu);

  auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();

  mJoinAs = std::make_shared<OptionListComponent<PasswordType>>(mWindow, "passwordtype", false, FONT_SIZE_MEDIUM);
  mJoinAs->add(_("PLAYER"), PasswordType::Player, true);
  mJoinAs->add(_("VIEWER-ONLY"), PasswordType::Viewer, false);
  mMenu.addWithLabel(mJoinAs, _("JOIN AS"));

  mPasswords = std::make_shared<OptionListComponent<int>>(mWindow, "password", false, FONT_SIZE_MEDIUM);
  int passwordIndex = RecalboxConf::Instance().AsInt("netplay.password.client", -1);
  mPasswords->add(_("NONE"), -1, passwordIndex < 0);
  for(int i = 0; i < DefaultPasswords::sPasswordCount; i++)
  {
    std::string password = RecalboxConf::Instance().AsString("netplay.password." + Strings::ToString(i), DefaultPasswords::sDefaultPassword[i]);
    auto passwordComp = std::make_shared<TextComponent>(mWindow, password, menuTheme->menuText.font, menuTheme->menuText.color);
    mPasswords->add(password, i, passwordIndex == i);
  }
  mMenu.addWithLabel(mPasswords, _("CHOOSE PASSWORD"), "");

	mMenu.addButton(_("START"), "", [&]
	{
	  // Save
    RecalboxConf::Instance().SetInt("netplay.password.client", mPasswords->getSelected());
    RecalboxConf::Instance().Save();

    // Run game
    if(mLobbyGame.mGame->IsGame())
    {
      std::string playerPassword, viewerPassword;
      if (mPasswords->getSelected() >= 0)
      {
        if (mJoinAs->getSelected() == PasswordType::Player)
          playerPassword = RecalboxConf::Instance().AsString("netplay.password." + Strings::ToString(mPasswords->getSelected()), DefaultPasswords::sDefaultPassword[mPasswords->getSelected()]);
        else if (mJoinAs->getSelected() == PasswordType::Viewer)
          viewerPassword = RecalboxConf::Instance().AsString("netplay.password." + Strings::ToString(mPasswords->getSelected()), DefaultPasswords::sDefaultPassword[mPasswords->getSelected()]);
      }

      bool mitm = mLobbyGame.mHostMethod == 3;
      std::string& ip = mitm ? mLobbyGame.mMitmIp : mLobbyGame.mIp;
      int port = mitm ? mLobbyGame.mMitmPort : mLobbyGame.mPort;

      GameLinkedData data(mLobbyGame.mCoreShortName, ip, port, playerPassword, viewerPassword, mJoinAs->getSelected() == PasswordType::Viewer);
      ViewController::Instance().Launch(mLobbyGame.mGame, data, Vector3f());
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

bool GuiNetPlayClientPasswords::ProcessInput(const InputCompactEvent& event)
{
  if (event.CancelPressed())
  {
    Close();
    return true;
  }

  return Component::ProcessInput(event);
}

bool GuiNetPlayClientPasswords::getHelpPrompts(Help& help)
{
	mMenu.getHelpPrompts(help);
	help.Set(Help::Cancel(), _("BACK"));
	return true;
}
