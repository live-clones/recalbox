//
// Created by digitalLumberjack on 20/0/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include "GuiMenuCRT.h"
#include "views/ViewController.h"
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
  mOriginalEsResolution = is31kHz ? "480p" : CrtConf::Instance().GetSystemCRTResolution();
  mEsResolution = AddList<std::string>(_("MENU RESOLUTION"), (int)Components::EsResolution, this, GetEsResolutionEntries(is31kHz), _(MENUMESSAGE_ADVANCED_CRT_ES_RESOLUTION_HELP_MSG));

  // Horizontal output frequency
  if (Board::Instance().CrtBoard().Has31KhzSupport()) AddText(_("SCREEN TYPE"), GetHorizontalFrequency());

  // Force 50HZ
  if (Board::Instance().CrtBoard().HasForced50hzSupport()) AddText(_("FORCE 50HZ"), Get50hz());

  // Game Region selection
  AddSwitch(_("SELECT GAME REFRESH RATE AT LAUNCH"), CrtConf::Instance().GetSystemCRTGameRegionSelect(), (int)Components::GameRegion, this, _(MENUMESSAGE_ADVANCED_CRT_GAME_REGION_HELP_MSG));

  // Game Resolution selection
  AddSwitch(_("SELECT GAME RESOLUTION AT LAUNCH"), CrtConf::Instance().GetSystemCRTGameResolutionSelect(), (int)Components::GameResolution, this, _(MENUMESSAGE_ADVANCED_CRT_GAME_RESOLUTION_HELP_MSG));

  // Zero Lag
  AddSwitch(_("ZERO LAG (BETA)"), RecalboxConf::Instance().GetGlobalZeroLag(), (int)Components::ZeroLag, this, _(MENUMESSAGE_ADVANCED_CRT_ZERO_LAG_HELP_MSG));

  // 31khz resolution
  if (is31kHz)
    m31kHzResolution = AddList<std::string>(_("GAMES RESOLUTION"), (int)Components::GamesResolutionOn31kHz, this, GetGamesResolutionOn31kHzEntries(), _(MENUMESSAGE_ADVANCED_CRT_GAMES_REZ_ON_31KHZ_HELP_MSG));

  // Force Jack
  mOriginalForceJack = CrtConf::Instance().GetSystemCRTForceJack();
  mForceJack = mOriginalForceJack;
  if(isRGBDual)
    AddSwitch(_("FORCE SOUND ON JACK"), mOriginalForceJack, (int)Components::ForceJack, this, _(MENUMESSAGE_ADVANCED_CRT_FORCE_JACK_HELP_MSG));

  // Screen Adjustments
  AddSubMenu(_("SCREEN CALIBRATION (BETA)"), (int)Components::Adjustment);

}

GuiMenuCRT::~GuiMenuCRT()
{
  // Reboot?
  if (mOriginalDac != mDac->getSelected() || mOriginalEsResolution != mEsResolution->getSelected() || mOriginalForceJack != mForceJack)
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
  list.push_back( { "None", CrtAdapterType::None, selectedDac == CrtAdapterType::None } );
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
    list.push_back({ "480p", "480p", true });
    return list;
  }

  bool rdef = CrtConf::Instance().GetSystemCRTResolution() == "240";

  list.push_back({ "240p", "240", rdef });
  list.push_back({ "480i (recommended)", "480", !rdef });

  return list;
}

std::vector<GuiMenuBase::ListEntry<std::string>> GuiMenuCRT::GetGamesResolutionOn31kHzEntries()
{
  std::vector<GuiMenuBase::ListEntry<std::string>> list;

  std::string selectedResolution = CrtConf::Instance().GetSystemCRTGamesResolutionOn31kHz();

  list.push_back({ "480p", "480p", selectedResolution == "480" || selectedResolution.empty() });
  list.push_back({ "240p (double freq)", "240", selectedResolution == "240" });

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
    CrtConf::Instance().SetSystemCRTResolution(value).Save();
  }
  if ((Components)id == Components::GamesResolutionOn31kHz)
  {
    if(m31kHzResolution != nullptr){
      CrtConf::Instance().SetSystemCRTGamesResolutionOn31kHz(value).Save();
    }
  }
}

void GuiMenuCRT::SwitchComponentChanged(int id, bool status)
{
  if ((Components)id == Components::GameRegion)
    CrtConf::Instance().SetSystemCRTGameRegionSelect(status).Save();
  if ((Components)id == Components::GameResolution)
    CrtConf::Instance().SetSystemCRTGameResolutionSelect(status).Save();
  if ((Components)id == Components::ForceJack)
  {
    mForceJack = status;
    CrtConf::Instance().SetSystemCRTForceJack(status).Save();
  }
  if ((Components)id == Components::ZeroLag)
    RecalboxConf::Instance().SetGlobalZeroLag(status).Save();
}

void GuiMenuCRT::SubMenuSelected(int id)
{
  if ((Components)id == Components::Adjustment)
  {
    ViewController::Instance().goToCrtView();
    mWindow.CloseAll();
  }
}

