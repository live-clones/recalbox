//
// Created by bkg2k on 11/08/2019.
//
#pragma once

#include <algorithm>
#include <themes/ThemeData.h>
#include <components/base/Component.h>

class ThemeExtras : public Component
{
  public:
    explicit ThemeExtras(WindowManager& window)
      : Component(window)
    {
    }
    ~ThemeExtras() override;

    // will take ownership of the components within extras (delete them in destructor or when setExtras is called again)
    void setExtras(const std::vector<Component*>& extras);
    inline std::vector<Component*> getmExtras(){return mExtras;}
    inline void sortExtrasByZIndex(){
      std::stable_sort(mExtras.begin(), mExtras.end(),  [](Component* a, Component* b) {
        return b->getZIndex() > a->getZIndex();
      });
    }

  private:
    std::vector<Component*> mExtras;
};
