//
// Created by bkg2k on 30/04/2020.
//
#pragma once

#include <string>
#include <utils/Strings.h>
#include <utils/storage/HashMap.h>

class Validator
{
  public:
    //! Configuration Datatype
    enum class Types
    {
      StringFree,        //!< Free String
      StringConstrained, //!< Chars-restricted string
      StringPicker,      //!< String value among a pre-defined list
      StringMultiPicker, //!< String value among a pre-defined list
      IntRange,          //!< Numeric value among a pre-defined range
      Bool,              //!< Boolean
    };

  private:
    //! Data type
    Types mType;

    //! String/Int list
    std::string mList;
    //! String/Int display info
    std::string mDisplay;
    //! Int range lower value
    int mLower;
    //! Int range higher value
    int mHigher;

  public:
    /*!
     * @brief Default constructor: Free string
     */
    Validator()
      : mType(Types::StringFree),
        mLower(0),
        mHigher(0)
    {
    }

    /*!
     * @brief Char restricted string
     */
    explicit Validator(const char* charList)
      : mType(Types::StringConstrained),
        mList(charList),
        mLower(0),
        mHigher(0)
    {
    }

    /*!
     * @brief String list constructor
     * @param list string list
     */
    explicit Validator(bool multi, const std::vector<const char*>&  list)
      : mType(multi ? Types::StringMultiPicker : Types::StringPicker),
        mList('|' + Strings::Join(list, "|")),
        mLower(0),
        mHigher(0)
    {
    }

    /*!
     * @brief String list constructor
     * @param list string list
     */
    Validator(const std::vector<std::string>&  list, bool multi)
      : mType(multi ? Types::StringMultiPicker : Types::StringPicker),
        mList('|' + Strings::Join(list, "|")),
        mLower(0),
        mHigher(0)
    {
    }

    /*!
     * @brief String list constructor w/ display info
     * @param list string list
     */
    explicit Validator(const HashMap<std::string, std::string>&  map, bool multi)
      : mType(multi ? Types::StringMultiPicker : Types::StringPicker),
        mLower(0),
        mHigher(0)
    {
      mList = '|';
      mDisplay = '|';
      for(const auto& item : map)
      {
        mList.append(item.first).append(1, '|');
        mDisplay.append(item.second).append(1, '|');
      }
    }

    /*!
     * @brief Int range constructor
     * @param from lower range value
     * @param to higher range value
     */
    explicit Validator(int from, int to)
      : mType(Types::IntRange),
        mLower(from),
        mHigher(to)
    {
    }

    /*!
     * @brief Bool constructor
     * @param list
     */
    explicit Validator(bool)
      : mType(Types::Bool),
        mLower(0),
        mHigher(0)
    {
    }

    /*!
     * @brief Validate the given string
     * @param value String to validate
     * @param outvalue Normalized output value
     * @return True if the string is valid, false otherwise
     */
    bool Validate(std::string& value) const;

    /*
     * Getters
     */

    //! Get type
    Types Type() const { return mType; }

    //! Get type as string
    const char* TypeAsString() const;

    //! Get validation data
    std::string StringConstraint() const;
    //! Get validation data
    Strings::Vector StringList() const;
    //! Get validation data
    std::vector<int> IntList() const;

    //! Has display info?
    bool HasDisplay() const { return !mDisplay.empty(); }
    //! Get display info
    Strings::Vector DisplayList() const;

    //! Get int lower range value
    int Lower() const { return mLower; }
    //! Get int lower range value
    int Higher() const { return mHigher; }
};
