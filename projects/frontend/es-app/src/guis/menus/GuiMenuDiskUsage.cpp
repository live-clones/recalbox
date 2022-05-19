//
// Created by bkg2k on 26/03/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include "GuiMenuDiskUsage.h"

GuiMenuDiskUsage::GuiMenuDiskUsage(WindowManager& window, MountMonitor& mountMonitor)
  : GuiMenuBase(window, _("DISK USAGE (FREE/TOTAL)"), nullptr)
{
  for(DeviceMount* device : mountMonitor.AllMountPoints())
  {
    device->UpdateSize();
    AddText(device->DisplayableDeviceName(), device->DisplayableFreeSpace());
  }
}
