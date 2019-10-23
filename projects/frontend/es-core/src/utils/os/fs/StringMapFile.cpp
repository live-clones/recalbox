//
// Created by bkg2k on 17/10/2019.
//

#include <utils/StringUtil.h>
#include "StringMapFile.h"

void StringMapFile::Load()
{
  mMap.clear();
  FILE* f = fopen(mPath.c_str(), "r");
  if (f != nullptr)
  {
    char buffer[2048];
    while (fgets(buffer, sizeof(buffer), f) != nullptr)
    {
      std::string keyvalue = buffer;
      unsigned long pos = 0;
      if ((pos = keyvalue.find('=')) != std::string::npos)
      {
        std::string key = StringUtil::trim(keyvalue.substr(0, pos), " \t\n\r");
        std::string value = StringUtil::trim(keyvalue.substr(pos + 1), " \t\n\r");
        mMap[key] = value;
      }
    }
    fclose(f);
  }
}

void StringMapFile::Save()
{
  FILE* f = fopen(mPath.c_str(), "w+");
  if (f != nullptr)
  {
    for(auto& holder : mMap)
    {
      std::string keyvalue = holder.first;
      keyvalue += '=';
      keyvalue += holder.second;
      keyvalue += '\n';
      fputs(keyvalue.c_str(), f);
    }
    fclose(f);
  }
}

std::string StringMapFile::GetString(const std::string& key, const std::string& defaultvalue)
{
  std::map<std::string, std::string>::iterator it = mMap.find(key);
  if (it != mMap.end()) return it->second;
  return defaultvalue;
}

int StringMapFile::GetInt(const std::string& key, int defaultvalue)
{
  std::map<std::string, std::string>::iterator it = mMap.find(key);
  if (it != mMap.end()) return std::stoi(it->second);
  return defaultvalue;
}

bool StringMapFile::GetBool(const std::string& key, bool defaultvalue)
{
  std::map<std::string, std::string>::iterator it = mMap.find(key);
  if (it != mMap.end()) return (it->second == "1" || it->second == "true");
  return defaultvalue;
}

void StringMapFile::SetString(const std::string& key, const std::string& value)
{
  mMap[key] = value;
}

void StringMapFile::SetInt(const std::string& key, int value)
{
  mMap[key] = std::to_string(value);
}

void StringMapFile::SetBool(const std::string& key, bool value)
{
  mMap[key] = value ? "1" : "0";
}


