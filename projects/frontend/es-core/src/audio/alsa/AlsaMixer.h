//
// Created by bkg2k on 13/08/2020.
//
#pragma once

#include <string>
#include <alsa/asoundlib.h>

class AlsaMixer
{
  public:
    //! Mixer type
    enum class MixerType
    {
        Volume,               //!< Volume settings
        Switch,               //!< Switch output on/off
        OdroidAdvanceGo2Path, //!< Special object for Odroid Advance Go 2
    };

    //! Default constructor
    AlsaMixer(int id, const std::string& name, int cardReference, MixerType mixerType)
      : mMixerHandle(nullptr)
      , mMixerName(name)
      , mMixerId(id)
      , mCardReference(cardReference)
      , mMixerType(mixerType)
    {
    }

    //! Get mixer identifier
    int Identifier() const { return mMixerId; }
    //! Get mixer name
    const std::string& Name() const { return mMixerName; }
    //! Get mixer type
    MixerType Type() const { return mMixerType; }

  protected:
    //! Mixer objects
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

  private:
    // Internal references
    std::string mMixerName; //!< Mixer name
    int mMixerId;           //!< Mixer ID
    int mCardReference;     //!< Reference index of the parent sound provider
    MixerType mMixerType;   //!< Mixer type
};
