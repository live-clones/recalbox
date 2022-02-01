//
// Created by gugue_u on 06/04/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <games/IFilter.h>
#include "SystemData.h"

class SmartCollection : public IFilter
{
  public:
    //! Constructor
    explicit SmartCollection(XmlNode& collection);

    //! Xml path
    static constexpr const char* sXmlPath = "/recalbox/share/roms/collections/";

    bool ContainsSystem(const SystemData& system) const
    {
      return mSystemLists.empty() ||  mSystemLists.contains(system.Name());
    }

    std::string Identifier() const { return mIdentifier; }

    std::string FullName() const { return mFullName; }

    std::string NameFilter() const { return  mNameFilter; }

    std::string DescriptionFilter() const { return mDescriptionFilter; }

    std::string PlatformFilter() const { return mPlatformFilter; }

    std::string EditorFilter() const { return  mEditorFilter; }

    std::string RegionFilter() const { return mRegionFilter; }

    FileSorts::Sorts Sort() const { return mSort; }

  private:

    std::string mIdentifier;
    std::string mFullName;
    std::string mNameFilter = Strings::Empty;
    std::string mDescriptionFilter = Strings::Empty;
    std::string mPlatformFilter = Strings::Empty;
    std::string mEditorFilter = Strings::Empty;
    std::string mRegionFilter = Strings::Empty;
    FileSorts::Sorts mSort = FileSorts::Sorts::SystemAscending;

    //! All game list
    HashMap<std::string, HashSet<std::string>> mSystemLists;

    /*
     * IFilter implementation
     */

    //! Return true if the given FileGata math a game in the current database system
    bool ApplyFilter(const FileData& file) const override;
};



