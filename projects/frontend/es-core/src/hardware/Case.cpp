//
// Created by digitalLumberjack on 17/06/2022.
//

#include "Case.h"
#include "Board.h"
#include <utils/locale/LocaleHelper.h>

bool Case::SetCaseInBoot(const std::string& theCase)
{
  IniFile bootConf(Path("/boot/recalbox-boot.conf"), false);
  bootConf.SetString("case", theCase);
  bootConf.Save();
  return true;
}


bool Case::Install() const
{
  switch (mModel)
  {
    case CaseModel::NespiCasePlus:
    case CaseModel::SuperPiCase:
    case CaseModel::MegaPiCase:
    case CaseModel::PiStation:
    case CaseModel::ArgonOne:
    case CaseModel::Nespi4CaseManual:
    case CaseModel::SuperPi4Case:
    case CaseModel::RaspberryPiTouchDisplay:
    {
      SetCaseInBoot(mShortName);
      break;
    }
    case CaseModel::None:
    {
      SetCaseInBoot("none");
      break;
    }
    case CaseModel::GPiV1:
    case CaseModel::GPiV2:
    case CaseModel::GPiV3:
    case CaseModel::GPi2:
    case CaseModel::GPi2W:
    case CaseModel::Nuxii:
    case CaseModel::PiBoy:
    case CaseModel::Nespi4Case:
      break;
  }
  return true;
}

//! Get case from string
Case Case::FromShortName(const std::string& value)
{
  if (value == "GPiV1") return Create(CaseModel::GPiV1);
  if (value == "GPiV2") return Create(CaseModel::GPiV2);
  if (value == "GPiV3") return Create(CaseModel::GPiV3);
  if (value == "GPi2") return Create(CaseModel::GPi2);
  if (value == "GPi2W") return Create(CaseModel::GPi2W);
  if (value == "Nuxii") return Create(CaseModel::Nuxii);
  if (value == "PiBoy") return Create(CaseModel::PiBoy);
  if (value == "NESPi4") return Create(CaseModel::Nespi4Case);
  if (value == "SuperPi4Case") return Create(CaseModel::SuperPi4Case);
  if (value == "NESPi4Manual") return Create(CaseModel::Nespi4CaseManual);
  if (value == "NespiCasePlus") return Create(CaseModel::NespiCasePlus);
  if (value == "PiStation") return Create(CaseModel::PiStation);
  if (value == "SuperPiCase") return Create(CaseModel::SuperPiCase);
  if (value == "MegaPiCase") return Create(CaseModel::MegaPiCase);
  if (value == "ArgonOne") return Create(CaseModel::ArgonOne);
  if (value == "RaspberryPiTouchDisplay") return Create(CaseModel::RaspberryPiTouchDisplay);
  return Create(CaseModel::None);
}

//! Create case from CaseModel
Case Case::Create(CaseModel model)
{
  const std::string retroflagInstallMessage = _("To take advantage of the safe shutdown feature, the SAFE SHUTDOWN switch of your Retroflag case (located inside the case, on the electronic part) must be positioned on ON.");
  switch (model)
  {
    case CaseModel::None:
      return Case(CaseModel::None, !CASE_DETECTION_AUTOMATIC, CASE_SHUTDOWN_SUPPORTED, _("NONE"), "", "");
    case CaseModel::GPiV1:
      return Case(CaseModel::GPiV1, CASE_DETECTION_AUTOMATIC, CASE_SHUTDOWN_SUPPORTED, "Gpi Case (v1)", "GPiV1","");
    case CaseModel::GPiV2:
      return Case(CaseModel::GPiV2, CASE_DETECTION_AUTOMATIC, CASE_SHUTDOWN_SUPPORTED, "Gpi Case (v2)", "GPiV2", "");
    case CaseModel::GPiV3:
      return Case(CaseModel::GPiV3, CASE_DETECTION_AUTOMATIC, CASE_SHUTDOWN_SUPPORTED, "Gpi Case (v3)", "GPiV3", "");
    case CaseModel::GPi2:
      return Case(CaseModel::GPi2, CASE_DETECTION_AUTOMATIC, CASE_SHUTDOWN_SUPPORTED, "Gpi Case 2", "GPi2", "");
    case CaseModel::GPi2W:
      return Case(CaseModel::GPi2W, CASE_DETECTION_AUTOMATIC, CASE_SHUTDOWN_SUPPORTED, "Gpi Case 2W", "GPi2W", "");
    case CaseModel::Nuxii:
      return Case(CaseModel::Nuxii, CASE_DETECTION_AUTOMATIC, CASE_SHUTDOWN_SUPPORTED, "Nuxii", "Nuxii", "");
    case CaseModel::PiBoy:
      return Case(CaseModel::PiBoy, CASE_DETECTION_AUTOMATIC, CASE_SHUTDOWN_SUPPORTED, "PiBoy DMG", "PiBoy", "");
    case CaseModel::Nespi4Case:
      return Case(CaseModel::Nespi4Case, CASE_DETECTION_AUTOMATIC, !CASE_SHUTDOWN_SUPPORTED, "Nespi4Case", "NESPi4", "");
    case CaseModel::Nespi4CaseManual:
      return Case(CaseModel::Nespi4CaseManual, !CASE_DETECTION_AUTOMATIC, !CASE_SHUTDOWN_SUPPORTED, "Nespi4Case (Retroflag)", "NESPi4Manual", retroflagInstallMessage);
    case CaseModel::SuperPi4Case:
      return Case(CaseModel::SuperPi4Case, !CASE_DETECTION_AUTOMATIC, !CASE_SHUTDOWN_SUPPORTED, "SuperPi4Case (Retroflag)", "SuperPi4Case", retroflagInstallMessage);
    case CaseModel::NespiCasePlus:
      return Case(CaseModel::NespiCasePlus, !CASE_DETECTION_AUTOMATIC, !CASE_SHUTDOWN_SUPPORTED, "Nespi Case + (Retroflag)", "NespiCasePlus", retroflagInstallMessage);
    case CaseModel::PiStation:
      return Case(CaseModel::PiStation, !CASE_DETECTION_AUTOMATIC, !CASE_SHUTDOWN_SUPPORTED, "PiStation (Retroflag)", "PiStation", retroflagInstallMessage);
    case CaseModel::SuperPiCase:
      return Case(CaseModel::SuperPiCase, !CASE_DETECTION_AUTOMATIC, !CASE_SHUTDOWN_SUPPORTED, "Super Pi Case (Retroflag)", "SuperPiCase", retroflagInstallMessage);
    case CaseModel::MegaPiCase:
      return Case(CaseModel::MegaPiCase, !CASE_DETECTION_AUTOMATIC, !CASE_SHUTDOWN_SUPPORTED, "Mega Pi Case (Retroflag)", "MegaPiCase", retroflagInstallMessage);
    case CaseModel::ArgonOne:
      return Case(CaseModel::ArgonOne, !CASE_DETECTION_AUTOMATIC, CASE_SHUTDOWN_SUPPORTED, "Argon One (Argon40)", "ArgonOne", "");
    case CaseModel::RaspberryPiTouchDisplay:
      return Case(CaseModel::RaspberryPiTouchDisplay, !CASE_DETECTION_AUTOMATIC, !CASE_SHUTDOWN_SUPPORTED, "Raspberry Pi Touch Display", "RaspberryPiTouchDisplay", "");
  }
  return Case(CaseModel::None, false, true, _("NONE"), "", "");
}


Case Case::CurrentCase()
{
  IniFile bootConf(Path("/boot/recalbox-boot.conf"), false);
  std::string currentCase = bootConf.AsString("case");
  Strings::Vector splitted = Strings::Split(currentCase, ':');
  if (!splitted.empty())
    return Case::FromShortName(splitted[0]);
  return Case::Create(Case::CaseModel::None);
}

std::vector<Case> Case::SupportedManualCases()
{
  std::vector<Case> list;
  if (Board::Instance().GetBoardType() == BoardType::Pi4)
  {
    list.push_back(Case::Create(Case::CaseModel::PiStation));
    list.push_back(Case::Create(Case::CaseModel::ArgonOne));
    list.push_back(Case::Create(Case::CaseModel::Nespi4CaseManual));
    list.push_back(Case::Create(Case::CaseModel::SuperPi4Case));
    list.push_back(Case::Create(Case::CaseModel::RaspberryPiTouchDisplay));
    list.push_back(Case::Create(Case::CaseModel::None));
  }
  if (Board::Instance().GetBoardType() == BoardType::Pi3plus || Board::Instance().GetBoardType() == BoardType::Pi3)
  {
    list.push_back(Case::Create(Case::CaseModel::MegaPiCase));
    list.push_back(Case::Create(Case::CaseModel::SuperPiCase));
    list.push_back(Case::Create(Case::CaseModel::NespiCasePlus));
    list.push_back(Case::Create(Case::CaseModel::RaspberryPiTouchDisplay));
    list.push_back(Case::Create(Case::CaseModel::None));
  }
  return list;
}

