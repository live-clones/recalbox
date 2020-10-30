//
// Created by bkg2k on 30/10/2020.
//
#pragma once

#include <string>
#include "AlsaMixer.h"

class OdroidAdvanceGo2 : public AlsaMixer
{
  public:
    enum class OutputPath
    {
      Off,       //!< No sound
      Speaker,   //!< Internal Speaker
      Headphone, //!< Headphone jack
      Both,      //!< Both outputs
    };

    //! Default constructor
    OdroidAdvanceGo2(int id, const std::string& name, int cardReference)
      : AlsaMixer(id, name, cardReference, MixerType::OdroidAdvanceGo2Path)
    {
    }

    //! Route sound to the given output
    void Route(OutputPath path);
};

