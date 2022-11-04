//
// Created by bkg2k on 20/10/22.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

enum class CrtResolution
{
  rNone, //!< Null element
  r224p, //!< PAL + NTSC 224p - Compatible VGA 120hz
  r240p, //!< PAL + NTSC 224p - Compatible VGA 120hz
  r288p, //!< PAL only
  r480i, //!< NTSC interlaced hires
  r576i, //!< PAL interlaced hires
  r480p, //!< VGA hires
  _rCount, //!< Counter
};
