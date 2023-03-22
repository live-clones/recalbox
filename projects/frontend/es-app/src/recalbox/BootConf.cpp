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

BootConf& BootConf::SetCase(std::string caze)
{
  SetString("case", caze);
  return *this;
}

RotationType BootConf::GetRotation()
{
  return (RotationType)AsInt("screen.rotation",0);
}

BootConf& BootConf::SetRotation(RotationType rotation)
{
  SetInt("screen.rotation", (uint)rotation);
  return *this;
}