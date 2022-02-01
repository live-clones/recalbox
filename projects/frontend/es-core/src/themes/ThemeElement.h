//
// Created by bkg2k on 09/11/2019.
//
#pragma once

#include <map>
#include <string>

#include <utils/Strings.h>
#include <utils/math/Vector2f.h>

class ThemeElement
{
  private:
    //! Property type
    enum Type: char
    {
      Vector,
      String,
      Integer,
      Float,
      Bool,
    };
    //! Bag of data
    class PropertyBag
    {
      private:
        std::string mString;
        union // Save some space by union-ing cardinal exclusive values
        {
          int mInteger;
          float mFloat;
          bool mBool;
        };
        float mSecondFloat;
        Type mType;
      public:
        PropertyBag()                              : mInteger(0), mSecondFloat(0.0f), mType(Type::String) {}
        explicit PropertyBag(const Vector2f& v)    : mFloat(v.x()), mSecondFloat(v.y()), mType(Type::Vector) {}
        explicit PropertyBag(float v1, float v2)   : mFloat(v1), mSecondFloat(v2), mType(Type::Vector) {}
        explicit PropertyBag(const std::string& s) : mString(s), mInteger(0), mSecondFloat(0.0f), mType(Type::String) {}
        explicit PropertyBag(int v)                : mInteger(v), mSecondFloat(0.0f), mType(Type::Integer) {}
        explicit PropertyBag(float v)              : mFloat(v), mSecondFloat(0.0f), mType(Type::Float) {}
        explicit PropertyBag(bool v)               : mBool(v), mSecondFloat(0.0f), mType(Type::Bool) {}

        std::string AsString() const
        {
          switch(mType)
          {
            case Type::Vector: return Strings::ToString(mFloat, 4) + ' ' + Strings::ToString(mSecondFloat, 4);
            case Type::String: return mString;
            case Type::Integer: return Strings::ToString(mInteger);
            case Type::Float: return Strings::ToString(mFloat, 4);
            case Type::Bool: return std::string(mBool ? "1" : "0", 1);
          }
          return std::string();
        }
        int AsInt() const
        {
          switch(mType)
          {
            case Type::String: int result; return Strings::ToInt(mString, result) ? result : 0;
            case Type::Integer: return mInteger;
            case Type::Vector:
            case Type::Float: return (int)mFloat;
            case Type::Bool: return (int)mBool;
          }
          return 0;
        }
        float AsFloat() const
        {
          switch(mType)
          {
            case Type::String: float result; return Strings::ToFloat(mString, result) ? result : 0.0f;
            case Type::Integer: return (float)mInteger;
            case Type::Vector:
            case Type::Float: return mFloat;
            case Type::Bool: return (float)mBool;
          }
          return 0.0f;
        }
        bool AsBool() const
        {
          switch(mType)
          {
            case Type::String: bool result; return Strings::ToBool(mString, result) ? result : false;
            case Type::Integer: return (bool)mInteger;
            case Type::Vector:
            case Type::Float: return (bool)mFloat;
            case Type::Bool: return mBool;
          }
          return false;
        }
        Vector2f AsVector() const
        {
          switch(mType)
          {
            case Type::Vector: return { mFloat, mSecondFloat };
            case Type::String:
            {
              float x,y;
              if (Strings::ToFloat(mString, 0, ' ', x))
              {
                size_t pos = mString.find(' ');
                if (pos == std::string::npos)
                  if (Strings::ToFloat(mString, (int) pos + 1, 0, y))
                    return { x, y };
              }
              break;
            }
            case Type::Integer: return { (float)mInteger, (float)mInteger };
            case Type::Float: return { mFloat, mFloat };
            case Type::Bool: return {(float)mBool, (float)mBool};
          }
          return { 0.0f, 0.0f };
        }
    };

    std::map<std::string, PropertyBag> mProperties;
    std::string mType;
    bool mExtra;

  public:
    std::string AsString(const std::string& name) const
    {
      auto it = mProperties.find(name);
      if (it != mProperties.end()) return it->second.AsString();
      return std::string();
    }

    int AsInt(const std::string& name) const
    {
      auto it = mProperties.find(name);
      if (it != mProperties.end()) return it->second.AsInt();
      return 0;
    }

    float AsFloat(const std::string& name) const
    {
      auto it = mProperties.find(name);
      if (it != mProperties.end()) return it->second.AsFloat();
      return 0.0f;
    }

    bool AsBool(const std::string& name) const
    {
      auto it = mProperties.find(name);
      if (it != mProperties.end()) return it->second.AsBool();
      return false;
    }

    Vector2f AsVector(const std::string& name) const
    {
      auto it = mProperties.find(name);
      if (it != mProperties.end()) return it->second.AsVector();
      return { 0.0f, 0.0f };
    }

    bool HasProperty(const std::string& prop) const { return (mProperties.find(prop) != mProperties.end()); }

    bool HasProperties() const { return !mProperties.empty(); }

    const std::string& Type() const { return mType; }

    bool Extra() const { return mExtra; }

    void SetRootData(const std::string& type, bool extra)
    {
      mType = type;
      mExtra = extra;
    }

    void AddVectorProperty(const std::string& name, float x, float y) { mProperties[name] = PropertyBag(x, y); }
    void AddVectorProperty(const std::string& name, const Vector2f& v) { mProperties[name] = PropertyBag(v); }
    void AddStringProperty(const std::string& name, const std::string& s) { mProperties[name] = PropertyBag(s); }
    void AddIntProperty(const std::string& name, int v) { mProperties[name] = PropertyBag(v); }
    void AddFloatProperty(const std::string& name, float v) { mProperties[name] = PropertyBag(v); }
    void AddBoolProperty(const std::string& name, bool v) { mProperties[name] = PropertyBag(v); }
};

