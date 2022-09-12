//
// Created by bkg2k on 15/03/2022.
//
#pragma once

#include <string>
#include <vector>
#include "Resolutions.h"

class ResolutionAdapter
{
  public:
    // Resolution holder
    struct Resolution
    {
      int Display;      //!< Display index
      int Width;        //!< Resolution width
      int Height;       //!< Resolution height
      int Bpp;          //!< Bit per plane (color deepness)
      int Frequency;    //!< Frequency (Hz)
      bool Interlaced;  //!< Interlaced mode
      bool IsDefault;   //!< True if this is the default resolution (Desktop)

      std::string ToRawString() const
      {
        return std::to_string(Width)
               .append(1, 'x')
               .append(std::to_string(Height));
      }

      std::string ToString() const
      {
        return std::string("Display ")
          .append(std::to_string(Display))
          .append(": ")
          .append(std::to_string(Width))
          .append(1, 'x')
          .append(std::to_string(Height))
          .append(" - ")
          #ifdef USE_KMSDRM
          .append(Interlaced ? "interlaced " : "progressive ")
          #else
          //.append(std::to_string(Bpp))
          //.append("Bpp ")
          #endif
          .append(std::to_string(Frequency))
          .append("Hz Max")
          .append(IsDefault ? " (Default)" : "");
      }
    };
    typedef std::vector<Resolution> ResolutionList;

    //! Adjust the given resolution to the best matching available resolution
    bool AdjustResolution(int display, const std::string& value, Resolutions::SimpleResolution& output);

    //! Resolution list with bpm and frequency
    const ResolutionList& ResolutionsDetailed() { return GetResolutionDetailedList(); }

    //! Resolution list simplified: one entry by couple (width, height)
    const ResolutionList& Resolutions() { return GetResolutionList(); }

    //! Default resolution, replacing the desktop resolution
    const Resolution DefaultResolution() { return GetDefaultResolution(); }



private:
    ResolutionList mResolutionsDetailed;
    ResolutionList mResolutions;

    //! Get resolution list
    const ResolutionList& GetResolutionDetailedList();

    //! Get resolution list
    const ResolutionList& GetResolutionList();

    //! Get resolution list
    const Resolution GetDefaultResolution();
};



