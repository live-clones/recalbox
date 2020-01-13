#pragma once

#include "components/base/Component.h"
#include "help/HelpStyle.h"

class ComponentGrid;

class HelpComponent : public Component
{
  public:
    explicit HelpComponent(Window&window);

    void Render(const Transform4x4f& parent) override;
    void setOpacity(unsigned char opacity) override;

    void UpdateHelps();

  private:
  	std::shared_ptr<ComponentGrid> mGrid;
};
