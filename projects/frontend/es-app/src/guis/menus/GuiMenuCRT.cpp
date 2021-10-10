//
// Created by digitalLumberjack on 20/0/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include "GuiMenuCRT.h"
#include <utils/locale/LocaleHelper.h>
#include <guis/MenuMessages.h>
#include <recalbox/RecalboxSystem.h>
#include <systems/SystemManager.h>
#include <components/SwitchComponent.h>

GuiMenuCRT::GuiMenuCRT(WindowManager& window)
  : GuiMenuBase(window, _("CRT SETTINGS"), nullptr)
{
  // Selected Dac
  mDac = AddList<std::string>(_("CRT SYSTEM"), (int)Components::CRTDac, this, GetDacEntries(), _(MENUMESSAGE_ADVANCED_CRT_DAC_HELP_MSG));

  // Resolution
  mEsResolution = AddList<std::string>(_("MENU RESOLUTION"), (int)Components::EsResolution, this, GetEsResolutionEntries(), _(MENUMESSAGE_ADVANCED_CRT_ES_RESOLUTION_HELP_MSG));

  // Force 50HZ
  //mForce50Hz = AddSwitch(_("FORCE 50HZ"), RecalboxConf::Instance().GetSystemCRTForce50HZ(), (int)Components::Force50HZ, this, _(MENUMESSAGE_ADVANCED_CRT_FORCE_50HZ_HELP_MSG));

  mOriginalDac = RecalboxConf::Instance().GetSystemCRT();
  mOriginalEsResolution = RecalboxConf::Instance().GetEmulationstationVideoMode();
  mOriginalForce50Hz = RecalboxConf::Instance().GetSystemCRTForce50HZ();
}

GuiMenuCRT::~GuiMenuCRT()
{
  // Reboot?
  if (mOriginalDac != mDac->getSelected()
  || mOriginalEsResolution != mEsResolution->getSelected()
  || mOriginalForce50Hz != mForce50Hz->getState())
    RequestReboot();
}


std::vector<GuiMenuBase::ListEntry<std::string>> GuiMenuCRT::GetDacEntries()
{
  std::vector<GuiMenuBase::ListEntry<std::string>> list;

  std::string selectedDac = RecalboxConf::Instance().GetSystemCRT();

  list.push_back({ "None", "", selectedDac == "" });
  //list.push_back({ "Recalbox RGB Dual", "recalboxrgbdual", selectedDac == "recalboxrgbdual" });
  list.push_back({ "VGA666", "vga666", selectedDac == "vga666" });
  list.push_back({ "Pi2SCART", "pi2scart", selectedDac == "pi2scart" });
  list.push_back({ "RGBPi", "rgbpi", selectedDac == "rgbpi" });

  return list;
}

std::vector<GuiMenuBase::ListEntry<std::string>> GuiMenuCRT::GetEsResolutionEntries()
{
  std::vector<GuiMenuBase::ListEntry<std::string>> list;

  std::string selectedResolution = RecalboxConf::Instance().GetEmulationstationVideoMode();

  list.push_back({ "240p (compatible mode)", "default", selectedResolution == "default" });
  list.push_back({ "480i (recommended)", "640x480", selectedResolution == "640x480" });
  list.push_back({ "240p", "1920x240", selectedResolution == "1920x240" });

  return list;
}

void GuiMenuCRT::OptionListComponentChanged(int id, int index, const std::string& value)
{
  (void)index;
  if ((Components)id == Components::CRTDac)
  {
    std::string oldValue = RecalboxConf::Instance().GetSystemCRT();
    if(value == ""){
      if(oldValue != "") {
        RecalboxConf::Instance().SetSystemCRT("").Save();
        RecalboxConf::Instance().SetEmulationstationRatio("default").Save();
        RecalboxConf::Instance().SetEmulationstationVideoMode("default").Save();
        mEsResolution->select("default");
      }
    } else {
        RecalboxConf::Instance().SetSystemCRT(value).Save();
    }
  }
  if ((Components)id == Components::EsResolution)
  {
    if(value == "1920x240"){
      RecalboxConf::Instance().SetEmulationstationRatio("1.3334").Save();
    }else {
      RecalboxConf::Instance().SetEmulationstationRatio("default").Save();
    }
    RecalboxConf::Instance().SetEmulationstationVideoMode(value).Save();
  }
  mReboot = true;
}

void GuiMenuCRT::SwitchComponentChanged(int id, bool status)
{
  switch((Components)id)
  {
    case Components::Force50HZ: RecalboxConf::Instance().SetSystemCRTForce50HZ(status).Save(); break;
  }
}
