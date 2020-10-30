//
// Created by bkg2k on 13/08/2020.
//
#pragma once

#include <string>
#include "AlsaMixer.h"

class AlsaSwitch : public AlsaMixer
{
  public:
    //! Default constructor
    AlsaSwitch(int id, const std::string& name, int cardReference)
      : AlsaMixer(id, name, cardReference, MixerType::Switch)
    {
    }

    /*!
     * @brief Switch on this mixer
     */
    void SwitchOn();
};
