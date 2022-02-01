//
// Created by gugue_u on 22/01/2022.
//

#include <guis/menus/GuiMenuNas.h>
#include <guis/GuiMsgBox.h>
#include <utils/locale/LocaleHelper.h>


GuiMenuNas::GuiMenuNas(WindowManager& window)
  : GuiMenuBase(window, _("retroNAS SETTINGS"), this)
{
    AddSwitch(_("Activate retroNAS"), RecalboxConf::Instance().GetFavoritesOnly(), 0, this, "");

  AddEditable(_("RetroNAS hostname"), "retronas", 0, this, false);

  AddEditable(_("username"), "recalbox", 0, this, false);

  AddEditable(_("PASSWORD"), "recalbox", 0, this, true);

  mMenu.addButton(_("CONNECT"), "connect", [this] { Close(); });

}

void GuiMenuNas::EditableComponentTextChanged(int id, const std::string& text)
{}

void GuiMenuNas::SwitchComponentChanged(int id, bool status)
{}

void GuiMenuNas::SubMenuSelected(int id)
{}
