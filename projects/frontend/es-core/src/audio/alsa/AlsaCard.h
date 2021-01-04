//
// Created by bkg2k on 13/08/2020.
//
#pragma once

#include <string>
#include <vector>
#include "AlsaDevice.h"
#include "audio/NameFiltering.h"
#include "AlsaVolume.h"
#include "AlsaSwitch.h"
#include "OdroidAdvanceGo2Alsa.h"

class AlsaCard
{
  private:
    int mCardIdentifier;                //!< Card identifier/index in ALSA
    std::string mCardName;              //!< Displayable Card name
    std::vector<AlsaVolume> mVolumes;   //!< Available mixers for volume control
    std::vector<AlsaSwitch> mSwitches;  //!< Available mixers for switching output on/off
    std::vector<AlsaMixer>  mRawMixers; //!< Raw mixer objects
    std::vector<AlsaDevice> mDevices;   //!< Device list

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
    void AddVolumeControl(const AlsaVolume& mixer) { mVolumes.push_back(mixer); }
    //! Add a new mixer to the mixer list attached to the current card
    void AddSwitch(const AlsaSwitch& mixer) { mSwitches.push_back(mixer); }
    //! Add a new mixer to the mixer list attached to the current card
    void AddOdroidAdvanceGo2Router(const OdroidAdvanceGo2Alsa& mixer) { mRawMixers.push_back(mixer); }

    //! Add a new device to the device list attached to the current card
    void AddDevice(const AlsaDevice& device) { mDevices.push_back(device); }

    //! Get card index
    int Identifier() const { return mCardIdentifier; }
    //! Get card name
    const std::string& Name() const { return mCardName; }

    //! Get mixer count
    int VolumeCount() const { return mVolumes.size(); }
    //! Get mixer reference at position index (from 0 to MixerCount()-1)
    const AlsaVolume& VolumeAt(int index) const { return mVolumes[index]; }

    //! Get switch count
    int SwitchesCount() const { return mSwitches.size(); }
    //! Get switch reference at position index (from 0 to SwitchCount()-1)
    const AlsaSwitch& SwitchAt(int index) const { return mSwitches[index]; }

    //! Get Odroid Router
    OdroidAdvanceGo2Alsa* OdroidAdvanceGo2Router() const;

    //! Get device count
    int DeviceCount() const { return mDevices.size(); }
    //! Get device reference at position index (from 0 to DeviceCount()-1)
    const AlsaDevice& DeviceAt(int index) const { return mDevices[index]; }

    /*!
     * @brief Set volume to all underlaying volume mixers
     * @param volume volume to set from 0 to 100 (clamped if OOB)
     */
    void SetVolume(int volume);

    /*!
     * @brief Switch on all underlaying switch mixers
     */
    void SwitchOn();
};
