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
  , mOriginalDac(RecalboxConf::Instance().GetSystemCRT())
  , mOriginal31kHzResolution(RecalboxConf::Instance().GetSystemCRTGamesResolutionOn31kHz())
{
  // Selected Dac
  mDac = AddList<CrtAdapterType>(_("CRT ADAPTER"), (int)Components::CRTDac, this, GetDacEntries(), _(MENUMESSAGE_ADVANCED_CRT_DAC_HELP_MSG));

  // Resolution
  mEsResolution = AddList<std::string>(_("MENU RESOLUTION"), (int)Components::EsResolution, this, GetEsResolutionEntries(), _(MENUMESSAGE_ADVANCED_CRT_ES_RESOLUTION_HELP_MSG));

  // Horizontal output frequency
  if (Board::Instance().CrtBoard().Has31KhzSupport()) AddText(_("SCREEN TYPE"), GetHorizontalFrequency());

  // Force 50HZ
  if (Board::Instance().CrtBoard().HasForced50hzSupport()) AddText(_("FORCE 50HZ"), Get50hz());

  // Game options
  AddSwitch(_("GAME OPTION AT LAUNCH"), RecalboxConf::Instance().GetSystemCRTGameOptions(), (int)Components::GameOptions, this, _(MENUMESSAGE_ADVANCED_CRT_GAME_OPTION_HELP_MSG));

  // 31khz resolution
  if (Board::Instance().CrtBoard().GetHorizontalFrequency() == ICrtInterface::HorizontalFrequency::KHz31)
    m31kHzResolution = AddList<std::string>(_("GAMES RESOLUTION"), (int)Components::GamesResolutionOn31kHz, this, GetGamesResolutionOn31kHzEntries(), _(MENUMESSAGE_ADVANCED_CRT_GAMES_REZ_ON_31KHZ_HELP_MSG));

  mOriginalEsResolution = RecalboxConf::Instance().GetEmulationstationVideoMode();
}

GuiMenuCRT::~GuiMenuCRT()
{
  // Reboot?
  if (mOriginalDac != mDac->getSelected()
  || mOriginalEsResolution != mEsResolution->getSelected())
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

std::vector<GuiMenuBase::ListEntry<CrtAdapterType>> GuiMenuCRT::GetDacEntries()
{
  std::vector<GuiMenuBase::ListEntry<CrtAdapterType>> list;

  CrtAdapterType selectedDac = RecalboxConf::Instance().GetSystemCRT();

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

std::vector<GuiMenuBase::ListEntry<std::string>> GuiMenuCRT::GetEsResolutionEntries()
{
  std::vector<GuiMenuBase::ListEntry<std::string>> list;

  std::string selectedResolution = RecalboxConf::Instance().GetEmulationstationVideoMode();

  bool rdef = selectedResolution == "default" || selectedResolution == "640x480";

  list.push_back({ "480i (recommended)", "default", rdef });
  list.push_back({ "240p", "1920x240", !rdef });

  return list;
}

std::vector<GuiMenuBase::ListEntry<std::string>> GuiMenuCRT::GetGamesResolutionOn31kHzEntries()
{
  std::vector<GuiMenuBase::ListEntry<std::string>> list;

  std::string selectedResolution = RecalboxConf::Instance().GetSystemCRTGamesResolutionOn31kHz();

  list.push_back({ "480p", "480p", selectedResolution == "480p" || selectedResolution == "" });
  list.push_back({ "240p (double freq)", "240p", selectedResolution == "240p" });

  return list;
}


void GuiMenuCRT::OptionListComponentChanged(int id, int index, const CrtAdapterType& value)
{
  (void)index;
  if ((Components)id == Components::CRTDac)
  {
    CrtAdapterType oldValue = RecalboxConf::Instance().GetSystemCRT();
    if (value == CrtAdapterType::None)
    {
      if (oldValue != CrtAdapterType::None)
      {
        RecalboxConf::Instance().SetSystemCRT(CrtAdapterType::None).Save();
        RecalboxConf::Instance().SetEmulationstationRatio("default").Save();
        RecalboxConf::Instance().SetEmulationstationVideoMode("default").Save();
        mEsResolution->select("default");
      }
    }
    else RecalboxConf::Instance().SetSystemCRT(value).Save();
  }
}

void GuiMenuCRT::OptionListComponentChanged(int id, int index, const std::string& value)
{
  (void)index;
  if ((Components)id == Components::EsResolution)
  {
    RecalboxConf::Instance().SetEmulationstationRatio(value == "1920x240" ? "1.3334" : "default").Save();
    RecalboxConf::Instance().SetEmulationstationVideoMode(value).Save();
  }
  if ((Components)id == Components::GamesResolutionOn31kHz)
  {
    if(m31kHzResolution != nullptr){
      RecalboxConf::Instance().SetSystemCRTGamesResolutionOn31kHz(value).Save();
    }
  }
}

void GuiMenuCRT::SwitchComponentChanged(int id, bool status)
{
  if ((Components)id == Components::GameOptions)
    RecalboxConf::Instance().SetSystemCRTGameOptions(status).Save();
}

