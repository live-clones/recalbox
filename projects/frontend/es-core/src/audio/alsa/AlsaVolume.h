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

  private:
    /*!
     * @brief Convert percent volume into a raw volune
     * @param percent Percent value ton convert
     * @param min Min raw volume
     * @param max Max raw volume
     * max-min gives the raw volume range
     * @return raw volume between min and max
     */
    static long PercentToVolume(double percent, long min, long max);

    /*!
     * @brief Convert raw volume between min and max into volument percent
     * @param raw Raw volume to convert
     * @param min Min raw volume
     * @param max Max raw volume
     * max-min gives the raw volume range
     * @return Volume percent
     */
    static int VolumeToPercent(double raw, long min, long max);
};
