#pragma once

#include <string>

/*!
 * Raspberry generation
 */
enum class RaspberryGeneration
{
    UndetectedYet, // Not yet detected
    NotRaspberry,  // Hardware is not a raspberry
    Pi0or1,        // Pi 0, 0W, 1, A, B, A+, B+
    Pi2,           // Pi 2B
    Pi3,           // Pi 3B
    Pi3plus,       // Pi 3B+
    NotYetKnown,   // Unknown Pi with higher revisions
};

/*!
 * Raspberry model (real models)
 * https://www.raspberrypi.org/documentation/hardware/raspberrypi/revision-codes/README.md
 */
enum class RaspberryModel
{
  OneA      = 0x0,
  OneB      = 0x1,
  OneAPlus  = 0x2,
  OneBPlus  = 0x3,
  TwoB      = 0x4,
  Alpha     = 0x5,
  OneCM1    = 0x6,
  TreeB     = 0x8,
  Zero      = 0x9,
  TreeCM3   = 0xA,
  ZeroW     = 0xC,
  TreeBPlus = 0xD,
  TreeAPlus = 0xE,
};

std::string getHomePath();

int runShutdownCommand(); // shut down the system (returns 0 if successful)
int runRestartCommand(); // restart the system (returns 0 if successful)
int runSystemCommand(const std::string& cmd_utf8); // run a utf-8 encoded in the shell (requires wstring conversion on Windows)
RaspberryGeneration getRaspberryVersion();
