//
// Created by bkg2k on 19/12/2020.
//
#pragma once

#include <audio/IAudioController.h>
#include <pulse/pulseaudio.h>
#include <vector>
#include <utils/os/system/Thread.h>
#include <utils/os/system/Mutex.h>

class PulseAudioController: public IAudioController, private Thread
{
  public:
    /*!
     * @brief Default constructor
     */
    PulseAudioController();

    /*!
     * @brief Destructor
     */
    ~PulseAudioController() override;

    /*
     * Audio Controller implementation
     */

    /*!
     * @brief Return device list using identifier/displayable name
     * @return Device list
     */
    HashMap<int, std::string> GetPlaybackList() override;

    /*!
     * @brief Set default playback device by identifier
     * @param identifier device identifier
     */
    void SetDefaultPlayback(int identifier) override;

    /*!
     * @brief Set default playback device by name
     * @param playbackName device name
     * @return Actual selected device name
     */
    std::string SetDefaultPlayback(const std::string& playbackName) override;

    /*!
     * @brief Get volume level, from 0 to 100
     * @return Volume level
     */
    int GetVolume() override;

    /*!
     * @brief Set global volume from 0 to 100
     * @param volume Volume to set
     */
    void SetVolume(int volume) override;

  private:
    //! Device structure
    struct PulseAudioDevice
    {
      std::string Name;        //!< Device name
      std::string Description; //!< Description
      int Index;               //!< Device index in pulseaudio context
    };

    //! Pulseaudio connection state
    enum class ConnectionState
    {
      NotConnected, //!< Not yet connected
      Ready,        //!< Ready!
      Closed,       //!< Connection close
    };

    //! Source enumeration state state
    enum class EnumerationState
    {
        Starting,    //!< Just start!
        Enumerating, //!< Enumerating devices
        Complete,    //!< Enumeration complete
    };

    //! Device list (output only)
    std::vector<PulseAudioDevice> mDeviceList;
    //! Syncer
    Mutex mSyncer;

    //! Connection state
    ConnectionState mConnectionState;

    //! PulseAudio Context
    pa_context* mPulseAudioContext;
    //! PulseAudio Mainloop handle
    pa_mainloop* mPulseAudioMainLoop;
    //! Signal
    Mutex mSignal;

    /*!
     * @brief Initialize all
     */
    void Initialize();

    /*!
     * @brief Finalize all
     */
    void Finalize();

    /*
     * Pulse Audio callback
     */

    /*!
     * @brief Pulseaudio state callback
     * @param context Pulseaudio context
     * @param userdata This
     */
    static void ContextStateCallback(pa_context *context, void *userdata);

    /*!
     * @brief Callback used to retrieve Sinks
     * @param context Pulseaudio context
     * @param info Sink information structure
     * @param eol End-of-list flag
     * @param userdata This
     */
    static void EnumerateSinkCallback(pa_context *context, const pa_sink_info *info, int eol, void *userdata);

    /*!
     * @brief Callback called when volume is set
     * @param context Pulseaudio context
     * @param success Success flag
     * @param userdata This
     */
    static void SetVolumeCallback(pa_context *context, int success, void *userdata);

    /*!
     * @brief Callback called when volume is set
     * @param context Pulseaudio context
     * @param success Success flag
     * @param userdata This
     */
    static void SetSourceCallback(pa_context *context, int success, void *userdata);

    /*!
     * @brief Adjust device names
     */
    void AdjustDevices();

    /*
     * Thread implementation
     */

    //! Break blocking method in thread loop
    void Break() override;

    //! Thread loop
    void Run() override;

    /*
     * PulseAudio api
     */

    void PulseContextConnect();

    void PulseContextDisconnect();

    void PulseEnumerateDevices();
};
