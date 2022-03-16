//
// Created by bkg2k on 15/03/2022.
//
#pragma once

#include <utils/storage/HashMap.h>

class Resolutions
{
  public:
    //! Simple resolution holder
    struct SimpleResolution
    {
      int Width;  //!< Resolution width
      int Height; //!< Resolution width

      //! Is resolution valid?
      bool IsValid() const { return (Width * Height) > 0; }
    };

    /*!
     * @brief Convert CEA, DMT, named or WxH resolution into a simple resolution
     * @param resolution Input resolution
     * @return Simple resolution
     */
    static SimpleResolution ConvertSimpleResolution(const std::string& resolution);

  private:
    //! CEA index to resolution
    static const HashMap<int, SimpleResolution>& GetCeaMap();
    //! DMT index to resolution
    static const HashMap<int, SimpleResolution>& GetDmtMap();
    //! Named resolution to resolutions
    static const HashMap<std::string, SimpleResolution>& GetNamedMap();
};



