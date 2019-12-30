//
// Created by bkg2k on 29/12/2019.
//
#pragma once

#include <guis/Gui.h>
#include <components/MenuComponent.h>
#include <components/OptionListComponent.h>

class SystemManager;
class SwitchComponent;


class GuiScraperSelect : public Gui
{
  public:
    explicit GuiScraperSelect(Window& window, SystemManager& systemManager);

  private:
    //! SystemManager instance
    SystemManager& mSystemManager;

    std::shared_ptr<OptionListComponent<std::string>>     mScrapers;

    MenuComponent mMenu;
};
