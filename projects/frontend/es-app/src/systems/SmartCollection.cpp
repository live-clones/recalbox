//
// Created by gugue_u on 06/04/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include "SmartCollection.h"


/*<collections>
  <collection identifier="zelda" fullname="zelda">
    <filters>
      <nameFilter>zelda,link</nameFilter>
    </filters>
  </collection>

  <collection identifier="sailormoon" fullname="sailor moon">
    <filters>
      <nameFilter>sailor moon</nameFilter>
    </filters>
  </collection>
  <collection identifier="dragonball" fullname="dragonball">
    <filters>
      <nameFilter>dragon ball</nameFilter>
      <regionFilter>jp</regionFilter>
    </filters>
  </collection>
</collections>*/

SmartCollection::SmartCollection(XmlNode& collection)
{
      if (collection == nullptr)
        return;

      mIdentifier = Xml::AttributeAsString(collection, "identifier", "");
      mFullName = Xml::AttributeAsString(collection, "fullname", "");

      XmlNode filters = collection.child("filters");
      mNameFilter = Xml::AsString(filters, "nameFilter", Strings::Empty);
      mDescriptionFilter = Xml::AsString(filters, "descriptionFilter", Strings::Empty);
      mPlatformFilter = Xml::AsString(filters, "platformFilter", Strings::Empty);
      mRegionFilter = Xml::AsString(filters, "regionFilter", Strings::Empty);

      for(auto& platefrom : Strings::Split(mPlatformFilter, ','))
         mSystemLists[platefrom];

      { LOG(LogDebug) << "[SmartCollection] Load collection: " << mFullName; }
}

bool SmartCollection::ApplyFilter(const FileData& file) const
{
  bool nameMatch = false;
  bool descMatch = false;
  bool platformMatch = false;
  bool regionMatch = false;
  for(auto& name : Strings::Split(mNameFilter, ','))
   if(Strings::Contains(Strings::ToLowerASCII(file.Name()), Strings::ToLowerASCII(name)))
     nameMatch = true;
  if(mNameFilter == Strings::Empty) nameMatch = true;

  for(auto& desc : Strings::Split(mDescriptionFilter, ','))
    if(Strings::Contains(Strings::ToLowerASCII(file.Metadata().Description()), Strings::ToLowerASCII(desc)))
      descMatch = true;
  if(mDescriptionFilter == Strings::Empty) descMatch = true;


  for(auto& platform : Strings::Split(mPlatformFilter, ','))
    if (Strings::Contains(Strings::ToLowerASCII(file.System().Name()), Strings::ToLowerASCII(platform)))
      platformMatch = true;

  if(mPlatformFilter == Strings::Empty) platformMatch = true;

  for(auto& region : Strings::Split(mRegionFilter, ','))
  {
    Regions::GameRegions f = Regions::ExtractRegionsFromFileName(Path(file.FilePath()));
    Regions::GameRegions c = Regions::DeserializeRegion(region);

    if( c == f)
      regionMatch = true;
  }

  if(mRegionFilter == Strings::Empty) regionMatch = true;

  return nameMatch && descMatch && platformMatch && regionMatch;
}
