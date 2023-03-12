//
// Created by digitalLumberjack on 10/03/23.
//

#include "BootConf.h"

BootConf::BootConf()
    : IniFile(Path("/boot/recalbox-boot.conf"), false),
      StaticLifeCycleControler<BootConf>("BootConf")
{
}

std::string BootConf::GetCase()
{
  std::string currentCase = AsString("case");
  Strings::Vector splitted = Strings::Split(currentCase, ':');
  if (!splitted.empty())
    return splitted[0];
  return "none";
}

bool BootConf::SetCase(std::string caze)
{
  SetString("case", caze);
  return true;
}

RotationType BootConf::GetRotation()
{
  return (RotationType)AsInt("screen.rotation",0);
}

bool BootConf::SetRotation(RotationType rotation)
{
  SetString("screen.rotation", RotationUtils::StringValue(rotation));
  return true;
}