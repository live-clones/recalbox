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

      [[nodiscard]] std::string ToRawString() const
      {
        return std::to_string(Width)
               .append(1, 'x')
               .append(std::to_string(Height));
      }

      [[nodiscard]] std::string ToString() const
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
    const ResolutionList& ResolutionsDetailed() { int w = 0, h = 0; GetMaximumResolution(w, h); return GetResolutionDetailedList(w, h); }

    //! Resolution list simplified: one entry by couple (width, height)
    const ResolutionList& Resolutions() { return GetResolutionList(); }

    //! Default resolution, replacing the desktop resolution
    Resolution DefaultResolution() { return GetDefaultResolution(); }

private:
    ResolutionList mResolutionsDetailed;
    ResolutionList mResolutions;

    /*!
     * @brief Get resolution list under maxWidth & maxHeight
     * use 0 to ignore one or both max values
     * if no resolution is availaiable under the given maximum, return all available resolution
     * @param maxWidth Maximum width (0 to ignore)
     * @param maxHeight Maximum height (0 to ignore)
     * @return Resolution list
     */
    const ResolutionList& GetResolutionDetailedList(int maxWidth, int maxHeight);

    //! Get resolution list
    const ResolutionList& GetResolutionList();

    //! Get resolution list
    Resolution GetDefaultResolution();

    /*!
     * @brief Get maximum resolution regarding the current board
     * @param w Output maximum with
     * @param h Output maximum height
     */
    static void GetMaximumResolution(int& w, int& h);
};



