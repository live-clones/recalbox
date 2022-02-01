//
// Created by gugue_u on 10/02/2022.
//

#pragma once

#include "Collections.h"

Collections::Collections()
{
  Deserialize();
}

std::vector<SmartCollection> Collections::Deserialize()
{
  for (Path& file : Path(SmartCollection::sXmlPath).GetDirectoryContent())
  {
    XmlDocument document;
    std::string xlsPath = file.ToString();
    XmlResult result = document.load_file(xlsPath.c_str());
    if (!result)
    {
      { LOG(LogError) << "[SearchCollection] Could not parse " << xlsPath << " file!"; }
    }

    XmlNode collections = document.child("collections");

    if (collections != nullptr)
    {
      for (XmlNode& collectionNode : collections.children("collection"))
      {
        SmartCollection collection(collectionNode);
        mSmartCollections.insert(mSmartCollections.begin(), collection);
      }
    }
  }}
