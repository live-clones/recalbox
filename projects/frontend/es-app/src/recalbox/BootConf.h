//
// Created by digitalLumberjack on 10/03/23.
//

#ifndef EMULATIONSTATION_ALL_BOOTCONF_H
#define EMULATIONSTATION_ALL_BOOTCONF_H


#include "utils/IniFile.h"
#include "utils/cplusplus/StaticLifeCycleControler.h"
#include "hardware/Case.h"
#include "hardware/RotationType.h"

class BootConf : public IniFile, public StaticLifeCycleControler<BootConf> {
public:
  BootConf();


  std::string GetCase();
  BootConf& SetCase(std::string caze);
  RotationType GetRotation();
  BootConf& SetRotation(RotationType rotation);
};


#endif //EMULATIONSTATION_ALL_BOOTCONF_H
