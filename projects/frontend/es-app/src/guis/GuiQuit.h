//
// Created by thierry.imbert on 12/02/2020.
//
#pragma once

#include "GuiSettings.h"

class GuiQuit : public GuiSettings
{
  private:

  public:
    GuiQuit(Window& window);

    static void PushQuitGui(Window& window);
};
