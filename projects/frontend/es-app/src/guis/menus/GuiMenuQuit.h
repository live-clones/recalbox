//
// Created by thierry.imbert on 12/02/2020.
//
#pragma once

#include <guis/menus/GuiMenuBase.h>

class GuiMenuQuit : public GuiMenuBase
{
  public:
    explicit GuiMenuQuit(WindowManager& window);

    static void PushQuitGui(WindowManager& window);

  private:
    //! Shutdown
    static void Shutdown(GuiMenuQuit* thiz);
    //! Fast Shutdown
    static void FastShutdown(GuiMenuQuit* thiz);
    //! Reboot
    static void Reboot(GuiMenuQuit* thiz);
};
