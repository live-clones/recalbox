//
// Created by gugue_u on 22/01/2022.
//
#pragma once

#include <guis/menus/GuiMenuBase.h>

class GuiMenuNas : public GuiMenuBase
                  , private IGuiMenuBase
                  , private IEditableComponent
                  , private ISwitchComponent
{
  public:
    explicit GuiMenuNas(WindowManager& window);

  private:

  private:
    enum class Components
    {
      Password
    };

    void SubMenuSelected(int id) override;
    void EditableComponentTextChanged(int id, const std::string& text) override;
    void SwitchComponentChanged(int id, bool status) override;

};
