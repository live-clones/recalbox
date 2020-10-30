//
// Created by bkg2k on 13/08/2020.
//
#pragma once

#include <string>
#include "AlsaMixer.h"

class AlsaVolume : public AlsaMixer
{
  public:
    //! Default constructor
    AlsaVolume(int id, const std::string& name, int cardReference)
      : AlsaMixer(id, name, cardReference, MixerType::Volume)
    {
    }

    /*!
     * @brief Set volume
     * @param volume volume to set from 0 to 100 (clamped if OOB)
     */
    void SetVolume(int volume);

    /*!
     * @brief Get current volume in 0-100 range
     */
    int GetVolume();
};
