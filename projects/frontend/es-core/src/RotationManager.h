//
// Created by digitalLumberjack on 10/03/23.
//

#ifndef EMULATIONSTATION_ALL_ROTATIONMANAGER_H
#define EMULATIONSTATION_ALL_ROTATIONMANAGER_H


#include "hardware/RotationType.h"
#include "games/FileData.h"

class RotationManager {
private:

public:
  static bool ShouldRotateTateEnter(RotationType& rotationType);
  static bool ShouldRotateTateExit(RotationType& rotationType);
  static bool ShouldRotateFrontendControls();
  static bool ShouldRotateGameControls(const FileData& game);
  static RotationType ShouldRotateGame(const FileData& game);
  static RotationType GetSystemRotation();
};


#endif //EMULATIONSTATION_ALL_ROTATIONMANAGER_H
