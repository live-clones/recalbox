//
// Created by pre2010-02 on 10/03/23.
//


#pragma once

#include <string>
#include "games/FolderData.h"

class SearchForcedOptions
{
  public:
    explicit SearchForcedOptions(std::string& searchText, FolderData::FastSearchContext context)
      : mSearchText(searchText),
        mContext(context)
    {}

    std::string mSearchText;
    FolderData::FastSearchContext mContext;
};

