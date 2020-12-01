#pragma once

#include "games/FileData.h"

class EmptyData : public FileData
{
  public:
    explicit EmptyData(SystemData* system): FileData(ItemType::Empty, Path::Root(), system->MasterRoot()) {}
};


