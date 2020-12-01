//
// Created by bkg2k on 11/08/2019.
//
#pragma once

#include "utils/cplusplus/Bitflags.h"

enum class ThemeProperties : unsigned int
{
    Path           = (1 << 0),
    Position       = (1 << 1),
    Size           = (1 << 2),
    Origin         = (1 << 3),
    Color          = (1 << 4),
    FontPath       = (1 << 5),
    FontSize       = (1 << 6),
    Sound          = (1 << 7),
    Alignment      = (1 << 8),
    Text           = (1 << 9),
    ForceUppercase = (1 << 10),
    LineSpacing    = (1 << 11),
    ZIndex         = (1 << 12),
    Rotation       = (1 << 13),
    Display        = (1 << 15),

    All            = 0xFFFFFFFF,
};

DEFINE_BITFLAG_ENUM(ThemeProperties, unsigned int)