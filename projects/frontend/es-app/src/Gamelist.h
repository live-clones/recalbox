#pragma once

#include <string>
#include <unordered_map>
#include "FileData.h"

// Forward declarations
class SystemData;

// Loads gamelist.xml data into a SystemData.
void parseGamelist(SystemData* system, FileData::StringMap& doppelgangerWatcher);

// Writes currently loaded metadata for a SystemData to gamelist.xml.
void updateGamelist(SystemData* system);
