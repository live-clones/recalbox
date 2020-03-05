//
// Created by bkg2k on 20/11/2019.
//

#include <Window.h>
#include <guis/Gui.h>

bool Gui::AmIOnTopOfScreen() const
{
  return mWindow.AmIOnTopOfScreen(this);
}

