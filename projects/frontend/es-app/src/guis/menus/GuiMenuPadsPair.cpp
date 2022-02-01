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
  : GuiMenuBase(window, _("PAIR A BLUETOOTH CONTROLLER"), this)
  , mDevices(deviceList)
{
  int index = -1;
  for (const auto & controllerString : mDevices)
    AddSubMenu(controllerString, ++index);
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

void GuiMenuPadsPair::SubMenuSelected(int id)
{
  std::string device = mDevices[id];
  std::string text = _("PAIRING %s ...");
  Strings::ReplaceAllIn(text, "%s", device);
  mWindow.pushGui((new GuiWaitLongExecution<std::string, bool>(mWindow, *this))->Execute(device, text));
}

