//
// Created by bkg2k on 28/09/2020.
//
#pragma once

#include <platform_gl.h>

class Colors
{
  public:
    //! ARGB
    typedef unsigned int ColorARGB;
    //! ARGB
    typedef unsigned int ColorRGBA;
    //! Component
    typedef unsigned char Component;

    static constexpr ColorARGB None = 0x00000000;

    /*!
     * @brief Get Alpha component from an ARGB color
     * @param color Input Color
     * @return Alpha component
     */
    static Component Alpha(ColorARGB color) { return color >> 24; }
    /*!
     * @brief Get Red component from an ARGB color
     * @param color Input Color
     * @return Red component
     */
    static Component Red(ColorARGB color) { return (color >> 16) & 0xFF; }
    /*!
     * @brief Get Green component from an ARGB color
     * @param color Input Color
     * @return Green component
     */
    static Component Green(ColorARGB color) { return (color >> 8) & 0xFF; }
    /*!
     * @brief Get Blue component from an ARGB color
     * @param color Input Color
     * @return Blue component
     */
    static Component Blue(ColorARGB color) { return color & 0xFF; }
    /*!
     * @brief Get pure color RGB, w/o Alpha
     * @param color Input color
     * @return Pure color
     */
    static ColorARGB PureColor(ColorARGB color) { return color & 0xFFFFFF; }

    /*!
     * @brief Check if the alpha component of the given color is not zero
     * @param color Color to check
     * @return True if the alpha component is > 0
     */
    static bool IsVisible(ColorARGB color) { return (color >> 24) != 0; }

    /*!
     * @brief Convert ARGB to RGBA
     * @param argb source ARGB
     * @return RGBA conversion
     */
    static ColorRGBA ARGBToABGR(ColorARGB argb)
    {
      unsigned int bgra = __builtin_bswap32(argb);
      return (argb & 0xFF00FF00) | ((bgra & 0xFF00FF00) >> 8);
    }

    /*!
     * @brief Convert the given ARGB color to ABGR and store RGBA component into a GL byte array
     * @param destination Destination GL byte array
     * @param argb Source ARGB color
     */
    static void ARGBToGLColoArray(GLubyte* destination, ColorARGB argb)
    {
      *((unsigned int*)destination) = ARGBToABGR(argb);
    }

    /*!
     * @brief Convert the given ARGB color to ABGR and store the 4 components X times into the destination array
     * @param destination Destination array
     * @param argb Source ARGB color
     * @param count Number of time to repeat the color in the destination array
     */
    static void BuildGLColorArray(GLubyte* destination, ColorARGB argb, int count)
    {
      ColorRGBA* glrgba = (ColorRGBA*)destination;
      ColorRGBA rgba = ARGBToABGR(argb);
      for(int i = count; --i >= 0; )
        glrgba[i] = rgba;
    }
};
