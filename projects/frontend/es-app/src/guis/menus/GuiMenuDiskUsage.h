//
// Created by bkg2k on 26/03/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <guis/menus/GuiMenuBase.h>
#include "hardware/devices/mount/MountMonitor.h"

class GuiMenuDiskUsage : public GuiMenuBase
{
  public:
    //! Constructor
    GuiMenuDiskUsage(WindowManager& window, MountMonitor& mountMonitor);
};



