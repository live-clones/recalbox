//
// Created by bkg2k on 16/01/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

class SystemData;

enum class SystemSorting
{
  Default,                                    //!< Default sorting as defined in the xml file
  Name,                                       //!< Sort by system name (not including manufacturer)
  ReleaseDate,                                //!< Sort by release date of the original hardware
  SystemTypeThenName,                         //!< Sort by system type (console, computer, ...), then by name
  SystemTypeThenReleaseDate,                  //!< Sort by system type (console, computer, ...), then by release data
  ManufacturerThenName,                       //!< Sort by manufacturer name, then system name
  ManufacturerThenReleaseData,                //!< Sort by manufacturer name, then system name
  SystemTypeThenManufacturerThenName,         //! Sort by type, then manufacturer, then name
  SystemTypeThenManufacturerThenReleasdeDate, //! Sort by type, then manufacturer, then release date
};

//! Sort by name only
struct SortingName
{
  bool operator()(const SystemData* a, const SystemData* b) const;
};

//! Sort by release date only
struct SortingReleaseDate
{
  bool operator()(const SystemData* a, const SystemData* b) const;
};

//! Sort by type then by name
struct Sorting1Type2Name
{
  bool operator()(const SystemData* a, const SystemData* b) const;
};

//! Sort by type then by release date
struct Sorting1Type2ReleaseDate
{
  bool operator()(const SystemData* a, const SystemData* b) const;
};

//! Sort by manufacturer then by name
struct Sorting1Manufacturer2Name
{
  bool operator()(const SystemData* a, const SystemData* b) const;
};

//! Sort by manufacturer then by release date
struct Sorting1Manufacturer2ReleaseDate
{
  bool operator()(const SystemData* a, const SystemData* b) const;
};

//! Sort by type then by manufacturer then by name
struct Sorting1Type2Manufacturer3Name
{
  bool operator()(const SystemData* a, const SystemData* b) const;
};

//! Sort by type then by manufacturer then by release date
struct Sorting1Type2Manufacturer3ReleaseDate
{
  bool operator()(const SystemData* a, const SystemData* b) const;
};

