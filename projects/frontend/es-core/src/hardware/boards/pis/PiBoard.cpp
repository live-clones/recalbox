//
// Created by digitalLumberjack on 20/06/2022.
//

#include <recalbox/RecalboxSystem.h>
#include "PiBoard.h"

bool PiBoard::OnRebootOrShutdown() {
  if (Path("/sys/class/leds/retroflagled/trigger").Exists())
  {
    RecalboxSystem::execute("echo 'heartbeat' > /sys/class/leds/retroflagled/trigger");
    return true;
  }
  return false;
}