//
// Created by bkg2k on 01/11/2019.
//
#pragma once

#include <utils/Log.h>

class EmulatorDescriptor
{
  public:
    static constexpr int sMaximumCores = 8;

  private:
    std::string mEmulator;
    std::string mCores[sMaximumCores];
    unsigned char mCorePriorities[sMaximumCores];
    int mCoreCount;

  public:
    EmulatorDescriptor()
      : mCorePriorities {},
        mCoreCount(0)
    {
      for(int i = sMaximumCores; --i >= 0; )
        mCorePriorities[i] = 255;
    }

    explicit EmulatorDescriptor(const std::string& emulator)
      : EmulatorDescriptor()
    {
      mEmulator = emulator;
    }

    const std::string& Name() const { return mEmulator; }

    int CoreCount() const { return mCoreCount; }

    bool HasAny() const { return mCoreCount != 0; }

    bool HasCore(const std::string& name) const
    {
      for(int i=mCoreCount; --i>=0; )
        if (name == mCores[i])
          return true;
      return false;
    }

    const std::string& CoreAt(int index) const { return (unsigned int)index < (unsigned int)mCoreCount ? mCores[index] : mCores[0]; }

    unsigned char CorePriorityAt(int index) const { return (unsigned int)index < (unsigned int)mCoreCount ? mCorePriorities[index] : 255; }

    void AddCore(const std::string& core, int priority)
    {
      if (mCoreCount < sMaximumCores)
      {
        mCorePriorities[mCoreCount] = priority;
        mCores[mCoreCount++] = core;
      }
      else
        LOG(LogError) << "Core " << core << " cannot be added to emulator " << mEmulator;
    }
};

