//
// Created by bkg2k on 13/08/2020.
//
#pragma once

#include <utils/cplusplus/StaticLifeCycleControler.h>
#include <audio/IAudioController.h>
#include "AlsaCard.h"

class AlsaController: public IAudioController//, public StaticLifeCycleControler<AlsaController>
{
  private:
    std::vector<AlsaCard> mPlaybacks;

    /*!
     * @brief Initialize ALSA and fetch all cards/devices
     */
    void Initialize();

    /*!
     * @brief Lookup card and device from an opaque identifier
     * @param identifier Opaque identifier
     * @param cardIndex Output card index if found
     * @param deviceIndex Output device index if found
     * @return True if the card & device have been found, false otherwise
     */
    bool LookupCardDevice(int identifier, int& cardIndex, int& deviceIndex);

  public:
    //! Default output name
    static constexpr const char* sDefaultOutput = "Default output";

    /*!
     * @brief Default constructor
     */
    AlsaController()
    //  : StaticLifeCycleControler("AlsaController")
    {
      Initialize();
    }

    /*!
     * @brief Destructor
     */
    ~AlsaController() override = default;

    /*
     * IAudioController implementation
     */

    /*!
     * @brief Get playback list
     * @return Map opaque identifier : playback name
     */
    HashMap<std::string, std::string> GetPlaybackList() final;

    /*!
     * @brief Set the default card/device
     * @param playbackName playback name from GetPlaybackList()
     * @return playbackName or default value if playbackName is invalid
     */
    std::string SetDefaultPlayback(const std::string& playbackName) final;

    /*!
     * @brief Get volume from the given playback
     * @return Volume percent
     */
    int GetVolume() final;

    /*!
     * @brief Set volume to the given playback
     * @param volume Volume percent
     */
    void SetVolume(int volume) final;
};
