//
// Created by bkg2k on 13/08/2020.
//
#pragma once

#include <string>
#include <alsa/asoundlib.h>
#include "NameFiltering.h"

class AlsaMixer
{
  private:
    // Internal references
    std::string mMixerName; //!< Mixer name
    int mMixerId;           //!< Mixer ID
    int mCardReference;     //!< Reference index of the parent sound provider

  protected:
    // Mixer objects
    snd_mixer_t* mMixerHandle;

    /*!
     * @brief Open mixer and return the mixer element
     * @return ALSA mixer element, or nullptr on error
     */
    snd_mixer_elem_t* OpenMixer();

    /*!
     * @brief Close previously opened mixer
     */
    void CloseMixer();

  public:
    //! Default constructor
    AlsaMixer(int id, const std::string& name, int cardReference)
      : mMixerName(name)
      , mMixerId(id)
      , mCardReference(cardReference)
      , mMixerHandle(nullptr)
    {
    }

    //! Get mixer identifier
    int Identifier() const { return mMixerId; }
    //! Get mixer name
    const std::string& Name() const { return mMixerName; }
};
