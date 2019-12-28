#include "RecalboxConf.h"
#include <utils/Strings.h>
#include <utils/Files.h>
#include "utils/Log.h"

static Path recalboxConfFile("/recalbox/share/system/recalbox.conf");
static Path recalboxConfFileBackup("/recalbox/share/system/recalbox.conf.previous");
static Path recalboxConfFileInit("/recalbox/share_init/system/recalbox.conf");

RecalboxConf::RecalboxConf(bool initialConfigOnly)
{
  LoadRecalboxConf(initialConfigOnly);
}

RecalboxConf& RecalboxConf::Instance()
{
  static RecalboxConf _Instance(false); // Initialized the first time this is called - destroyed on program exit
  return _Instance;
}

bool RecalboxConf::IsValidKeyValue(const std::string& line, std::string& key, std::string& value)
{
  static std::string _allowedCharacters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.-";
  if (!line.empty()) // Ignore empty line
  {
    bool comment = (line[0] == ';' || line[0] == '#');
    if (!comment)
    {
      size_t separatorPos = line.find('=');
      if (separatorPos != std::string::npos) // Expect a key=value line
      {
        size_t validated = line.find_first_not_of(_allowedCharacters);
        if (validated == std::string::npos || validated >= separatorPos) // Unknown characters after the = ?
        {
          key = line.substr(0, separatorPos);
          value = line.substr(separatorPos + 1);
          return true;
        }
        else LOG(LogWarning) << "Invalid key: `" << line << '`';
      }
      else LOG(LogError) << "Invalid line: `" << line << '`';
    }
  }
  return false;
}

bool RecalboxConf::LoadRecalboxConf(bool initialConfigOnly)
{
  // Load file
  std::string content = Files::LoadFile(initialConfigOnly ? recalboxConfFileInit : recalboxConfFile);
  if (content.empty() && !initialConfigOnly) content = Files::LoadFile(recalboxConfFileInit);

  // Split lines
  content = Strings::Replace(content, "\r", "");
  Strings::Vector lines = Strings::Split(content, '\n');

  // Get key/value
  std::string key, value;
  for (std::string& line : lines)
    if (IsValidKeyValue(line, key, value))
      confMap[key] = value;

  return true;
}

bool RecalboxConf::SaveRecalboxConf()
{
  // Load file
  std::string content = Files::LoadFile(recalboxConfFile);
  if (content.empty()) content = Files::LoadFile(recalboxConfFileInit);

  // Split lines
  content = Strings::Replace(content, "\r", "");
  Strings::Vector lines = Strings::Split(content, '\n');

  // Save new value if exists
  for (auto& it : confMap)
  {
    std::string key = it.first;
    std::string val = it.second;
    bool lineFound = false;
    for (auto& line : lines)
      if (Strings::StartsWith(line, key + "=") || Strings::StartsWith(line, ";" + key + "="))
      {
        line = key.append("=").append(val);
        lineFound = true;
      }
    if (!lineFound)
      lines.push_back(key.append("=").append(val));
  }

  // Delete old backup
  if (recalboxConfFileBackup.Exists())
    recalboxConfFileBackup.Delete();
  // Rename
  Path::Rename(recalboxConfFile, recalboxConfFileBackup);
  // Save new
  Files::SaveFile(recalboxConfFile, Strings::Join(lines, "\r\n"));

  return true;
}

std::string RecalboxConf::AsString(const std::string& name)
{
  auto item = confMap.find(name);
  return (item != confMap.end()) ? item->second : std::string();
}

std::string RecalboxConf::AsString(const std::string& name, const std::string& defaultValue)
{
  auto item = confMap.find(name);
  return (item != confMap.end()) ? item->second : defaultValue;
}

bool RecalboxConf::AsBool(const std::string& name, bool defaultValue)
{
  auto item = confMap.find(name);
  return (item != confMap.end()) ? (item->second.size() == 1 && item->second[0] == '1') : defaultValue;
}

unsigned int RecalboxConf::AsUInt(const std::string& name, unsigned int defaultValue)
{
  auto item = confMap.find(name);
  if (item != confMap.end())
  {
    long long int value;
    if (Strings::ToLong(item->second, value))
      return (unsigned int)value;
  }

  return defaultValue;
}

int RecalboxConf::AsInt(const std::string& name, int defaultValue)
{
  auto item = confMap.find(name);
  if (item != confMap.end())
  {
    int value;
    if (Strings::ToInt(item->second, value))
      return value;
  }

  return defaultValue;
}

void RecalboxConf::SetString(const std::string& name, const std::string& value)
{
  confMap[name] = value;
}

void RecalboxConf::SetBool(const std::string& name, bool value)
{
  confMap[name] = value ? "1" : "0";
}

void RecalboxConf::SetUInt(const std::string& name, unsigned int value)
{
  confMap[name] = std::to_string(value);
}

void RecalboxConf::SetInt(const std::string& name, unsigned int value)
{
  confMap[name] = std::to_string(value);
}

void RecalboxConf::SetList(const std::string& name, const std::vector<std::string>& values)
{
  confMap[name] = Strings::Join(values, ",");
}

bool RecalboxConf::isInList(const std::string& name, const std::string& value)
{
  bool result = false;
  if (confMap.count(name) != 0u)
  {
    std::string s = confMap[name];
    std::string delimiter = ",";

    size_t pos = 0;
    std::string token;
    while (((pos = s.find(delimiter)) != std::string::npos))
    {
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

