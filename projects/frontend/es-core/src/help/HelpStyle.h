#pragma once

#include "utils/math/Vector2f.h"
#include "Help.h"
#include <memory>
#include <string>
#include <utils/os/fs/Path.h>

class ThemeData;
class Font;

class HelpStyle
{
  private:
    //! Images for every single help type
    Path mImagesPath[(int)HelpType::__Count + 1];
    //! Text font
    std::shared_ptr<Font> mFont;
    //! Position of help items
    Vector2f mPosition;
    //! Icon color
    unsigned int mIconColor;
    //! Text color
    unsigned int mTextColor;

  public:
    /*!
     * @brief Default constructor
     */
    HelpStyle();

    /*!
     * @brief Fill the style using the given theme
     * @param theme Theme data
     * @param view View from which to extract data
     */
  	void FromTheme(const ThemeData& theme, const std::string& view);

    /*
     * Accessors
     */

    /*!
     * @brief Return the imag epath for the given help type
     * @param type Help type
     * @return Path
     */
    const Path& ImagePath(HelpType type) const
    {
      if ((unsigned int)type >= (unsigned int)HelpType::__Count) return mImagesPath[(int)HelpType::__Count];
      return mImagesPath[(int)type];
    }

    //! Get font object
    const std::shared_ptr<Font>& TextFont() const { return mFont; }

    //! Get position
    const Vector2f& Position() const { return mPosition; }

    //! Get Icon color
    unsigned int IconColor() const { return mIconColor; }

    //! Get Text color
    unsigned int TextColor() const { return mTextColor; }
};