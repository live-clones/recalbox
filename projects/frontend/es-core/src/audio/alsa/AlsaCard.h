//
// Created by bkg2k on 13/08/2020.
//
#pragma once

#include <string>
#include <vector>
#include "AlsaMixer.h"
#include "AlsaDevice.h"
#include "NameFiltering.h"

class AlsaCard
{
  private:
    int mCardIdentifier;              //!< Card identifier/index in ALSA
    std::string mCardName;            //!< Displayable Card name
    std::vector<AlsaMixer> mMixers;   //!< Available mixers for this card
    std::vector<AlsaDevice> mDevices; //!< Device list

  public:
    /*!
     * @brief Constructor
     * @param identifier Card index
     * @param name Card name
     */
    AlsaCard(int identifier, const char* name)
    {
      mCardIdentifier = identifier;
      mCardName = NameFiltering::Filter(name, NameFiltering::Source::Card);
    }

    //! Add a new mixer to the mixer list attached to the current card
    void AddMixer(const AlsaMixer& mixer) { mMixers.push_back(mixer); }

    //! Add a new device to the device list attached to the current card
    void AddDevice(const AlsaDevice& device) { mDevices.push_back(device); }

    //! Get card index
    int Identifier() const { return mCardIdentifier; }
    //! Get card name
    const std::string& Name() const { return mCardName; }

    //! Get mixer count
    int MixerCount() const { return mMixers.size(); }
    //! Get mixer reference at position index (from 0 to MixerCount()-1)
    const AlsaMixer& MixerAt(int index) const { return mMixers[index]; }
    //! Get device count
    int DeviceCount() const { return mDevices.size(); }
    //! Get device reference at position index (from 0 to DeviceCount()-1)
    const AlsaDevice& DeviceAt(int index) const { return mDevices[index]; }

    /*!
     * @brief Set volume to all underlaying mixers
     * @param volume volume to set from 0 to 100 (clamped if OOB)
     */
    void SetVolume(int volume);
};
