//
// Created by digitalLumberjack on 20/0/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include "GuiMenuCRT.h"
#include "views/ViewController.h"
#include "guis/GuiMsgBox.h"
#include <utils/locale/LocaleHelper.h>
#include <guis/MenuMessages.h>
#include <recalbox/RecalboxSystem.h>
#include <components/SwitchComponent.h>
#include <hardware/crt/CrtAdapterDetector.h>
#include <CrtConf.h>

GuiMenuCRT::GuiMenuCRT(WindowManager& window)
  : GuiMenuBase(window, _("CRT SETTINGS"), this)
{
  bool isRGBDual = Board::Instance().CrtBoard().GetCrtAdapter() == CrtAdapterType::RGBDual;
  bool is31kHz = Board::Instance().CrtBoard().GetHorizontalFrequency() == ICrtInterface::HorizontalFrequency::KHz31;
  // If we run on Recalbox RGB Dual, we ignore the recalbox.conf configuration
  mOriginalDac = isRGBDual ? CrtAdapterType::RGBDual : CrtConf::Instance().GetSystemCRT();
  // Selected Dac
  mDac = AddList<CrtAdapterType>(_("CRT ADAPTER"), (int)Components::CRTDac, this, GetDacEntries(isRGBDual), _(MENUMESSAGE_ADVANCED_CRT_DAC_HELP_MSG));

  // Resolution
  mOriginalEsResolution = is31kHz ? CrtConf::Instance().GetSystemCRT31kHzResolution() : CrtConf::Instance().GetSystemCRTResolution();
  mEsResolution = AddList<std::string>(_("MENU RESOLUTION"), (int)Components::EsResolution, this, GetEsResolutionEntries(is31kHz), _(MENUMESSAGE_ADVANCED_CRT_ES_RESOLUTION_HELP_MSG));

  // Horizontal output frequency
  if (Board::Instance().CrtBoard().Has31KhzSupport()) AddText(_("SCREEN TYPE"), GetHorizontalFrequency());

  // Force 50HZ
  if (Board::Instance().CrtBoard().HasForced50hzSupport()) AddText(_("FORCE 50HZ"), Get50hz());

  // Force HDMI
  mOriginalForceHDMI = CrtConf::Instance().GetSystemCRTForceHDMI();
  mForceHDMI = mOriginalForceHDMI;
  if(isRGBDual)
    AddSwitch(_("PRIORITY TO HDMI"), mOriginalForceHDMI, (int)Components::ForceHDMI, this, _(MENUMESSAGE_ADVANCED_CRT_FORCE_HDMI_HELP_MSG));

  // Game Region selection
  AddSwitch(_("SELECT GAME REFRESH RATE AT LAUNCH"), CrtConf::Instance().GetSystemCRTGameRegionSelect(), (int)Components::GameRegion, this, _(MENUMESSAGE_ADVANCED_CRT_GAME_REGION_HELP_MSG));

  // Game Resolution selection
  AddSwitch(_("SELECT GAME RESOLUTION AT LAUNCH"), CrtConf::Instance().GetSystemCRTGameResolutionSelect(), (int)Components::GameResolution, this, _(MENUMESSAGE_ADVANCED_CRT_GAME_RESOLUTION_HELP_MSG));

  if(is31kHz)
  {
    // Demo Game Resolution on 31khz
    AddSwitch(_("RUN DEMOS IN 240P@120"), CrtConf::Instance().GetSystemCRTRunDemoIn240pOn31kHz(), (int)Components::DemoIn240pOn31kHz, this, _(MENUMESSAGE_ADVANCED_CRT_DEMO_RESOLUTION_ON_31KHZ_HELP_MSG));

    // Scanlines on 31kHz resolution
    AddSwitch(_("SCANLINES IN 480P"), CrtConf::Instance().GetSystemCRTScanlines31kHz(), (int)Components::ScanlinesOn31kHz, this, _(MENUMESSAGE_ADVANCED_CRT_DEMO_RESOLUTION_ON_31KHZ_HELP_MSG));
  }

  // Zero Lag
  AddSwitch(_("ZERO LAG (BETA)"), RecalboxConf::Instance().GetGlobalZeroLag(), (int)Components::ZeroLag, this, _(MENUMESSAGE_ADVANCED_CRT_ZERO_LAG_HELP_MSG));

  // Force Jack
  mOriginalForceJack = CrtConf::Instance().GetSystemCRTForceJack();
  mForceJack = mOriginalForceJack;
  if(isRGBDual)
    AddSwitch(_("FORCE SOUND ON JACK"), mOriginalForceJack, (int)Components::ForceJack, this, _(MENUMESSAGE_ADVANCED_CRT_FORCE_JACK_HELP_MSG));

  // Screen Adjustments
  AddSubMenu(_("SCREEN CALIBRATION (BETA)"), (int)Components::Adjustment);

  /*if(!Board::Instance().CrtBoard().MustForce50Hz())
  {
    // PAL offsets until when have a PAL calibration screen
    // Only displayed when we are in 60Hz, the calibration screen set thoses values if we are in 50Hz
    AddSlider(_("PAL HORIZONTAL OFFSET"), -30, 30, 1, CrtConf::Instance().GetSystemCRTHorizontalPALOffset(), ".0", (int)Components::HorizontalPalOffset, this, _(MENUMESSAGE_ADVANCED_CRT_HORIZONTAL_PAL_OFFSET_HELP_MSG));
    AddSlider(_("PAL VERTICAL OFFSET"), -10, 10, 1, CrtConf::Instance().GetSystemCRTVerticalPALOffset(), ".0", (int)Components::VerticalPalOffset, this, _(MENUMESSAGE_ADVANCED_CRT_VERTICAL_PAL_OFFSET_HELP_MSG));
  }
  }*/

}

GuiMenuCRT::~GuiMenuCRT()
{
  // Reboot?
  if (mOriginalDac != mDac->getSelected() || mOriginalEsResolution != mEsResolution->getSelected() ||
      mOriginalForceJack != mForceJack || mOriginalForceHDMI != mForceHDMI)
    RequestReboot();
}

std::string GuiMenuCRT::Get50hz()
{
  std::string result = Board::Instance().CrtBoard().MustForce50Hz() ? _("ON") : _("OFF");
  result.append(1, ' ').append(_("(Hardware managed)"));
  return result;
}

std::string GuiMenuCRT::GetHorizontalFrequency()
{
  std::string result = "15khz";
  switch(Board::Instance().CrtBoard().GetHorizontalFrequency())
  {
    case ICrtInterface::HorizontalFrequency::KHz31: result = "31khz";
    case ICrtInterface::HorizontalFrequency::KHz15:
    default: break;
  }
  if (Board::Instance().CrtBoard().Has31KhzSupport())
    result.append(1, ' ').append(_("(Hardware managed)"));

  return result;
}

std::vector<GuiMenuBase::ListEntry<CrtAdapterType>> GuiMenuCRT::GetDacEntries(bool onlyRgbDual)
{
  std::vector<GuiMenuBase::ListEntry<CrtAdapterType>> list;
  if(onlyRgbDual){
    list.push_back( { "Recalbox RGB Dual", CrtAdapterType::RGBDual, true } );
    return list;
  }

  CrtAdapterType selectedDac = CrtConf::Instance().GetSystemCRT();

  static struct
  {
    const char* Name;
    CrtAdapterType Type;
  }
  Adapters[] =
  {
    { "Recalbox RGB Dual", CrtAdapterType::RGBDual },
    { "VGA666", CrtAdapterType::Vga666 },
    { "RGBPi", CrtAdapterType::RGBPi },
    { "Pi2SCART", CrtAdapterType::Pi2Scart },
  };

  // Always push none
  list.push_back( { "NONE", CrtAdapterType::None, selectedDac == CrtAdapterType::None } );
  // Push all adapters or only one if it is automatically detected
  const ICrtInterface& crt = Board::Instance().CrtBoard();
  for(const auto& item : Adapters)
  {
    if (!crt.HasBeenAutomaticallyDetected() || crt.GetCrtAdapter() == item.Type)
      list.push_back( { item.Name, item.Type, selectedDac == item.Type } );
  }

  return list;
}

std::vector<GuiMenuBase::ListEntry<std::string>> GuiMenuCRT::GetEsResolutionEntries(bool only31kHz)
{
  std::vector<GuiMenuBase::ListEntry<std::string>> list;

  if(only31kHz)
  {
    bool is480 = CrtConf::Instance().GetSystemCRT31kHzResolution() == "480";
    list.push_back({ "480p", "480", is480 });
    list.push_back({ "240p@120Hz", "240", !is480 });
    return list;
  } else {
    bool rdef = CrtConf::Instance().GetSystemCRTResolution() == "240";
    list.push_back({ "240p", "240", rdef });
    list.push_back({ "480i", "480", !rdef });
  }

  return list;
}


void GuiMenuCRT::OptionListComponentChanged(int id, int index, const CrtAdapterType& value)
{
  (void)index;
  if ((Components)id == Components::CRTDac)
  {
    CrtAdapterType oldValue = Board::Instance().CrtBoard().GetCrtAdapter();
    if (value == CrtAdapterType::None)
    {
      if (oldValue != CrtAdapterType::None)
      {
        CrtConf::Instance().SetSystemCRT(CrtAdapterType::None).Save();
        RecalboxConf::Instance().SetEmulationstationVideoMode("default").Save();
        mEsResolution->select("default");
      }
    }
    CrtConf::Instance().SetSystemCRT(value).Save();
  }
}

void GuiMenuCRT::OptionListComponentChanged(int id, int index, const std::string& value)
{
  (void)index;
  if ((Components)id == Components::EsResolution)
  {
    if(Board::Instance().CrtBoard().GetHorizontalFrequency() == ICrtInterface::HorizontalFrequency::KHz31)
    {
      CrtConf::Instance().SetSystemCRT31kHzResolution(value).Save();
    } else {
      CrtConf::Instance().SetSystemCRTResolution(value).Save();
    }
  }
}

void GuiMenuCRT::SwitchComponentChanged(int id, bool status)
{
  if ((Components)id == Components::GameRegion)
    CrtConf::Instance().SetSystemCRTGameRegionSelect(status).Save();
  if ((Components)id == Components::GameResolution)
    CrtConf::Instance().SetSystemCRTGameResolutionSelect(status).Save();
  if ((Components)id == Components::DemoIn240pOn31kHz)
    CrtConf::Instance().SetSystemCRTRunDemoIn240pOn31kHz(status).Save();
  if ((Components)id == Components::ScanlinesOn31kHz)
    CrtConf::Instance().SetSystemCRTScanlines31kHz(status).Save();
  if ((Components)id == Components::ZeroLag)
    RecalboxConf::Instance().SetGlobalZeroLag(status).Save();
  if ((Components)id == Components::ForceJack)
  {
    mForceJack = status;
    CrtConf::Instance().SetSystemCRTForceJack(status).Save();
  }
  if ((Components)id == Components::ForceHDMI)
  {
    mForceHDMI = status;
    CrtConf::Instance().SetSystemCRTForceHDMI(status).Save();
  }
}

void GuiMenuCRT::SubMenuSelected(int id)
{
  if ((Components)id == Components::Adjustment)
  {
    if (Board::Instance().CrtBoard().GetHorizontalFrequency() == ICrtInterface::HorizontalFrequency::KHz31)
    {
      ViewController::Instance().goToCrtView(CrtView::CalibrationType::kHz31);
      mWindow.CloseAll();
    }
    else if (Board::Instance().CrtBoard().MustForce50Hz()){
      ViewController::Instance().goToCrtView(CrtView::CalibrationType::kHz15_50Hz);
      mWindow.CloseAll();
    }
    else
    {
      mWindow.pushGui(new GuiMsgBox(mWindow, _("Select standard resolution set\nto calibrate:"),
                                    _("60Hz & 50Hz"), [this] {ViewController::Instance().goToCrtView(CrtView::CalibrationType::kHz15_60plus50Hz);
            mWindow.CloseAll(); },
                                    _("60Hz Only"), [this] {ViewController::Instance().goToCrtView(CrtView::CalibrationType::kHz15_60Hz);
            mWindow.CloseAll(); },
                                    _("50Hz Only"), [this] {ViewController::Instance().goToCrtView(CrtView::CalibrationType::kHz15_50Hz);
            mWindow.CloseAll();},
                                    TextAlignment::Center));
    }
  }
}


void GuiMenuCRT::SliderMoved(int id, float value)
{
  /*if ((Components)id == Components::HorizontalPalOffset)
  {
    CrtConf::Instance().SetSystemCRTHorizontalPALOffset(value).Save();
  }
  if ((Components)id == Components::VerticalPalOffset)
  {
    CrtConf::Instance().SetSystemCRTVerticalPALOffset(value).Save();
  }*/
}
