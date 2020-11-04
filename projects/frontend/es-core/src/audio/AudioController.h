//
// Created by bkg2k on 13/08/2020.
//
#pragma once

#include <audio/IAudioController.h>
#include <audio/alsa/AlsaController.h>
#include <utils/cplusplus/StaticLifeCycleControler.h>

class AudioController: public StaticLifeCycleControler<AudioController>
{
  private:
    static IAudioController& GetAudioController()
    {
      // Statically stuck to ALSA for now
      static AlsaController sAlsaController;
      return sAlsaController;
    }

    // Underlaying audio controler
    IAudioController& mController;

    // Has special ausio settings?
    bool mHasSpecialAudio;

    /*!
     * @brief Check if the current machine has a special audio configuration
     * @return True if the current machine has a special audio configuration
     */
    static bool GetSpecialAudio();

  public:
    AudioController()
      : StaticLifeCycleControler("AudioController")
      , mController(GetAudioController())
      , mHasSpecialAudio(GetSpecialAudio())
    {
    }

    /*!
     * @brief Get playback list
     * @return Map identifier : playback name
     */
    HashMap<int, std::string> GetPlaybackList() const { return mController.GetPlaybackList(); }

    /*!
     * @brief Set the default card/device
     * @param identifier opaque identifier from GetPlaybackList()
     */
    void SetDefaultPlayback(int identifier) { if (!mHasSpecialAudio) mController.SetDefaultPlayback(identifier); }

    /*!
     * @brief Set the default card/device
     * @param playbackName playback name from GetPlaybackList()
     * @return playbackName or default value if playbackName is invalid
     */
    std::string SetDefaultPlayback(const std::string& playbackName);

    /*!
     * @brief Get volume from the given playback
     * @return Volume percent
     */
    int GetVolume() const { return mController.GetVolume(); }

    /*!
     * @brief Set volume to the given playback
     * @param volume Volume percent
     */
    void SetVolume(int volume) { if (!mHasSpecialAudio) mController.SetVolume(volume); }

    /*!
     * @brief Check if there is a special audio configuration
     * @return True if there is a special audio configuration
     */
    bool HasSpecialAudio() const { return mHasSpecialAudio; }
};
