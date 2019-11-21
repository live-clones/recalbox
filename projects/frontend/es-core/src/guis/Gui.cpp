//
// Created by bkg2k on 20/11/2019.
//

#include "Gui.h"
#include "Window.h"

bool Gui::AmIOnTopOfScreen() const
{
  return mWindow.AmIOnTopOfScreen(this);
}

