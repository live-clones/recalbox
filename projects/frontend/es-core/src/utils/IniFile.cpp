//
// Created by thierry.imbert on 18/02/2020.
//

#include "IniFile.h"

#include <utils/Strings.h>
#include <utils/Files.h>
#include "utils/Log.h"

IniFile::IniFile(const Path& path, const Path& fallbackpath, bool extraSpace)
  : mFilePath(path)
  , mFallbackFilePath(fallbackpath)
  , mExtraSpace(extraSpace)
  , mValid(Load())
{
}

IniFile::IniFile(const Path& path, bool extraSpace)
  : mFilePath(path)
  , mFallbackFilePath()
  , mExtraSpace(extraSpace)
  , mValid(Load())
{
}

bool IniFile::IsValidKeyValue(const std::string& line, std::string& key, std::string& value, bool& isCommented)
{
  static std::string _allowedCharacters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_.-";
  if (!line.empty()) // Ignore empty line
  {
    bool comment = (line[0] == '#');
    if (!comment)
    {
      size_t separatorPos = line.find('=');
      if (separatorPos != std::string::npos) // Expect a key=value line
      {
        key = Strings::Trim(line.substr(0, separatorPos));
        isCommented = (!key.empty() && key[0] == ';');
        if (isCommented) key.erase(0, 1);
        value = Strings::Trim(line.substr(separatorPos + 1));
        if (key.find_first_not_of(_allowedCharacters) == std::string::npos) return true;
        { LOG(LogWarning) << "[IniFile] Invalid key: `" << key << '`'; }
      }
      else { LOG(LogError) << "[IniFile] Invalid line: `" << line << '`'; }
    }
  }
  return false;
}

bool IniFile::Load()
{
  // Load file
  std::string content;
  if (!mFilePath.IsEmpty() && mFilePath.Exists()) content = Files::LoadFile(mFilePath);
  else if (!mFallbackFilePath.IsEmpty() && mFallbackFilePath.Exists()) content = Files::LoadFile(mFallbackFilePath);
  else return false;

  // Split lines
  content = Strings::Replace(content, "\r", "");
  Strings::Vector lines = Strings::Split(content, '\n');

  // Get key/value
  std::string key, value;
  bool comment = false;
  for (std::string& line : lines)
    if (IsValidKeyValue(Strings::Trim(line, " \t\r\n"), key, value, comment))
      if (!comment)
        mConfiguration[key] = value;

  OnLoad();
  return !mConfiguration.empty();
}

static bool MakeBootReadOnly()
{
  return system("mount -o remount,ro /boot") == 0;
}

static bool MakeBootReadWrite()
{
  return system("mount -o remount,rw /boot") == 0;
}

bool IniFile::Save()
{
  // No change?
  if (mPendingWrites.empty() && mPendingDelete.empty()) return true;

  // Load file
  std::string content = Files::LoadFile(mFilePath);

  // Split lines
  content = Strings::Replace(content, "\r", "");
  Strings::Vector lines = Strings::Split(content, '\n');

  // Save new value if exists
  std::string lineKey;
  std::string lineVal;
  std::string equal(mExtraSpace ? " = " : "=");
  for (auto& it : mPendingWrites)
  {
    // Write new kay/value
    std::string key = it.first;
    std::string val = it.second;
    bool lineFound = false;
    bool commented = false;
    for (auto& line : lines)
      if (IsValidKeyValue(Strings::Trim(line, " \t\r\n"), lineKey, lineVal, commented))
        if (lineKey == key)
        {
          line = key.append(equal).append(val);
          lineFound = true;
          break;
        }
    if (!lineFound)
      lines.push_back(key.append(equal).append(val));

    // Move from Pendings to regular Configuration
    mConfiguration[key] = val;
    mPendingWrites.erase(key);
  }

  // Delete (comment) keys
  for (auto& deletedKey : mPendingDelete)
  {
    bool commented = false;
    for (auto& line : lines)
      if (IsValidKeyValue(Strings::Trim(line, " \t\r\n"), lineKey, lineVal, commented))
        if (lineKey == deletedKey)
          if (!commented)
            line = std::string(1, ';').append(deletedKey).append(equal).append(lineVal);
  }
  mPendingDelete.clear();

  // Save new
  bool boot = mFilePath.StartWidth("/boot/");
  if (boot && MakeBootReadWrite()) LOG(LogError) <<"[IniFile] Error remounting boot partition (RW)";
  Files::SaveFile(mFilePath, Strings::Join(lines, '\n').append(1, '\n'));
  if (boot && MakeBootReadOnly()) LOG(LogError) << "[IniFile] Error remounting boot partition (RO)";

  OnSave();
  return true;
}

std::string IniFile::AsString(const std::string& name) const
{
  return ExtractValue(name);
}

std::string IniFile::AsString(const std::string& name, const std::string& defaultValue) const
{
  std::string item = ExtractValue(name);
  return (!item.empty()) ? item : defaultValue;
}

bool IniFile::AsBool(const std::string& name, bool defaultValue) const
{
  std::string item = ExtractValue(name);
  return (!item.empty()) ? (item.size() == 1 && item[0] == '1') : defaultValue;
}

unsigned int IniFile::AsUInt(const std::string& name, unsigned int defaultValue) const
{
  std::string item = ExtractValue(name);
  if (!item.empty())
  {
    long long int value = 0;
    if (Strings::ToLong(item, value))
      return (unsigned int)value;
  }

  return defaultValue;
}

int IniFile::AsInt(const std::string& name, int defaultValue) const
{
  std::string item = ExtractValue(name);
  if (!item.empty())
  {
    int value = 0;
    if (Strings::ToInt(item, value))
      return value;
  }

  return defaultValue;
}

void IniFile::Delete(const std::string& name)
{
  mPendingDelete.insert(name);
}

void IniFile::SetString(const std::string& name, const std::string& value)
{
  mPendingDelete.erase(name);
  mPendingWrites[name] = value;
}

void IniFile::SetBool(const std::string& name, bool value)
{
  mPendingDelete.erase(name);
  mPendingWrites[name] = value ? "1" : "0";
}

void IniFile::SetUInt(const std::string& name, unsigned int value)
{
  mPendingDelete.erase(name);
  mPendingWrites[name] = Strings::ToString((long long)value);
}

void IniFile::SetInt(const std::string& name, int value)
{
  mPendingDelete.erase(name);
  mPendingWrites[name] = Strings::ToString(value);
}

void IniFile::SetList(const std::string& name, const std::vector<std::string>& values)
{
  mPendingDelete.erase(name);
  mPendingWrites[name] = Strings::Join(values, ',');
}

bool IniFile::isInList(const std::string& name, const std::string& value) const
{
  bool result = false;
  if (mConfiguration.contains(name))
  {
    std::string s = AsString(name);
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

std::string IniFile::ExtractValue(const std::string& key) const
{
  std::string* item = mPendingWrites.try_get(key);
  if (item == nullptr) item = mConfiguration.try_get(key);
  return (item != nullptr) ? *item : std::string();
}

bool IniFile::HasKeyStartingWith(const std::string& startWidth) const
{
  for (auto& it : mPendingWrites)
    if (Strings::StartsWith(it.first, startWidth))
      return true;

  for (auto& it : mConfiguration)
    if (Strings::StartsWith(it.first, startWidth))
      return true;

  return false;
}

bool IniFile::HasKey(const std::string& key) const
{
  for (auto& it : mPendingWrites)
    if (it.first == key)
      return true;

  for (auto& it : mConfiguration)
    if (it.first == key)
      return true;

  return false;
}

Strings::Vector IniFile::GetKeyEndingWith(const std::string& endWidth)
{
  Strings::Vector result;
  for (auto& it : mPendingWrites)
    if (Strings::EndsWith(it.first, endWidth))
      result.push_back(it.first);

  for (auto& it : mConfiguration)
    if (Strings::EndsWith(it.first, endWidth))
      result.push_back(it.first);

  return result;
}