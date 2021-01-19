//
// Created by bkg2k on 13/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include <guis/menus/GuiMenuPadsPair.h>
#include <recalbox/RecalboxSystem.h>
#include <guis/GuiMsgBox.h>
#include <utils/locale/LocaleHelper.h>

GuiMenuPadsPair::GuiMenuPadsPair(WindowManager& window, const Strings::Vector& deviceList)
  : GuiMenuBase(window, _("PAIR A BLUETOOTH CONTROLLER"))
{
  for (const auto & controllerString : deviceList)
    AddSubMenu(controllerString, std::bind(GuiMenuPadsPair::DeviceSelected, this, controllerString));
}

void GuiMenuPadsPair::DeviceSelected(GuiMenuPadsPair* thiz, const std::string& device)
{
  std::string text = _("PAIRING %s ...");
  Strings::ReplaceAllIn(text, "%s", device);
  thiz->mWindow.pushGui((new GuiWaitLongExecution<std::string, bool>(thiz->mWindow, *thiz))->Execute(device, text));
}

bool GuiMenuPadsPair::Execute(GuiWaitLongExecution<std::string, bool>& from, const std::string& parameter)
{
  (void)from;
  return RecalboxSystem::pairBluetooth(parameter);
}

void GuiMenuPadsPair::Completed(const std::string& parameter, const bool& result)
{
  (void)parameter;
  mWindow.pushGui(new GuiMsgBox(mWindow, result ? _("CONTROLLER PAIRED") : _("UNABLE TO PAIR CONTROLLER"), _("OK")));
}

