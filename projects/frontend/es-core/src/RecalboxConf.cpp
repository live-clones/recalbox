#include "RecalboxConf.h"
#include <iostream>
#include <fstream>
#include <boost/regex.hpp>
#include "Log.h"
#include <boost/algorithm/string/predicate.hpp>

boost::regex validLine("^(?<key>[^;|#].*?)=(?<val>.*?)$");
boost::regex commentLine("^;(?<key>.*?)=(?<val>.*?)$");

std::string recalboxConfFile = "/recalbox/share/system/recalbox.conf";
std::string recalboxConfFileInit = "/recalbox/share_init/system/recalbox.conf";
std::string recalboxConfFileTmp = "/recalbox/share/system/recalbox.conf.tmp";

RecalboxConf::RecalboxConf(bool mainFile) {
    loadRecalboxConf(mainFile);
}

RecalboxConf& RecalboxConf::Instance()
{
  static RecalboxConf _Instance; // Initialized the first time this is called - destroyed on program exit
  return _Instance;
}

bool RecalboxConf::loadRecalboxConf(bool mainFile) {
    std::string line;
    std::string filePath = mainFile ? recalboxConfFile : recalboxConfFileInit;
    std::ifstream recalboxConf(filePath);
    if (recalboxConf && recalboxConf.is_open()) {
        while (std::getline(recalboxConf, line)) {
            boost::smatch lineInfo;
            if (boost::regex_match(line, lineInfo, validLine)) {
                confMap[std::string(lineInfo["key"])] = std::string(lineInfo["val"]);
            }
        }
        recalboxConf.close();
    } else {
        LOG(LogError) << "Unable to open " << filePath;
        return false;
    }
    return true;
}


bool RecalboxConf::saveRecalboxConf() {
    std::ifstream filein(recalboxConfFile); //File to read from
    if (!filein) {
        LOG(LogError) << "Unable to open for saving :  " << recalboxConfFile << "\n";
        return false;
    }
    /* Read all lines in a vector */
    std::vector<std::string> fileLines;
    std::string line;
    while (std::getline(filein, line)) {
        fileLines.push_back(line);
    }
    filein.close();


    /* Save new value if exists */
    for (std::map<std::string, std::string>::iterator it = confMap.begin(); it != confMap.end(); ++it) {
        std::string key = it->first;
        std::string val = it->second;
        bool lineFound = false;
        for (int i = 0; i < (int)fileLines.size(); i++) {
            std::string currentLine = fileLines[i];

            if (boost::starts_with(currentLine, key+"=") || boost::starts_with(currentLine, ";"+key+"=")){
                fileLines[i] = key + "=" + val;
                lineFound = true;
            }
        }
        if(!lineFound){
            fileLines.push_back(key + "=" + val);
        }
    }
    std::ofstream fileout(recalboxConfFileTmp); //Temporary file
    if (!fileout) {
        LOG(LogError) << "Unable to open for saving :  " << recalboxConfFileTmp << "\n";
        return false;
    }
    for (int i = 0; i < (int)fileLines.size(); i++) {
        fileout << fileLines[i] << "\n";
    }

    fileout.close();


    /* Copy back the tmp to recalbox.conf */
    std::ifstream  src(recalboxConfFileTmp, std::ios::binary);
    std::ofstream  dst(recalboxConfFile,   std::ios::binary);
    dst << src.rdbuf();

    remove(recalboxConfFileTmp.c_str());

    return true;
}

std::string RecalboxConf::get(const std::string &name) {
    if (confMap.count(name)) {
        return confMap[name];
    }
    return "";
}
std::string RecalboxConf::get(const std::string &name, const std::string &defaultValue) {
    if (confMap.count(name)) {
        return confMap[name];
    }
    return defaultValue;
}

bool RecalboxConf::getBool(const std::string &name, bool defaultValue) {
    if (confMap.count(name)) {
        return confMap[name] == "1";
    }
    return defaultValue;
}

unsigned int RecalboxConf::getUInt(const std::string &name, unsigned int defaultValue) {
    try {
        if (confMap.count(name)) {
            int value = std::stoi(confMap[name]);
            return value > 0 ? (unsigned int) value : 0;
        }
    } catch(std::invalid_argument&) {}

    return defaultValue;
}

void RecalboxConf::set(const std::string &name, const std::string &value) {
    confMap[name] = value;
}

void RecalboxConf::setBool(const std::string &name, bool value) {
    confMap[name] = value ? "1" : "0";
}

void RecalboxConf::setUInt(const std::string &name, unsigned int value) {
    confMap[name] = std::to_string(value).c_str();
}

/**
 * Join a list using the , as delimiter, removing all empty values
 */
void RecalboxConf::setList(const std::string &name, const std::vector<std::string> &values) {

  std::stringstream ss;
  std::string delimiter = ",";
  bool addDelimiter = false;

  for (auto & value: values)
  {
    if (value.length())
    {
      if (addDelimiter)
        ss << delimiter;
      ss << value;
      addDelimiter = true;
    }
  }
  confMap[name] = ss.str();
}

bool RecalboxConf::isInList(const std::string &name, const std::string &value) {
    bool result = false;
    if (confMap.count(name)) {
        std::string s = confMap[name];
        std::string delimiter = ",";

        size_t pos = 0;
        std::string token;
        while (((pos = s.find(delimiter)) != std::string::npos) ) {
            token = s.substr(0, pos);
            if (token == value)
	            result = true;
            s.erase(0, pos + delimiter.length());
        }
	    if (s == value)
		    result = true;
    }
    return result;
}