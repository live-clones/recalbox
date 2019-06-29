#pragma once

#include <string>

/*!
 * Raspberry generation
 */
enum class RaspberryGeneration
{
    UndetectedYet, // Not yet detected
    NotRaspberry,  // Hardware is not a raspberry
    Pi0,           // Pi 0, 0W
    Pi1,           // Pi 1, A, B, A+, B+
    Pi2,           // Pi 2B
    Pi3,           // Pi 3B
    Pi3plus,       // Pi 3B+
    Pi4,           // Pi 4B
    NotYetKnown,   // Unknown Pi with higher revisions
};

/*!
 * Raspberry model (real models)
 * https://www.raspberrypi.org/documentation/hardware/raspberrypi/revision-codes/README.md
 */
enum class RaspberryModel
{
  OneA        = 0x00,
  OneB        = 0x01,
  OneAPlus    = 0x02,
  OneBPlus    = 0x03,
  TwoB        = 0x04,
  Alpha       = 0x05,
  OneCM1      = 0x06,
  TreeB       = 0x08,
  Zero        = 0x09,
  TreeCM3     = 0x0A,
  ZeroW       = 0x0C,
  TreeBPlus   = 0x0D,
  TreeAPlus   = 0x0E,
  TreeCM3Plus = 0x10,
  FourB       = 0x11,
};

std::string getHomePath();

int runShutdownCommand(); // shut down the system (returns 0 if successful)
int runRestartCommand(); // restart the system (returns 0 if successful)
int runSystemCommand(const std::string& cmd_utf8); // run a utf-8 encoded in the shell (requires wstring conversion on Windows)
RaspberryGeneration getRaspberryVersion();
