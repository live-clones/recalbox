#pragma once

#include <string>
#include <stdexcept>
#include "MetadataDescriptor.h"

//! Method pointer to default value getters
typedef bool (MetadataDescriptor::*IsDefaultValueMethodType)() const;
//! Method pointer to string getters
typedef std::string (MetadataDescriptor::*GetValueMethodType)() const;
//! Method pointer to string setters
typedef void (MetadataDescriptor::*SetValueMethodType)(const std::string& value);

class MetadataFieldDescriptor
{
  public:
    //! Data type
    enum class DataType
    {
        // Simple types
        String,  //!< std::string
        PString, //!< Pointer to std::string
        Int,     //!< int
        Bool,    //!< bool
        Float,   //!< float

        // Derived types
        Text,    //!< Multiline text (std::string)
        PList,   //!< Pointer to String list, space separated (std::string)
        Path,    //!< File path (std::string)
        PPath,   //!< Pointer to File path (std::string)
        Rating,  //!< Floating point value between 0.0 and 1.0 (float)
        Date,    //!< Epoc (int)
        Range,   //!< Integer range: LSW:from MSW:to
        Crc32,   //!< 4byte hash (int)
    };

  private:
    std::string               _Key;                  //!< Identifier
    std::string               _DefaultValue;         //!< default value
    std::string               _DisplayName;          //!< displayed as this in editors
    std::string               _DisplayPrompt;        //!< phrase displayed in editors when prompted to enter value (currently only for strings)
    int                       _Offset;               //!< Offset of the real field in the target Metadata structure
    DataType                  _Type;                 //!< Datatype
    IsDefaultValueMethodType  _IsDefaultValueMethod; //!< Is Default value?
    GetValueMethodType        _GetMethod;            //!< String getter
    SetValueMethodType        _SetMethod;            //!< String getter
    bool                      _IsStatistic;          //!< if true, ignore scraper values for this metadata
    bool                      _IsMain;               //!< if true, display on main metadata editor GUI, else in secondary

  public:
    // Public const accessors
    const std::string&        Key()                  const { return _Key;                  } //!< Identifier
    const std::string&        DefaultValue()         const { return _DefaultValue;         } //!< default value
    const std::string&        DisplayName()          const { return _DisplayName;          } //!< displayed as this in editors
    const std::string&        DisplayPrompt()        const { return _DisplayPrompt;        } //!< phrase displayed in editors when prompted to enter value (currently only for strings)
    int                       Offset()               const { return _Offset;               } //!< Offset of the real field in the target Metadata structure
    DataType                  Type()                 const { return _Type;                 } //!< Datatype
    IsDefaultValueMethodType  IsDefaultValueMethod() const { return _IsDefaultValueMethod; } //!< Is Default value?
    GetValueMethodType        GetValueMethod()       const { return _GetMethod;            } //!< String getter
    SetValueMethodType        SetValueMethod()       const { return _SetMethod;            } //!< String setter
    bool                      IsStatistic()          const { return _IsStatistic;          } //!< if true, ignore scraper values for this metadata
    bool                      IsMain()               const { return _IsMain;               } //!< if true, display on main metadata editor GUI, else in secondary

    //! Constructor
    MetadataFieldDescriptor(const std::string&        key,
                            const std::string&        defaultValue,
                            const std::string&        displayName,
                            const std::string&        displayPrompt,
                            int                       offset,
                            DataType                  type,
                            IsDefaultValueMethodType  isDefaultValueMethod,
                            GetValueMethodType        getMethod,
                            SetValueMethodType        setMethod,
                            bool                      isStatistic,
                            bool                      isMain)
      : _Key(key),
        _DefaultValue(defaultValue),
        _DisplayName(displayName),
        _DisplayPrompt(displayPrompt),
        _Offset(offset),
        _Type(type),
        _IsDefaultValueMethod(isDefaultValueMethod),
        _GetMethod(getMethod),
        _SetMethod(setMethod),
        _IsStatistic(isStatistic),
        _IsMain(isMain)
    {
    }
};

