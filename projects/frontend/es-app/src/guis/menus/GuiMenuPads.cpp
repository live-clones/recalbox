//
// Created by bkg2k on 12/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include "GuiMenuPads.h"
#include "GuiMenuPadsPair.h"
#include <guis/MenuMessages.h>
#include <utils/locale/LocaleHelper.h>
#include <components/OptionListComponent.h>
#include <components/SwitchComponent.h>
#include <utils/Files.h>
#include <guis/GuiMsgBox.h>
#include <recalbox/RecalboxSystem.h>
#include <guis/GuiDetectDevice.h>

GuiMenuPads::GuiMenuPads(WindowManager& window)
  : GuiMenuBase(window, _("CONTROLLERS SETTINGS"))
  , mMapper(this)
  , mRefreshing(false)
{
  // Configure a pad
  AddSubMenu(_("CONFIGURE A CONTROLLER"), std::bind(GuiMenuPads::StartConfiguring, this), _(MENUMESSAGE_CONTROLLER_CONF_HELP_MSG));

  // Pair a pad
  AddSubMenu(_("PAIR A BLUETOOTH CONTROLLER"), std::bind(GuiMenuPads::StartScanningDevices, this), _(MENUMESSAGE_CONTROLLER_BT_HELP_MSG));

  // Unpair all pads
  AddSubMenu(_("FORGET BLUETOOTH CONTROLLERS"), std::bind(GuiMenuPads::UnpairAll, this), _(MENUMESSAGE_CONTROLLER_FORGET_HELP_MSG));

  // Pad list
  for(int i = 0; i < Input::sMaxInputDevices; ++i)
  {
    static const char* balls[10] ={ "\u2776", "\u2777", "\u2778", "\u2779", "\u277a", "\u277b", "\u277c", "\u277d", "\u277e", "\u277f" };
    std::string name(balls[i]);
    name.append(1, ' ').append(_("INPUT P%i"));
    Strings::ReplaceAllIn(name, "%i", Strings::ToString(i + 1));
    mDevices[i] = std::make_shared<OptionListComponent<int>>(window, name, false);
    mDevices[i]->setChangedCallback(std::bind(GuiMenuPads::ChangePadAt, this, i));
    mMenu.addWithLabel(mDevices[i], name);
  }

  // Process & refresh device selector components
  RefreshDevices(this);
}

void GuiMenuPads::PadsAddedOrRemoved()
{
  RefreshDevices(this);
}

void GuiMenuPads::StartConfiguring(GuiMenuPads* thiz)
{
  Gui* msgBox = new GuiMsgBox(thiz->mWindow,
                              _("YOU ARE GOING TO CONFIGURE A CONTROLLER. IF YOU HAVE ONLY ONE JOYSTICK, "
                                "CONFIGURE THE DIRECTIONS KEYS AND SKIP JOYSTICK CONFIG BY HOLDING A BUTTON. "
                                "IF YOU DO NOT HAVE A SPECIAL KEY FOR HOTKEY, CHOOSE THE SELECT BUTTON. SKIP "
                                "ALL BUTTONS YOU DO NOT HAVE BY HOLDING A KEY. BUTTONS NAMES ARE BASED ON THE "
                                "SNES CONTROLLER."), _("OK"), std::bind(GuiMenuPads::RunDeviceDetection, thiz));
  thiz->mWindow.pushGui(msgBox);
}

void GuiMenuPads::RunDeviceDetection(GuiMenuPads* thiz)
{
  thiz->mWindow.pushGui(new GuiDetectDevice(thiz->mWindow, false, std::bind(GuiMenuPads::RefreshDevices, thiz)));
}

Strings::Vector GuiMenuPads::Execute(GuiWaitLongExecution<bool, Strings::Vector>&, const bool&)
{
  return RecalboxSystem::scanBluetooth();
}

void GuiMenuPads::Completed(const bool&, const Strings::Vector& result)
{
  mWindow.pushGui(result.empty()
                  ? (Gui*)new GuiMsgBox(mWindow, _("NO CONTROLLERS FOUND"), _("OK"))
                  : (Gui*)new GuiMenuPadsPair(mWindow, result));
}

void GuiMenuPads::StartScanningDevices(GuiMenuPads* thiz)
{
  thiz->mWindow.pushGui((new GuiWaitLongExecution<bool, Strings::Vector>(thiz->mWindow, *thiz))->Execute(false, _("SCANNING BLUETOOTH DEVICES...")));
}

void GuiMenuPads::UnpairAll(GuiMenuPads* thiz)
{
  RecalboxSystem::forgetBluetoothControllers();
  thiz->mWindow.pushGui(new GuiMsgBox(thiz->mWindow, _("CONTROLLERS LINKS HAVE BEEN DELETED."), _("OK")));
}

void GuiMenuPads::RefreshDevices(GuiMenuPads* thiz)
{
  thiz->mRefreshing = true;
  // Finaly fill in all components
  for(int i = 0; i < Input::sMaxInputDevices; ++i)
  {
    const InputMapper::Pad& pad = thiz->mMapper.PadAt(i);

    // Add none element
    thiz->mDevices[i]->clear();
    if (pad.IsValid())
      for(int j = 0; j < Input::sMaxInputDevices; ++j)
      {
        const InputMapper::Pad& displayablePad = thiz->mMapper.PadAt(j);
        if (displayablePad.IsValid())
          thiz->mDevices[i]->add(thiz->mMapper.GetDecoratedName(j), j, i == j);
      }
    else
      thiz->mDevices[i]->add(_("NONE"), -1, true);
  }
  thiz->mRefreshing = false;
}

void GuiMenuPads::ChangePadAt(GuiMenuPads* thiz, int index)
{
  if (thiz->mRefreshing) return;

  int newIndex = thiz->mDevices[index]->getSelected();
  if (newIndex < 0) return; // Ignore user playing with NONE :)

  // Swap both pads
  thiz->mMapper.Swap(index, newIndex);
  RefreshDevices(thiz);
}
