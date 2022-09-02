//
// Created by davidb2111 on 31/08/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include <guis/menus/GuiMenuSoundPair.h>
#include <recalbox/RecalboxSystem.h>
#include <guis/GuiMsgBox.h>
#include <utils/locale/LocaleHelper.h>

GuiMenuSoundPair::GuiMenuSoundPair(WindowManager& window, const Strings::Vector& deviceList)
  : GuiMenuBase(window, _("PAIR A BLUETOOTH AUDIO DEVICE"), this)
  , mDevices(deviceList)
{
  int index = -1;
  for (const auto & controllerString : mDevices)
    AddSubMenu(controllerString, ++index);
}

bool GuiMenuSoundPair::Execute(GuiWaitLongExecution<std::string, bool>& from, const std::string& parameter)
{
  (void)from;
  return RecalboxSystem::pairBluetooth(parameter);
}

void GuiMenuSoundPair::Completed(const std::string& parameter, const bool& result)
{
  (void)parameter;
  mWindow.pushGui(new GuiMsgBox(mWindow, result ? _("AUDIO DEVICE PAIRED") : _("UNABLE TO PAIR AUDIO DEVICE"), _("OK")));
}

void GuiMenuSoundPair::SubMenuSelected(int id)
{
  std::string device = mDevices[id];
  std::string text = _("PAIRING %s ...");
  Strings::ReplaceAllIn(text, "%s", device);
  mWindow.pushGui((new GuiWaitLongExecution<std::string, bool>(mWindow, *this))->Execute(device, text));
}

