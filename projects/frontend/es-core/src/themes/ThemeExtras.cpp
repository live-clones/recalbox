//
// Created by bkg2k on 11/08/2019.
//

#include "ThemeExtras.h"

void ThemeExtras::setExtras(const std::vector<Component*>& extras)
{
  // delete old extras (if any)
  for (auto& mExtra : mExtras)
    delete mExtra;

  mExtras = extras;
  for (auto& mExtra : mExtras)
    addChild(mExtra);
}

ThemeExtras::~ThemeExtras()
{
  for (auto& mExtra : mExtras)
    delete mExtra;
}


