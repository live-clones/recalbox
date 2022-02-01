//
// Created by bkg2k on 30/10/2020.
//
#pragma once

#include <string>
#include "AlsaMixer.h"

class OdroidAdvanceGo2Alsa : public AlsaMixer
{
  public:
    static constexpr const char* sOff = "Mute (no sound)";
    static constexpr const char* sSpeaker = "Internal Speakers";
    static constexpr const char* sHeadphone = "Headphone Jack";
    static constexpr const char* sBoth = "Internal Speakers + Headphone Jack";

    enum class OutputPath
    {
      Off,       //!< No sound
      Speaker,   //!< Internal Speaker
      Headphone, //!< Headphone jack
      Both,      //!< Both outputs
    };

    //! Default constructor
    OdroidAdvanceGo2Alsa(int id, const std::string& name, int cardReference)
      : AlsaMixer(id, name, cardReference, MixerType::OdroidAdvanceGo2Path)
    {
    }

    //! Route sound to the given output
    void Route(OutputPath path);
};

