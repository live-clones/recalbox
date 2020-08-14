//
// Created by bkg2k on 13/08/2020.
//
#pragma once

#include <utils/storage/HashMap.h>
#include <string>

class IAudioController
{
  public:
    virtual ~IAudioController() = default;

    /*!
     * @brief Get playback list
     * @return Map identifier : playback name
     */
    virtual HashMap<int, std::string> GetPlaybackList() const = 0;

    /*!
     * @brief Set the default card/device
     * @param identifier opaque identifier from GetPlaybackList()
     */
    virtual void SetDefaultPlayback(int identifier) = 0;

    /*!
     * @brief Set the default card/device
     * @param playbackName playback name from GetPlaybackList()
     * @return playbackName or default value if playbackName is invalid
     */
    virtual std::string SetDefaultPlayback(const std::string& playbackName) = 0;

    /*!
     * @brief Get volume from the given playback
     * @return Volume percent
     */
    virtual int GetVolume() = 0;

    /*!
     * @brief Set volume to the given playback
     * @param volume Volume percent
     */
    virtual void SetVolume(int volume) = 0;
};
