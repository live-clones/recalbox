//
// Created by matthieu on 12/09/15.
//

#ifndef EMULATIONSTATION_ALL_RECALBOXCONF_H
#define EMULATIONSTATION_ALL_RECALBOXCONF_H


#include <string>
#include <map>

class RecalboxConf {

public:
    RecalboxConf(bool mainFile = true);

    ~RecalboxConf();

    bool loadRecalboxConf(bool mainFile = true);

    bool saveRecalboxConf();

    std::string get(const std::string &name);
    std::string get(const std::string &name, const std::string &defaut);
    bool getBool(const std::string& name, bool defaut = false);

    void set(const std::string &name, const std::string &value);
    void setBool(const std::string &name, bool value);

    static RecalboxConf *sInstance;

    static RecalboxConf *getInstance();
private:
    std::map<std::string, std::string> confMap;

};


#endif //EMULATIONSTATION_ALL_RECALBOXCONF_H
