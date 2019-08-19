#pragma once

#include "RecalboxSystem.h"

class RecalboxUpgrade
{
  public:
    static std::string getVersion();

    static std::string getUpdateVersion();

    static bool updateLastChangelogFile();

    static std::string getChangelog();

    static std::pair<std::string, int> updateSystem(BusyComponent* ui);

    static std::string getUpdateChangelog();

    static bool canUpdate();
};
