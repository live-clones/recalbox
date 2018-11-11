#pragma once

#include <string>

enum class RaspberryPiVersion
{
    NotRaspberry, // Hardware is not a raspberry
    Pi0,          // Pi 0, 0W
    Pi1,          // Pi 1, A, B, A+, B+
    Pi2,          // Pi 2B
    Pi3,          // Pi 3B
    Pi3plus,      // Pi 3B+
    NotYetKnown,  // Unknown Pi with higher revisions
};

std::string getHomePath();

int runShutdownCommand(); // shut down the system (returns 0 if successful)
int runRestartCommand(); // restart the system (returns 0 if successful)
int runSystemCommand(const std::string& cmd_utf8); // run a utf-8 encoded in the shell (requires wstring conversion on Windows)
RaspberryPiVersion getRaspberryVersion();
