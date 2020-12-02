//
// Created by thierry.imbert on 12/02/2020.
//
#pragma once

#include "GuiSettings.h"

class GuiQuit : public GuiSettings
{
  private:

  public:
    explicit GuiQuit(WindowManager& window);

    static void PushQuitGui(WindowManager& window);
};
