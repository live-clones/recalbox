//
// Created by bkg2k on 30/04/2020.
//

#include "Validator.h"
#include <utils/Log.h>

bool Validator::Validate(std::string& value) const
{
  switch (mType)
  {
    case Types::StringFree: return true;
    case Types::StringConstrained:
    {
      for(int i = value.size(); --i >= 0; )
        if (mList.find(value[i]) == std::string::npos)
          return false;
      return true;
    }
    case Types::StringPicker:
    {
      size_t pos = mList.find(value);
      if (pos != std::string::npos)
        if ((pos == 0) || (mList[pos - 1] == '|'))
          if ((pos + value.size() >= mList.size()) || (mList[pos + value.size()] == '|'))
            return true;
      return false;
    }
    case Types::StringMultiPicker:
    {
      for(const std::string& v : Strings::Split(value, ','))
      {
        size_t pos = mList.find(v);
        if (pos == std::string::npos) return false;
        if ((pos != 0) && (mList[pos - 1] != '|')) return false;
        if ((pos + value.size() < mList.size()) && (mList[pos + value.size()] != '|')) return false;
      }
      return true;
    }
    case Types::IntRange:
    {
      int intValue = 0;
      if (Strings::ToInt(value, intValue))
        return (intValue >= mLower && intValue <= mHigher);
      return false;
    }
    case Types::Bool:
    {
      if (value.length() == 1)
        return (value[0] == '0' || value[0] == '1');
      value = Strings::ToLowerASCII(value);
      if (value == "true") value = "1";
      else if (value == "false") value = "0";
      return (value[0] == '0' || value[0] == '1');
    }
    default: break;
  }

  LOG(LogError) << "Unknown type";
  return false;
}

const char* Validator::TypeAsString() const
{
  switch(mType)
  {
    case Types::StringFree: return "string";
    case Types::StringConstrained: return "constrainedString";
    case Types::StringPicker: return "stringList";
    case Types::StringMultiPicker: return "stringListMulti";
    case Types::IntRange: return "intRange";
    case Types::Bool: return "boolean";
    default: break;
  }
  return "Unknown";
}

Strings::Vector Validator::StringList() const
{
  if (mType == Types::StringPicker ||
      mType == Types::StringMultiPicker)
    return Strings::Split(Strings::Trim(mList, "|"), '|');
  return Strings::Vector();
}

Strings::Vector Validator::DisplayList() const
{
  if (mType == Types::StringPicker ||
      mType == Types::StringMultiPicker)
    return Strings::Split(Strings::Trim(mDisplay, "|"), '|');
  return Strings::Vector();
}

std::vector<int> Validator::IntList() const
{
  std::vector<int> result;
  if (mType == Types::StringConstrained)
  {
    result.resize((int)(mList.size() / sizeof(int)));
    int* list = (int*) mList.data();
    for (int i = (int)(mList.size() / sizeof(int)); --i >= 0;)
      result[i] = list[i];

  }
  return result;
}

std::string Validator::StringConstraint() const
{
  if (mType == Types::StringConstrained) return mList;
  return std::string();
}

