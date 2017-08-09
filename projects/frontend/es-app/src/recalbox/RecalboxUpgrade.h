#ifndef EMULATIONSTATION_ALL_RecalboxUpgrade_H
#define EMULATIONSTATION_ALL_RecalboxUpgrade_H

#include "RecalboxSystem.h"

class RecalboxUpgrade {

public:
    static RecalboxUpgrade *getInstance();

    std::string getVersion();

    std::string getUpdateVersion();

    bool updateLastChangelogFile();

    std::string getChangelog();

    std::pair<std::string, int> updateSystem(BusyComponent *ui);

    std::string getUpdateChangelog();

    bool canUpdate();

private:
    RecalboxSystem *system = RecalboxSystem::getInstance();
    static RecalboxUpgrade *instance;

};


#endif //EMULATIONSTATION_ALL_RecalboxUpgrade_H
