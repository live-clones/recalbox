//
// Created by digitalLumberjack on 17/06/2022.
//

#include "Case.h"
#include "Board.h"

bool Case::SetCaseInBoot(const std::string& theCase, bool install)
{
  IniFile bootConf(Path("/boot/recalbox-boot.conf"), false);
  if(install)
    bootConf.SetString("case", theCase);
  else
    bootConf.SetString("case", theCase + ":3");
  bootConf.Save();
  return true;
}

bool Case::SetInstalled(bool install)
{
  switch (mModel)
  {
    case CaseModel::NespiCasePlus:
    case CaseModel::SuperPiCase:
    case CaseModel::MegaPiCase:
    case CaseModel::PiStation:
    {
      SetCaseInBoot(mShortName, install);
      break;
    }
    case CaseModel::None:
    {
      SetCaseInBoot("none:1");
      break;
    }
    default:
      break;
  }
  return true;
}
bool Case::Install()
{
  return SetInstalled(true);
}

bool Case::Uninstall()
{
  return SetInstalled(false);
}

//! Get case from string
Case Case::FromShortName(const std::string& value)
{
  if (value == "GPiV1") return Create(CaseModel::GPiV1);
  if (value == "GPiV2") return Create(CaseModel::GPiV2);
  if (value == "GPiV3") return Create(CaseModel::GPiV3);
  if (value == "GPi2") return Create(CaseModel::GPi2);
  if (value == "Nuxii") return Create(CaseModel::Nuxii);
  if (value == "PiBoy") return Create(CaseModel::PiBoy);
  if (value == "NESPi4") return Create(CaseModel::Nespi4Case);
  if (value == "NespiCasePlus") return Create(CaseModel::NespiCasePlus);
  if (value == "PiStation") return Create(CaseModel::PiStation);
  if (value == "SuperPiCase") return Create(CaseModel::SuperPiCase);
  if (value == "MegaPiCase") return Create(CaseModel::MegaPiCase);
  return Create(CaseModel::None);
}

//! Create case from CaseModel
Case Case::Create(CaseModel model)
{
  switch (model)
  {
    case CaseModel::None:
      return Case(CaseModel::None, false, "None", "");
    case CaseModel::GPiV1:
      return Case(CaseModel::GPiV1, true, "Gpi Case (v1)", "GPiV1");
    case CaseModel::GPiV2:
      return Case(CaseModel::GPiV2, true, "Gpi Case (v2)", "GPiV2");
    case CaseModel::GPiV3:
      return Case(CaseModel::GPiV3, true, "Gpi Case (v3)", "GPiV3");
    case CaseModel::GPi2:
      return Case(CaseModel::GPiV3, true, "Gpi Case 2", "GPi2");
    case CaseModel::Nuxii:
      return Case(CaseModel::Nuxii, true, "Nuxii", "Nuxii");
    case CaseModel::PiBoy:
      return Case(CaseModel::PiBoy, true, "PiBoy DMG", "PiBoy");
    case CaseModel::Nespi4Case:
      return Case(CaseModel::Nespi4Case, true, "Nespi4Case", "NESPi4");
    case CaseModel::NespiCasePlus:
      return Case(CaseModel::NespiCasePlus, false, "Nespi Case +", "NespiCasePlus");
    case CaseModel::PiStation:
      return Case(CaseModel::PiStation, false, "PiStation", "PiStation");
    case CaseModel::SuperPiCase:
      return Case(CaseModel::SuperPiCase, false, "Super Pi Case", "SuperPiCase");
    case CaseModel::MegaPiCase:
      return Case(CaseModel::MegaPiCase, false, "Mega Pi Case", "MegaPiCase");
  }
  return Case(CaseModel::None, false, "None", "");
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
    list.push_back(Case::Create(Case::CaseModel::None));
  }
  if (Board::Instance().GetBoardType() == BoardType::Pi3plus || Board::Instance().GetBoardType() == BoardType::Pi3)
  {
    list.push_back(Case::Create(Case::CaseModel::MegaPiCase));
    list.push_back(Case::Create(Case::CaseModel::SuperPiCase));
    list.push_back(Case::Create(Case::CaseModel::NespiCasePlus));
    list.push_back(Case::Create(Case::CaseModel::None));
  }
  return list;
}

