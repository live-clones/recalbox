//
// Created by matthieu on 12/09/15.
//

#ifndef EMULATIONSTATION_ALL_RECALBOXCONF_H
#define EMULATIONSTATION_ALL_RECALBOXCONF_H


#include <string>
#include <vector>
#include <map>

class RecalboxConf {

public:
    RecalboxConf(bool mainFile = true);

    bool loadRecalboxConf(bool mainFile = true);

    bool saveRecalboxConf();

    std::string get(const std::string &name);
    std::string get(const std::string &name, const std::string &defaultValue);
    bool getBool(const std::string& name, bool defaultValue = false);
    unsigned int getUInt(const std::string& name, unsigned int defaultValue = 0);

    void set(const std::string &name, const std::string &value);
    void setBool(const std::string &name, bool value);
    void setUInt(const std::string &name, unsigned int value);
    void setList(const std::string &name, const std::vector<std::string> &values);

    bool isInList(const std::string &name, const std::string &value);

    static RecalboxConf& Instance();
    static RecalboxConf *getInstance() { return &Instance(); } ; // Remove me ASAP
private:
    std::map<std::string, std::string> confMap;

};


#endif //EMULATIONSTATION_ALL_RECALBOXCONF_H
