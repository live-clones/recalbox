//
// Created by gugue_u on 03/01/2021.
//

#pragma once

#include <guis/menus/GuiMenuBase.h>
#include <components/SwitchComponent.h>
#include <systems/SystemManager.h>
#include <scraping/ScraperTools.h>
#include <scraping/ScraperFactory.h>

class GuiMenuCollections : public GuiMenuBase
                               , private IGuiMenuBase
{
  public:
    explicit GuiMenuCollections(WindowManager& window, SystemManager& systemManager);

  private:

    void SubMenuSelected(int id) override;

    SystemManager& mSystemManager;
    std::vector<SmartCollection> existingCollections;


};