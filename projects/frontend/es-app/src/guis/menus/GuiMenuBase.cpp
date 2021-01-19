//
// Created by bkg2k on 31/10/2020.
//

#include "GuiMenuBase.h"
#include <input/InputCompactEvent.h>
#include <help/Help.h>
#include <utils/locale/LocaleHelper.h>
#include <WindowManager.h>
#include <EmulationStation.h>
#include <components/SwitchComponent.h>
#include <MainRunner.h>
#include <guis/GuiMsgBox.h>

GuiMenuBase::GuiMenuBase(WindowManager& window, const std::string& title)
  : Gui(window)
  , mMenu(window, title)
  , mTheme(*MenuThemeData::getInstance()->getCurrentTheme())
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

  help.Set(HelpType::UpDown, _("CHOOSE"))
      .Set(HelpType::A, _("BACK"))
      .Set(HelpType::Start, _("CLOSE"));

  return true;
}

void GuiMenuBase::Update(int deltaTime)
{
  if (!mMenuInitialized)
  {
    // Default button
    mMenu.setFooter("RECALBOX VERSION " + Strings::ToUpperASCII(PROGRAM_VERSION_STRING));
    //mMenu.addButton(_("CLOSE"), _("CLOSE"), [this] { Close(); } );
    // Position
    mMenu.setPosition((Renderer::Instance().DisplayWidthAsFloat() - mMenu.getSize().x()) / 2, (Renderer::Instance().DisplayHeightAsFloat() - mMenu.getSize().y()) / 2);

    setSize(Renderer::Instance().DisplayWidthAsFloat(), Renderer::Instance().DisplayHeightAsFloat());

    mMenuInitialized = true;
  }

  Component::Update(deltaTime);
}

void GuiMenuBase::AddSubMenu(const std::string& label, const std::function<void()>& func, const std::string& help)
{
  ComponentListRow row;
  row.makeAcceptInputHandler(func);

  if (!help.empty())
    row.makeHelpInputHandler([this, help, label] { mWindow.pushGui(new GuiMsgBoxScroll(mWindow, label, help, _("OK"), []{}, "", nullptr, "", nullptr)); return true; });

  auto entryMenu = std::make_shared<TextComponent>(mWindow, label, mTheme.menuText.font, mTheme.menuText.color);
  row.addElement(entryMenu, true);
  row.addElement(makeArrow(mWindow), false);
  mMenu.addRowWithHelp(row, label, help);
}

void GuiMenuBase::AddSubMenu(const std::string& label, const Path& iconPath, const std::function<void()>& func,
                             const std::string& help)
{
  ComponentListRow row;
  row.makeAcceptInputHandler(func);

  if (!iconPath.IsEmpty())
  {
    // icon
    auto icon = std::make_shared<ImageComponent>(mWindow);
    icon->setImage(iconPath);
    icon->setColorShift(mTheme.menuText.color);
    icon->setResize(0, mTheme.menuText.font->getLetterHeight() * 1.25f);
    row.addElement(icon, false, true);

    // spacer between icon and text
    auto spacer = std::make_shared<Component>(mWindow);
    spacer->setSize(10.f + Math::round(mTheme.menuText.font->getLetterHeight() * 1.25f) - Math::round(icon->getSize().x()), 0);
    row.addElement(spacer, false, true);
  }

  if (!help.empty())
    row.makeHelpInputHandler([this, help, label] { mWindow.pushGui(new GuiMsgBoxScroll(mWindow, label, help, _("OK"), []{}, "", nullptr, "", nullptr)); return true; });

  auto entryMenu = std::make_shared<TextComponent>(mWindow, label, mTheme.menuText.font, mTheme.menuText.color);
  row.addElement(entryMenu, true);
  row.addElement(makeArrow(mWindow), false);
  mMenu.addRowWithHelp(row, label, help);
}

void GuiMenuBase::AddSubMenu(const std::string& label, const std::function<void()>& func)
{
  AddSubMenu(label, func, Strings::Empty);
}

void GuiMenuBase::AddSubMenu(const std::string& label, const Path& icon, const std::function<void()>& func)
{
  AddSubMenu(label, icon, func, Strings::Empty);
}

std::shared_ptr<SwitchComponent> GuiMenuBase::AddSwitch(const std::string& text, bool value, const std::function<void(bool)>& callback, const std::string& help)
{
  auto result = std::make_shared<SwitchComponent>(mWindow, value);
  result->setChangedCallback(callback);
  mMenu.addWithLabel(result, text, help);
  return result;
}

std::shared_ptr<SwitchComponent> GuiMenuBase::AddSwitch(const std::string& text, bool value, const std::function<void(bool)>& callback)
{
  return AddSwitch(text, value, callback, Strings::Empty);
}

void GuiMenuBase::Reboot()
{
  MainRunner::RequestQuit(MainRunner::ExitState::NormalReboot);
}

void GuiMenuBase::RequestReboot()
{
  mWindow.pushGui(new GuiMsgBox(mWindow, _("THE SYSTEM WILL NOW REBOOT"), _("OK"), Reboot));
}

