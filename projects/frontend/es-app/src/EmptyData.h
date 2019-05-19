#pragma once

#include "FileData.h"

class EmptyData : public FileData
{
  public:
    EmptyData(SystemData* system): FileData(ItemType::Empty, "/", system) {}
};


