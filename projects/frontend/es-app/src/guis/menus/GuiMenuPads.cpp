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
#include <utils/Files.h>
#include <guis/GuiMsgBox.h>
#include <recalbox/RecalboxSystem.h>
#include <guis/GuiDetectDevice.h>

GuiMenuPads::GuiMenuPads(WindowManager& window)
  : GuiMenuBase(window, _("CONTROLLERS SETTINGS"), this)
  , mMapper(this)
  , mRefreshing(false)
{
  // Configure a pad
  AddSubMenu(_("CONFIGURE A CONTROLLER"), (int)Components::Configure, _(MENUMESSAGE_CONTROLLER_CONF_HELP_MSG));

  // Pair a pad
  AddSubMenu(_("PAIR A BLUETOOTH CONTROLLER"), (int)Components::Pair, _(MENUMESSAGE_CONTROLLER_BT_HELP_MSG));

  // Unpair all pads
  AddSubMenu(_("FORGET BLUETOOTH CONTROLLERS"), (int)Components::Unpair, _(MENUMESSAGE_CONTROLLER_FORGET_HELP_MSG));

  // Driver
  AddList<std::string>(_("DRIVER"), (int)Components::Driver, this, GetModes(), _(MENUMESSAGE_CONTROLLER_DRIVER_HELP_MSG));

  // Pad list
  for(int i = 0; i < Input::sMaxInputDevices; ++i)
  {
    static const char* balls[10] ={ "\u2776", "\u2777", "\u2778", "\u2779", "\u277a", "\u277b", "\u277c", "\u277d", "\u277e", "\u277f" };
    std::string name(balls[i]);
    name.append(1, ' ').append(_("INPUT P%i"));
    Strings::ReplaceAllIn(name, "%i", Strings::ToString(i + 1));
    mDevices[i] = AddList<int>(name, (int)Components::Pads + i, this);
  }

  // Process & refresh device selector components
  RefreshDevices();
}

std::vector<GuiMenuBase::ListEntry<std::string>> GuiMenuPads::GetModes()
{
  std::vector<ListEntry<std::string>> list;

  std::string mode = RecalboxConf::Instance().GetGlobalInputDriver();
  if ((mode != "udev") && (mode != "sdl2")) mode = "auto";

  list.push_back({ _("AUTOMATIC"), "auto", mode == "auto" });
  list.push_back({ _("SYSTEM DRIVER"), "udev", mode == "udev" });
  list.push_back({ _("GAME LIBRARY DRIVER"), "sdl2", mode == "sdl2" });

  return list;
}

void GuiMenuPads::PadsAddedOrRemoved()
{
  RefreshDevices();
}

void GuiMenuPads::StartConfiguring()
{
  Gui* msgBox = new GuiMsgBox(mWindow,
                              _("YOU ARE GOING TO CONFIGURE A CONTROLLER. IF YOU HAVE ONLY ONE JOYSTICK, "
                                "CONFIGURE THE DIRECTIONS KEYS AND SKIP JOYSTICK CONFIG BY HOLDING A BUTTON. "
                                "IF YOU DO NOT HAVE A SPECIAL KEY FOR HOTKEY, CHOOSE THE SELECT BUTTON. SKIP "
                                "ALL BUTTONS YOU DO NOT HAVE BY HOLDING A KEY. BUTTONS NAMES ARE BASED ON THE "
                                "SNES CONTROLLER."), _("OK"), std::bind(GuiMenuPads::RunDeviceDetection, this));
  mWindow.pushGui(msgBox);
}

void GuiMenuPads::RunDeviceDetection(GuiMenuPads* thiz)
{
  thiz->mWindow.pushGui(new GuiDetectDevice(thiz->mWindow, false, [thiz] { thiz->RefreshDevices(); }));
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

void GuiMenuPads::StartScanningDevices()
{
  mWindow.pushGui((new GuiWaitLongExecution<bool, Strings::Vector>(mWindow, *this))->Execute(false, _("SCANNING BLUETOOTH DEVICES...")));
}

void GuiMenuPads::UnpairAll()
{
  RecalboxSystem::forgetBluetoothControllers();
  mWindow.pushGui(new GuiMsgBox(mWindow, _("CONTROLLERS LINKS HAVE BEEN DELETED."), _("OK")));
}

void GuiMenuPads::RefreshDevices()
{
  mRefreshing = true;
  // Finaly fill in all components
  for(int i = 0; i < Input::sMaxInputDevices; ++i)
  {
    const InputMapper::Pad& pad = mMapper.PadAt(i);

    // Add none element
    mDevices[i]->clear();
    if (pad.IsValid())
      for(int j = 0; j < Input::sMaxInputDevices; ++j)
      {
        const InputMapper::Pad& displayablePad = mMapper.PadAt(j);
        if (displayablePad.IsValid())
          mDevices[i]->add(mMapper.GetDecoratedName(j), j, i == j);
      }
    else
      mDevices[i]->add(_("NONE"), -1, true);
  }
  mRefreshing = false;
}

void GuiMenuPads::SubMenuSelected(int id)
{
  switch((Components)id)
  {
    case Components::Configure: StartConfiguring(); break;
    case Components::Pair: StartScanningDevices(); break;
    case Components::Unpair: UnpairAll(); break;
    case Components::Pads:
    case Components::Driver:break;
  }
}

void GuiMenuPads::OptionListComponentChanged(int id, int, const int&)
{
  if (mRefreshing) return;

  int newIndex = mDevices[id]->getSelected();
  if (newIndex < 0) return; // Ignore user playing with NONE :)

  // Swap both pads
  mMapper.Swap(id, newIndex);
  RefreshDevices();
}

void GuiMenuPads::OptionListComponentChanged(int id, int index, const std::string& value)
{
  (void)index;
  if ((Components)id == Components::Driver)
    RecalboxConf::Instance().SetGlobalInputDriver(value).Save();
}
