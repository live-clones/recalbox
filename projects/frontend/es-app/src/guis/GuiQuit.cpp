//
// Created by thierry.imbert on 12/02/2020.
//

#include <guis/GuiMsgBox.h>
#include <MainRunner.h>
#include "GuiQuit.h"

GuiQuit::GuiQuit(WindowManager& window)
  : GuiSettings(window, _("QUIT"))
{
  auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
  ComponentListRow row("");

  row.makeAcceptInputHandler([this]
  {
    mWindow.pushGui(new GuiMsgBox(mWindow, _("REALLY SHUTDOWN?"), _("YES"),
                    [] { MainRunner::RequestQuit(MainRunner::ExitState::Shutdown); }, _("NO"), nullptr));
  });
  auto icon2 = std::make_shared<ImageComponent>(mWindow);
  icon2->setImage(menuTheme->menuIconSet.shutdown);
  icon2->setColorShift(menuTheme->menuText.color);
  icon2->setResize(0, menuTheme->menuText.font->getLetterHeight() * 1.25f);
  row.addElement(icon2, false);
  // spacer between icon and text
  auto spacer = std::make_shared<Component>(mWindow);
  row.addElement(spacer, false);
  row.addElement(std::make_shared<TextComponent>(mWindow, _("SHUTDOWN SYSTEM"), menuTheme->menuText.font, menuTheme->menuText.color), true);
  addRow(row);

  row.elements.clear();
  row.makeAcceptInputHandler([this]
  {
    mWindow.pushGui(new GuiMsgBox(mWindow, _("REALLY SHUTDOWN WITHOUT SAVING METADATAS?"), _("YES"),
                    [] { MainRunner::RequestQuit(MainRunner::ExitState::FastShutdown); }, _("NO"), nullptr));
  });
  auto icon3 = std::make_shared<ImageComponent>(mWindow);
  icon3->setImage(menuTheme->menuIconSet.fastshutdown);
  icon3->setColorShift(menuTheme->menuText.color);
  icon3->setResize(0, menuTheme->menuText.font->getLetterHeight() * 1.25f);
  row.addElement(icon3, false);
  // spacer between icon and text
  row.addElement(spacer, false);
  row.addElement(std::make_shared<TextComponent>(mWindow, _("FAST SHUTDOWN SYSTEM"), menuTheme->menuText.font, menuTheme->menuText.color), true);
  addRow(row);

  row.elements.clear();
  row.makeAcceptInputHandler([this]
  {
    mWindow.pushGui(new GuiMsgBox(mWindow, _("REALLY RESTART?"), _("YES"),
                    [] { MainRunner::RequestQuit(MainRunner::ExitState::NormalReboot); }, _("NO"), nullptr));
  });
  // icon
  auto icon1 = std::make_shared<ImageComponent>(mWindow);
  icon1->setImage(menuTheme->menuIconSet.restart);
  icon1->setColorShift(menuTheme->menuText.color);
  icon1->setResize(0, menuTheme->menuText.font->getLetterHeight() * 1.25f);
  row.addElement(icon1, false);

  spacer->setSize(10, 0);
  row.addElement(spacer, false);
  row.addElement(std::make_shared<TextComponent>(mWindow, _("RESTART SYSTEM"), menuTheme->menuText.font, menuTheme->menuText.color), true);
  addRow(row);
}

void GuiQuit::PushQuitGui(WindowManager& window)
{
  window.pushGui(new GuiQuit(window));
}
