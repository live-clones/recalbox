//
// Created by bkg2k on 11/08/2019.
//
#pragma once

#include "ThemeData.h"
#include "GuiComponent.h"

class ThemeExtras : public GuiComponent
{
  public:
    explicit ThemeExtras(Window* window) : GuiComponent(window) {};
    ~ThemeExtras() override;

    // will take ownership of the components within extras (delete them in destructor or when setExtras is called again)
    void setExtras(const std::vector<GuiComponent*>& extras);
    inline std::vector<GuiComponent*> getmExtras(){return mExtras;}
    inline void sortExtrasByZIndex(){
      std::stable_sort(mExtras.begin(), mExtras.end(),  [](GuiComponent* a, GuiComponent* b) {
        return b->getZIndex() > a->getZIndex();
      });
    }

  private:
    std::vector<GuiComponent*> mExtras;
};
