//
// Created by bkg2k on 14/05/19.
//
#pragma once

#include <string>
#include <utils/os/system/Thread.h>
#include <utils/os/system/Mutex.h>
#include <SDL_system.h>
#include <resources/TextureData.h>

extern "C"
{
#include <libavfilter/avfilter.h>        // libavfilter-dev
#include <libavformat/avformat.h>        // libavformat-dev
#include <libavcodec/avcodec.h>          // libavformat-dev
#include <libavdevice/avdevice.h>        // libavdevice-dev
#include <libavutil/imgutils.h>          // libavutil-dev
#include <libswresample/swresample.h>    // libavformat-dev
#include <libswscale/swscale.h>          // libavfilter-dev
}

class VideoEngine : Thread
{
  public:
    /*!
     * @brief Curren tplayer state
     */
    enum class PlayerState
    {
      Idle,         // Do nothing, sleep
      StartPending, // Initializing the video
      Playing,      // Currently playing
      Paused,       // Pause flag
      StopPending,  // Currently playing but must stop asap and return idle
    };

  private:
    /*!
     * @brief Small packet queue
     */
    class AudioPacketQueue
    {
      private:
        Mutex mMutex;

        //! First paquet
        AVPacketList* First;
        //! Last paquet
        AVPacketList* Last;
        //! Packet count
        int Count;
        //! Packet size
        int Size;

      public:
        /*!
         * @brief Constructor
         */
        AudioPacketQueue()
        {
          First = Last = nullptr;
          Count = Size = 0;
        }

        void Reset()
        {
          AVPacket packet;
          while(Dequeue(packet));
          First = Last = nullptr;
          Count = Size = 0;
        }

        /*!
         * @brief Enqueue a new packet
         * @param packet packet to enqueue
         */
        void Enqueue(const AVPacket* packet);

        /*!
         * @brief Dequeue a packet or return nullptr of the queue is empty.
         * This call is non blocking
         * @param packet packet to dequeue
         * @return True if the packet is valid
         */
        bool Dequeue(AVPacket& pkt);
    };

    /*!
     * @brief Internal context, all embedded in a POD object
     */
    class PlayerContext
    {
      public:
        //! Global audio/video context
        AVFormatContext* AudioVideoContext;

        //! Index of the first audio stream available in the file
        int AudioStreamIndex;
        //! Index of the first video stream available in the file
        int VideoStreamIndex;

        //! Audio codec
        AVCodec* AudioCodec;
        //! Video codec
        AVCodec* VideoCodec;
        //! Audio codec context
        AVCodecContext* AudioCodecContext;
        //! Video codec context
        AVCodecContext* VideoCodecContext;

        //! Rescaler context
        SwrContext* ResamplerContext;
        //! Color Space context
        SwsContext* ColorsSpaceContext;

        //! Audio packets
        AudioPacketQueue AudioQueue;

        //! Frame buffer
        unsigned char* FrameBuffer;

        //! Source video Frame in native pixel format
        AVFrame* Frame;
        //! Video Frame transcode to RGB
        AVFrame* FrameRGB[2];
        //! RGB frame access locker
        Mutex FrammeRGBLocker[2];
        //! Buffer in use by the decoder (0/1)
        unsigned int FrameInUse;

        //! Video width in pixel
        int Width;
        //!Video height in pixel
        int Height;

        //! Frame time in Nano-seconds
        long long FrameTime;
        //! Video duration in milliseconds
        int TotalTime;

        PlayerContext()
          : AudioVideoContext(nullptr),
            AudioStreamIndex(-1),
            VideoStreamIndex(-1),
            AudioCodec(nullptr),
            VideoCodec(nullptr),
            AudioCodecContext(nullptr),
            VideoCodecContext(nullptr),
            ResamplerContext(nullptr),
            ColorsSpaceContext(nullptr),
            FrameBuffer(nullptr),
            Frame(nullptr),
            FrameRGB(),
            FrameInUse(0),
            Width(0),
            Height(0),
            FrameTime(0),
            TotalTime(0)
        {
        }

        void Dispose()
        {
          if (AudioVideoContext ) avformat_close_input(&AudioVideoContext);
          if (AudioCodecContext ) avcodec_close(AudioCodecContext);
          if (VideoCodecContext ) avcodec_close(VideoCodecContext);
          if (ResamplerContext  ) swr_free(&ResamplerContext);
          if (ColorsSpaceContext) sws_freeContext(ColorsSpaceContext);
          if (Frame             ) av_frame_free(&Frame);
          if (FrameRGB[0]       ) av_frame_free(&FrameRGB[0]);
          if (FrameRGB[1]       ) av_frame_free(&FrameRGB[1]);
          if (FrameBuffer       ) av_free(FrameBuffer);

          AudioVideoContext = nullptr;
          AudioCodec = VideoCodec = nullptr;
          AudioCodecContext = VideoCodecContext = nullptr;
          ResamplerContext = nullptr;
          ColorsSpaceContext = nullptr;
          AudioStreamIndex = VideoStreamIndex = -1;
          FrameBuffer = nullptr;
          Frame = FrameRGB[0] = FrameRGB[1] = nullptr;
          Width = Height = 0;
          FrameInUse = 0;
          FrameTime = 0;

          AudioQueue.Reset();
        }
    };

  private:
    //! Signal used to unlock the thread and actually run the video decoding
    Mutex mSignal;

    //! Video filename
    std::string mFileName;

    //! Video playing state
    volatile PlayerState mState;

    //! Global context
    PlayerContext mContext;

    //! Textures to render
    TextureData mTexture;

    static constexpr int SDL_AUDIO_BUFFER_SIZE = 4096;

  private:

    /*!
     * Thread main loop
     */
    void Run() override;

    /*!
     * @brief Unlock the thread on exit
     */
    void Break() override
    {
      mSignal.Signal();
    }

    static void AudioCallback(void *userdata, unsigned char* stream, int len)
    {
      ((VideoEngine*)userdata)->DecodeAudioFrameOnDemand(stream, len);
    }

    bool InitializeDecoder();

    void DecodeAudioFrameOnDemand(unsigned char * stream, int len);

    int DecodeAudioFrame(AVCodecContext& audioContext, unsigned char* buffer, int size);

    void DecodeFrames();

    void FinalizeDecoder();

    /*!
     * @brief Default constructor
     */
    VideoEngine();

  public:
    /*!
     * @brief Destructor
     */
    ~VideoEngine() override { Thread::Stop(); }

    /*!
     * @brief Start playing a video file
     * If a video is already playing, a call to stop is performed playing the new video
     * @param videopath Path to the video file ot play
     */
    void PlayVideo(const std::string& videopath);

    /*!
     * @brief Stop the currently playing video file.
     * Does nothing if no file is actually playing
     */
    void StopVideo(bool waitforstop = false);

    /*!
     * @brief Get current image of the current playing video
     * This must be called fr
     * @return TextureData containing last image
     */
    TextureData& GetDisplayableFrame();

    /*!
     * @brief Get Instance
     * @return Instance
     */
    static VideoEngine& This();

    /*!
     * @brief Return true if the player is actually playing a video
     * @return True if a video is playing, false otherwise
     */
    bool IsPlaying() { return ((mState == PlayerState::Playing) || (mState == PlayerState::StopPending)); }

    /*!
     * @brief Return true if the engine is in idle state
     * @return True if in idle state, false otherwise
     */
    bool IsIdle() { return (mState == PlayerState::Idle); }

    int GetVideoDurationMs() { return IsPlaying() ? mContext.TotalTime : 0; }

    /*!
     * @brief Start the video engine. After calling this method, the player is ready to play video
     */
    void StartEngine() { Thread::Start("VideoEngine"); }

    /*!
     * Pause the engine if it's actually playing a video. Otherwise do nothing.
     */
    void PauseEngine() { if (mState == PlayerState::Playing) mState = PlayerState::Paused; }

    /*!
     * Resume the engine if it's actually paused. Otherwise do nothing.
     */
    void ResumeEngine() { if (mState == PlayerState::Paused) mState = PlayerState::Playing; }
};
