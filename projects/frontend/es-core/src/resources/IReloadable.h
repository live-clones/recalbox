//
// Created by Bkg2k on 25/02/2020.
//
#pragma once

class ResourceManager;

class IReloadable
{
  public:
    virtual void unload(ResourceManager& rm) = 0;
    virtual void reload(ResourceManager& rm) = 0;
};
