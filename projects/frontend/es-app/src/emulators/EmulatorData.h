//
// Created by Bkg2k on 19/02/2020.
//
#pragma once

#include <string>

struct EmulatorData
{
  private:
    //! Emulator
    std::string mEmulator;
    //! Core
    std::string mCore;

  public:
    /*!
     * @brief Build an EmulatorData with the given emulator name and core name
     * @param emulator Emulator name
     * @param core Core name
     */
    EmulatorData(const std::string& emulator, const std::string& core)
      : mEmulator(emulator),
        mCore(core)
    {
    }

    /*
     * Accessors
     */

    //! Get emulator
    const std::string& Emulator() const { return mEmulator; }

    //! Get emulator
    const std::string& Core() const { return mCore; }

    //! Valid?
    bool IsValid() const { return !mEmulator.empty() && !mCore.empty(); }
};