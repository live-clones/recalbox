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
    int mCoreCount;
    std::string mCores[sMaximumCores];

  public:
    EmulatorDescriptor()
      : mCoreCount(0)
    {
    }

    explicit EmulatorDescriptor(const std::string& emulator)
      : mEmulator(emulator),
        mCoreCount(0)
    {
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

    const std::string& Core(int index) const { return (unsigned int)index < (unsigned int)mCoreCount ? mCores[index] : mCores[0]; }

    void AddCore(const std::string& core)
    {
      if (mCoreCount < sMaximumCores)
        mCores[mCoreCount++] = core;
      else
        LOG(LogError) << "Core " << core << " cannot be added to emulator " << mEmulator;
    }
};

