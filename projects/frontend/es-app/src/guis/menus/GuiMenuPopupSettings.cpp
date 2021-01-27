//
// Created by bkg2k on 31/10/2020.
//

#include <utils/locale/LocaleHelper.h>
#include "GuiMenuPopupSettings.h"
#include "guis/MenuMessages.h"

GuiMenuPopupSettings::GuiMenuPopupSettings(WindowManager& window)
  : GuiMenuBase(window, _("POPUP SETTINGS"), nullptr)
{
  //help popup time 0=no popup
  mHelp = AddSlider(_("HELP POPUP DURATION"), 0.f, 10.f, 1.f, (float)RecalboxConf::Instance().GetPopupHelp(), "s", (int)Components::Help, this, _(MENUMESSAGE_UI_HELP_POPUP_DURATION_HELP_MSG));

  //music popup time 0=no popup
  mMusic = AddSlider(_("MUSIC POPUP DURATION"), 0.f, 10.f, 1.f, (float)RecalboxConf::Instance().GetPopupMusic(), "s", (int)Components::Music, this, _(MENUMESSAGE_UI_MUSIC_POPUP_DURATION_HELP_MSG));

  //netplay popup time 0=no popup
  mNetplay = AddSlider(_("NETPLAY POPUP DURATION"), 0.f, 10.f, 1.f, (float)RecalboxConf::Instance().GetPopupNetplay(), "s", (int)Components::Netplay, this, _(MENUMESSAGE_UI_NETPLAY_POPUP_DURATION_HELP_MSG));
}

void GuiMenuPopupSettings::SliderMoved(int id, float value)
{
  switch((Components)id)
  {
    case Components::Help: if ((int)value != RecalboxConf::Instance().GetPopupHelp()) RecalboxConf::Instance().SetPopupHelp((int)value).Save(); break;
    case Components::Music: if ((int)value != RecalboxConf::Instance().GetPopupMusic()) RecalboxConf::Instance().SetPopupMusic((int)value).Save(); break;
    case Components::Netplay: if ((int)value != RecalboxConf::Instance().GetPopupNetplay()) RecalboxConf::Instance().SetPopupNetplay((int)value).Save();break;
  }
}

