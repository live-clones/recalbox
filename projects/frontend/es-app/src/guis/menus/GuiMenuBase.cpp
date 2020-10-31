//
// Created by bkg2k on 31/10/2020.
//

#include "GuiMenuBase.h"
#include <input/InputCompactEvent.h>
#include <help/Help.h>
#include <utils/locale/LocaleHelper.h>
#include <Window.h>

GuiMenuBase::GuiMenuBase(Window& window, const std::string& title)
  : Gui(window)
  , mMenu(window, title)
  , mMenuInitialized(false)
{
  addChild(&mMenu);
}

bool GuiMenuBase::ProcessInput(const InputCompactEvent& event)
{
  if (event.APressed())
  {
    Close();
    return true;
  }

  if (event.StartPressed())
  {
    // Close everything
    mWindow.CloseAll();
    return true;
  }
  return Gui::ProcessInput(event);
}

bool GuiMenuBase::getHelpPrompts(Help& help)
{
  mMenu.getHelpPrompts(help);

  help.Set(HelpType::A, _("BACK"))
      .Set(HelpType::Start, _("CLOSE"));

  return true;
}

void GuiMenuBase::Update(int deltaTime)
{
  if (!mMenuInitialized)
  {
    // Default button
    mMenu.addButton(_("CLOSE"), _("CLOSE"), [this] { Close(); } );
    // Position
    mMenu.setPosition((Renderer::getDisplayWidthAsFloat() - mMenu.getSize().x()) / 2, (Renderer::getDisplayHeightAsFloat() - mMenu.getSize().y()) / 2);

    setSize(Renderer::getDisplayWidthAsFloat(), Renderer::getDisplayHeightAsFloat());

    mMenuInitialized = true;
  }

  Component::Update(deltaTime);
}

void GuiMenuBase::AddSubMenu(const std::string& label, const std::function<void()>& func, const std::string& help)
{
  ComponentListRow row;
  row.makeAcceptInputHandler(func);

  auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
  if (!help.empty())
    row.makeHelpInputHandler([this, help, label] { mWindow.pushGui(new GuiMsgBoxScroll(mWindow, label, help, _("OK"), []{}, "", nullptr, "", nullptr)); return true; });

  auto entryMenu = std::make_shared<TextComponent>(mWindow, label, menuTheme->menuText.font, menuTheme->menuText.color);
  row.addElement(entryMenu, true);
  row.addElement(makeArrow(mWindow), false);
  mMenu.addRowWithHelp(row, label, help);
}
