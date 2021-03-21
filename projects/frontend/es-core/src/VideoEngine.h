//
// Created by bkg2k on 14/05/19.
//
#pragma once

#include <utils/os/system/Thread.h>
#include <utils/os/system/Mutex.h>
#include <utils/os/system/Signal.h>
#include <utils/cplusplus/StaticLifeCycleControler.h>
#include <utils/storage/MessageFactory.h>
#include <SDL_system.h>
#include <resources/TextureData.h>
#include <utils/storage/Queue.h>

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

class VideoEngine : public StaticLifeCycleControler<VideoEngine>, private Thread
{
  public:
    /*!
     * @brief Current player state
     */
    enum class PlayerState
    {
      Idle,         // Do nothing, sleep
      StartPending, // Initializing the video
      Playing,      // Currently playing
      Paused,       // Pause flag
      StopPending,  // Currently playing but must stop asap and return idle
      Error,        // video is in error
    };

    /*!
     * @brief Simple order sent to th eplayer
     */
    enum class Order
    {
      None,  //!< Emptuy order
      Play,  //!< Play a video
      Stop,  //!< Stop video
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
          : First(nullptr)
          , Last(nullptr)
          , Count(0)
          , Size(0)
        {
        }

        void Reset()
        {
          AVPacket packet;
          while(Dequeue(packet)) {
              av_packet_unref(&packet);
          }
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
          // Protect both access to RGB data
          FrammeRGBLocker[0].Lock();
          FrammeRGBLocker[1].Lock();

          if (AudioVideoContext != nullptr ) avformat_close_input(&AudioVideoContext);
          if (AudioCodecContext != nullptr ) avcodec_free_context(&AudioCodecContext);
          if (VideoCodecContext != nullptr ) avcodec_free_context(&VideoCodecContext);
          if (ResamplerContext != nullptr  ) swr_free(&ResamplerContext);
          if (ColorsSpaceContext != nullptr) sws_freeContext(ColorsSpaceContext);
          if (Frame != nullptr             ) av_frame_free(&Frame);
          if (FrameRGB[0] != nullptr       ) av_frame_free(&FrameRGB[0]);
          if (FrameRGB[1] != nullptr       ) av_frame_free(&FrameRGB[1]);
          if (FrameBuffer != nullptr       ) av_free(FrameBuffer);

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

          // Unlock RGB data
          FrammeRGBLocker[0].UnLock();
          FrammeRGBLocker[1].UnLock();
        }
    };

    /*!
     * @brief Order message
     */
    class OrderMessage
    {
      public:
        //! Default constructor
        OrderMessage() : mOrder(Order::Stop) {}
        //! Copy constructor
        OrderMessage(const OrderMessage& source) : mOrder(source.GetOrder()), mVideoPath(source.mVideoPath), mDecodeAudio(source.mDecodeAudio) {}
        //! Copy operator
        OrderMessage& operator = (const OrderMessage& source)
        {
          if (&source != this)
          {
            mOrder = source.GetOrder();
            mVideoPath = source.mVideoPath;
            mDecodeAudio = source.mDecodeAudio;
          }
          return *this;
        }
        //! Set properties
        void Set(Order order, const Path& videoPath, bool decodeAudio) { mOrder = order; mVideoPath = videoPath; mDecodeAudio = decodeAudio; }
        //! Set properties
        void Set(Order order) { mOrder = order; }

        Order GetOrder() const { return mOrder; }
        const Path& GetPath() const { return mVideoPath; }
        bool GetDecodeAudio() const { return mDecodeAudio; }

      private:
        Order mOrder;
        Path mVideoPath;
        bool mDecodeAudio;
    };

    //! Signal used to unlock the thread and actually run the video decoding
    Signal mSignal;
    //! Signal used to instruct other thread a video has been stoped
    Signal mWaitForStop;
    //! Video filename
    Path mFileName;

    //! Is playing?
    volatile bool mIsPlaying;

    //! Global context
    PlayerContext mContext;
    //! Textures to render
    TextureData mTexture;
    //! Texture protector
    Mutex mTextureSyncer;

    //! Order message provider
    MessageFactory<OrderMessage> mMessageProvider;
    //! Order Queue
    Queue<OrderMessage*> mPendingMessages;
    //! Queue protector
    Mutex mQueueSyncer;

    static constexpr int SDL_AUDIO_BUFFER_SIZE = 4096;

    /*!
     * Thread main loop
     */
    void Run() override;

    /*!
     * @brief Unlock the thread on exit
     */
    void Break() override
    {
      mSignal.Fire();
    }

    static void AudioCallback(void *userdata, unsigned char* stream, int len)
    {
      ((VideoEngine*)userdata)->DecodeAudioFrameOnDemand(stream, len);
    }

    bool InitializeDecoder();

    void DecodeAudioFrameOnDemand(unsigned char * stream, int len);

    int DecodeAudioFrame(AVCodecContext& audioContext, unsigned char* buffer, int size);

    void DecodeFrames(bool decodeAudio = false);

    void FinalizeDecoder();

    /*!
     * @brief Check if there is at least one pending message in the queue
     * @return True if at least one pending message is in the queue, fals otherwise
     */
    bool CheckPendingMessages();

    /*!
     * @brief Pop next message out of the order queue.
     * @param output Ouput message to fill in with the first message availablein the queue
     * @return True if the output contains a valid message, false otherwise
     */
    bool GetNextOrder(OrderMessage& output);

    /*!
     * @brief Start the video engine. After calling this method, the player is ready to play video
     */
    void StartEngine() { Thread::Start("VideoEngine"); }

  public:
    /*!
     * @brief Default constructor
     */
    VideoEngine();

    /*!
     * @brief Destructor
     */
    ~VideoEngine() override
    {
      StopVideo(true);
      Thread::Stop();
    }

    /*!
     * @brief Start playing a video file
     * If a video is already playing, a call to stop is performed playing the new video
     * @param videopath Path to the video file ot play
     */
    void PlayVideo(const Path& videopath, bool decodeAudio = false);

    /*!
     * @brief Stop the currently playing video file.
     * Does nothing if no file is actually playing
     * @param waitforstop wait for the playing video to stop
     */
    void StopVideo(bool waitforstop);

    /*!
     * @brief Get current image of the current playing video
     * This must be called fr
     * @return TextureData containing last image
     */
    TextureData& GetDisplayableFrame();

    /*!
     * @brief Return true if the player is actually playing a video
     * @return True if a video is playing, false otherwise
     */
    bool IsPlaying() const { return mIsPlaying; }

    /*!
     * @brief Return true if the engine is in idle state
     * @return True if in idle state, false otherwise
     */
    int GetVideoDurationMs() const { return IsPlaying() ? mContext.TotalTime : 0; }

    //! Lock texture for thread safety
    void AquireTexture() { mTextureSyncer.Lock(); }

    //! Release texture
    void ReleaseTexture() { mTextureSyncer.UnLock(); }
};
