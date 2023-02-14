//
// Created by gugue_u on 04/01/2023.
//

#pragma once

#include <string>

class SaveStateData
{
  public:
    SaveStateData()
      : mSlotConfigured(false)
    {};

    std::string SlotNumber() const { return mSlotNumber; }

    void SetSlotNumber(std::string slotNumber)
    {
      mSlotNumber = slotNumber;
      mSlotConfigured = true;
    }

    bool IsConfigured()
    {
      return mSlotConfigured;
    }

  private:
    bool mSlotConfigured;
    std::string mSlotNumber;
};
