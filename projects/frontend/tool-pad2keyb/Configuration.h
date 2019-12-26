//
// Created by bkg2k on 22/12/2019.
//
#pragma once

#include "PadConstants.h"

struct Configuration
{
  const char* RomPath;                              //!< Rom path
  const char* PadDevice[PadConstants::MaxPadSupported]; //!< Device path (/dev/xxxx)
  const char* PadName[PadConstants::MaxPadSupported];   //!< Device name
  const char* PadGUID[PadConstants::MaxPadSupported];   //!< Device GUID
  bool Debug;                                       //!< Debug flag

  /*!
   * @brief Constructor
   */
  Configuration()
    : PadDevice {},
      PadName {},
      PadGUID {},
      Debug(false)
  {
    RomPath = nullptr;
    for(int i = PadConstants::MaxPadSupported; --i >= 0; )
      PadDevice[i] = PadName[i] = PadGUID[i] = nullptr;
  }

  /*!
   * @brief Check if the configuratio is valid
   * @return True if the configuration is valid
   */
  bool Valid() const
  {
    // RomPath madatory
    if (RomPath == nullptr) return false;
    // Check every device is either full defined or not defined at all
    for(int i = PadConstants::MaxPadSupported; --i >= 0; )
    {
      // All values defined: valid
      if ((PadDevice[i] != nullptr) && (PadName[i] != nullptr) && (PadGUID[i] != nullptr)) continue;
      // All values undefined: valid
      if ((PadDevice[i] == nullptr) && (PadName[i] == nullptr) && (PadGUID[i] == nullptr)) continue;
      // At least one null value; invalid
      return false;
    }
    return true;
  }

  bool Valid(int index) const
  {
    if ((unsigned int)index >= PadConstants::MaxPadSupported) return false;
    return ((PadDevice[index] != nullptr) && (PadName[index] != nullptr) && (PadGUID[index] != nullptr));
  }
};
