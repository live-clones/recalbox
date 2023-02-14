//
// Created by gugue_u on 13/01/23.
//

#include "utils/os/fs/Path.h"
#include "utils/datetime/DateTime.h"

#pragma once

class SaveState
{
  public:
    explicit SaveState(const Path& path);

    [[nodiscard]] Path GetPath() const { return mPath; }
    [[nodiscard]] Path GetThrumbnail() const { return mThumbnailPath; }
    [[nodiscard]] std::string GetModificationDate() const { return mModificationDate; }
    [[nodiscard]] int GetSlotNumber() const { return mSlotNumber; }
    [[nodiscard]] bool GetIsAuto() const { return mIsAuto; }
    [[nodiscard]] timer_t GetMtimer() const { return mTimer; }

  private:
    Path mPath;
    Path mThumbnailPath;
    std::string mModificationDate;
    timer_t mTimer;
    int mSlotNumber;
    bool mIsAuto;
};