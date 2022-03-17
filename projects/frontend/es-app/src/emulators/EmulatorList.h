//
// Created by bkg2k on 01/11/2019.
//
#pragma once

#include "utils/Log.h"
#include "EmulatorDescriptor.h"

class EmulatorList
{
  public:
    static constexpr int sMaximumEmulators = 4;

  private:
    int mEmulatorCount;
    EmulatorDescriptor mEmulators[sMaximumEmulators];

    static EmulatorDescriptor sEmptyEmulator;

  public:
    EmulatorList()
      : mEmulatorCount(0)
    {
    }

    int Count() const { return mEmulatorCount; }

    const EmulatorDescriptor& EmulatorAt(int index) const { return (unsigned int)index < (unsigned int)mEmulatorCount ? mEmulators[index] : sEmptyEmulator; }

    const EmulatorDescriptor& Named(const std::string& name) const
    {
      for(int i=mEmulatorCount; --i>=0; )
        if (name == mEmulators[i].Name())
          return mEmulators[i];
      return sEmptyEmulator;
    }

    bool HasAny() const { return mEmulatorCount != 0; }

    bool HasNamed(const std::string& name) const
    {
      for(int i=mEmulatorCount; --i>=0; )
        if (name == mEmulators[i].Name())
          return true;
      return false;
    }

    void Clear() { mEmulatorCount = 0; }

    void AddEmulator(const EmulatorDescriptor& emulatorDescriptor)
    {
      if (mEmulatorCount < sMaximumEmulators)
        mEmulators[mEmulatorCount++] = emulatorDescriptor;
      else {LOG(LogError) << "[Emulator] " << emulatorDescriptor.Name() << " cannot be added to current system."; }
    }
};


