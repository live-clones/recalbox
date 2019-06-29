//
// Created by bkg2k on 14/05/19.
//

#include <SDL_audio.h>
#include <util/HighResolutionTimer.h>
#include "VideoEngine.h"
#include "Log.h"

#define PIXEL_FORMAT AV_PIX_FMT_RGBA

#define RETURN_ERROR(x, y) do{ LOG(LogError) << x; return y; }while(false)

static int NanoSleep(long long nanoseconds)
{
  static timespec remaining;
  if (nanoseconds < 1000000000LL)
  {
    if (nanoseconds < 0) return 0;
    timespec delay = { 0, nanoseconds };
    nanosleep(&delay, &remaining);
  }
  else
  {
    timespec delay = { nanoseconds / 1000000000LL, nanoseconds % 1000000000LL};
    nanosleep(&delay, &remaining);
  }
  // Return factionnal or remaining time
  return remaining.tv_nsec;
}

static char* _FourCCToString(unsigned int fourcc)
{
  static char FCC[5] = { 0, 0, 0, 0, 0 };
  FCC[0] = (char)fourcc;
  FCC[1] = (char)(fourcc >> 8U);
  FCC[2] = (char)(fourcc >> 16U);
  FCC[3] = (char)(fourcc >> 24U);
  FCC[4] = 0;
  return FCC;
}

VideoEngine& VideoEngine::This()
{
  static VideoEngine instance;
  return instance;
}

void VideoEngine::AudioPacketQueue::Enqueue(const AVPacket* packet)
{
  AVPacketList* elt = nullptr;
  AVPacket pkt;
  if (av_packet_ref(&pkt, packet)) return;
  elt = (AVPacketList*)av_malloc(sizeof(AVPacketList));
  if (!elt) return;
  elt->pkt = pkt;
  elt->next = nullptr;

  mMutex.Lock();

  if (!Last) First = elt;
  else Last->next = elt;
  Last = elt;
  Count++;
  Size += elt->pkt.size;

  mMutex.UnLock();
}

bool VideoEngine::AudioPacketQueue::Dequeue(AVPacket& pkt)
{
  bool result = false;

  mMutex.Lock();
  AVPacketList* elt = First;
  if (elt)
  {
    First = elt->next;
    if (!First) Last = nullptr;
    Count--;
    Size -= elt->pkt.size;
    pkt = elt->pkt;
    av_free(elt);
    result = true;
  }
  mMutex.UnLock();

  return result;
}

VideoEngine::VideoEngine()
  : mState(PlayerState::Idle)
{
}

void VideoEngine::Run()
{
  while(IsRunning())
  {
    // Wait for a video to play
    mState = PlayerState::Idle;
    mSignal.WaitSignal();
    if (!IsRunning())
    {
      LOG(LogInfo) << "Video Engine stopped.";
      break;
    }

    // Run the video
    LOG(LogDebug) << "Video Engine start playing " << mFileName;
    mState = PlayerState::StartPending;
    if (InitializeDecoder())
    {
      if (mState == PlayerState::StartPending)
        mState = PlayerState::Playing;
      while (mState == PlayerState::Playing)
      {
        DecodeFrames();
      }
    }
    FinalizeDecoder();
  }
}

void VideoEngine::PlayVideo(const std::string& videopath)
{
  LOG(LogDebug) << "Video Engine requested to play " << videopath;

  // Stop previous video
  StopVideo(true);

  // Start the new video
  if (!videopath.empty())
  {
    mFileName = videopath;
    mSignal.Signal();
  }
}

void VideoEngine::StopVideo(bool waitforstop)
{
  switch(mState)
  {
    case PlayerState::Idle: break;
    case PlayerState::StartPending:
    case PlayerState::Paused:
    case PlayerState::Playing:
    {
      LOG(LogDebug) << "Video Engine requested to stop playing " << mFileName;
      mState = PlayerState::StopPending;
      break;
    }
    case PlayerState::StopPending: break;
  }
  // Paused?

  // Wait for the video to stop
  if (waitforstop)
  {
    // So we can afford a little sleep loop. No need to use a signal here.
    while (mState == PlayerState::StopPending)
      sleep(10);
  }

  mFileName = "";
}

bool VideoEngine::InitializeDecoder()
{
  // Initialize FFMpeg engine
  static bool FFMpegInitialized = false;
  if (!FFMpegInitialized)
  {
    av_register_all();
    avcodec_register_all();
    avdevice_register_all();
    avformat_network_init();

    FFMpegInitialized = true;
    LOG(LogInfo) << "FFMpeg global context initialized.";
  }

  // Open the file
  if (avformat_open_input(&mContext.AudioVideoContext, mFileName.c_str(), nullptr, nullptr))
    RETURN_ERROR("Error opening video " << mFileName, false);

  // Lookup stream
  if (avformat_find_stream_info(mContext.AudioVideoContext, nullptr))
    RETURN_ERROR("Error finding streams in " << mFileName, false);

  // Lookup audio and vdeo stream indexes
  mContext.AudioStreamIndex = mContext.VideoStreamIndex = -1;
  for (int s = 0; s < (int)mContext.AudioVideoContext->nb_streams; ++s)
  {
    //av_dump_format(mContext.AudioVideoContext, s, mFileName.c_str(), 0);
    switch(mContext.AudioVideoContext->streams[s]->codecpar->codec_type)
    {
      case AVMEDIA_TYPE_AUDIO: if (mContext.AudioStreamIndex < 0) mContext.AudioStreamIndex = s; break;
      case AVMEDIA_TYPE_VIDEO: if (mContext.VideoStreamIndex < 0) mContext.VideoStreamIndex = s; break;
      case AVMEDIA_TYPE_UNKNOWN:
      case AVMEDIA_TYPE_DATA:
      case AVMEDIA_TYPE_ATTACHMENT:
      case AVMEDIA_TYPE_SUBTITLE:
      case AVMEDIA_TYPE_NB:
      default: break;
    }
  }
  if (mContext.VideoStreamIndex < 0)
    RETURN_ERROR("Error finding video stream in " << mFileName, false);

  // Initialize audio codec
  if (mContext.AudioStreamIndex >= 0)
  {
    mContext.AudioCodec = avcodec_find_decoder(mContext.AudioVideoContext->streams[mContext.AudioStreamIndex]->codecpar->codec_id);
    if (!mContext.AudioCodec) RETURN_ERROR("Error finding audio codec " << _FourCCToString(mContext.AudioVideoContext->streams[mContext.AudioStreamIndex]->codecpar->codec_tag), false);
    mContext.AudioCodecContext = avcodec_alloc_context3(mContext.AudioCodec);
    if (!mContext.AudioCodecContext) RETURN_ERROR("Error allocating audio codec context", false);
    if (avcodec_parameters_to_context(mContext.AudioCodecContext, mContext.AudioVideoContext->streams[mContext.AudioStreamIndex]->codecpar)) RETURN_ERROR("Error setting parameters to audio codec context", false);
    if (avcodec_open2(mContext.AudioCodecContext, mContext.AudioCodec, nullptr)) RETURN_ERROR("Error opening audio codec", false);
    mContext.ResamplerContext = swr_alloc();
    if (!mContext.ResamplerContext) RETURN_ERROR("Error allocating audio resampler context", false);
    av_opt_set_channel_layout(mContext.ResamplerContext, "in_channel_layout", mContext.AudioCodecContext->channel_layout, 0);
    av_opt_set_channel_layout(mContext.ResamplerContext, "out_channel_layout", mContext.AudioCodecContext->channel_layout, 0);
    av_opt_set_int(mContext.ResamplerContext, "in_sample_rate", mContext.AudioCodecContext->sample_rate, 0);
    av_opt_set_int(mContext.ResamplerContext, "out_sample_rate", mContext.AudioCodecContext->sample_rate, 0);
    av_opt_set_sample_fmt(mContext.ResamplerContext, "in_sample_fmt", mContext.AudioCodecContext->sample_fmt, 0);
    av_opt_set_sample_fmt(mContext.ResamplerContext, "out_sample_fmt", AV_SAMPLE_FMT_FLT, 0);
    if (swr_init(mContext.ResamplerContext)) RETURN_ERROR("Error initializing audio resampler context", false);
  }

  // Initialize video codec
  mContext.VideoCodec = avcodec_find_decoder(mContext.AudioVideoContext->streams[mContext.VideoStreamIndex]->codecpar->codec_id);
  if (!mContext.VideoCodec) RETURN_ERROR("Error finding video codec " << mContext.AudioVideoContext->streams[mContext.VideoStreamIndex]->codecpar->codec_id, false);
  mContext.VideoCodecContext = avcodec_alloc_context3(mContext.VideoCodec);
  if (!mContext.VideoCodecContext) RETURN_ERROR("Error allocating video codec context", false);
  if (avcodec_parameters_to_context(mContext.VideoCodecContext, mContext.AudioVideoContext->streams[mContext.VideoStreamIndex]->codecpar)) RETURN_ERROR("Error setting parameters to video codec context", false);
  if (avcodec_open2(mContext.VideoCodecContext, mContext.VideoCodec, nullptr)) RETURN_ERROR("Error opening video codec", false);
  mContext.ColorsSpaceContext = sws_getContext(mContext.VideoCodecContext->width,
                                               mContext.VideoCodecContext->height,
                                               mContext.VideoCodecContext->pix_fmt,
                                               mContext.VideoCodecContext->width,
                                               mContext.VideoCodecContext->height,
                                               PIXEL_FORMAT,
                                               SWS_BILINEAR,
                                               nullptr,
                                               nullptr,
                                               nullptr);
  if (!mContext.ColorsSpaceContext) RETURN_ERROR("Error initializing video color space context", false);

  // Time frame & duration
  AVStream* videoStream = mContext.AudioVideoContext->streams[mContext.VideoStreamIndex];
  mContext.FrameTime = (long long)(((double)videoStream->avg_frame_rate.den * 1000000000.0) / (double)videoStream->avg_frame_rate.num);
  mContext.TotalTime = (int)(((double)videoStream->avg_frame_rate.den * (double)videoStream->nb_frames * 1000.0) / (double)videoStream->avg_frame_rate.num);

  // Initialize video frame
  mContext.Frame = av_frame_alloc();
  mContext.FrameRGB[0] = av_frame_alloc();
  mContext.FrameRGB[1] = av_frame_alloc();
  if (!mContext.Frame || ! mContext.FrameRGB[0] || ! mContext.FrameRGB[1]) RETURN_ERROR("Error allocating video frames", false);

  mContext.Width = mContext.AudioVideoContext->streams[mContext.VideoStreamIndex]->codecpar->width;
  mContext.Height = mContext.AudioVideoContext->streams[mContext.VideoStreamIndex]->codecpar->height;
  int argbSize = av_image_get_buffer_size(PIXEL_FORMAT, mContext.Width, mContext.Height, 8);
  if (argbSize < 1) RETURN_ERROR("Error getting video frame size", false);
  auto FrameBuffer = (unsigned char*)av_malloc(argbSize);
  if (!FrameBuffer) RETURN_ERROR("Error allocating frame buffer", false);

  if (av_image_fill_arrays(&mContext.FrameRGB[0]->data[0], &mContext.FrameRGB[0]->linesize[0], FrameBuffer, PIXEL_FORMAT, mContext.Width, mContext.Height, 1) < 0)
    RETURN_ERROR("Error setting frame buffer", false);
  if (av_image_fill_arrays(&mContext.FrameRGB[1]->data[0], &mContext.FrameRGB[1]->linesize[0], FrameBuffer, PIXEL_FORMAT, mContext.Width, mContext.Height, 1) < 0)
    RETURN_ERROR("Error setting frame buffer", false);

  // Initialize audio callback
  if (mContext.AudioCodec)
  {
    SDL_AudioSpec Wanted;
    SDL_AudioSpec ActuallyGot;
    Wanted.channels = mContext.AudioCodecContext->channels;
    Wanted.freq = mContext.AudioCodecContext->sample_rate;
    Wanted.format = AUDIO_F32;
    Wanted.silence = 0;
    Wanted.samples = SDL_AUDIO_BUFFER_SIZE;
    Wanted.userdata = this;
    Wanted.callback = AudioCallback;

    SDL_OpenAudio(&Wanted, &ActuallyGot);
    SDL_PauseAudio(0);
  }

  return true;
}

int VideoEngine::DecodeAudioFrame(AVCodecContext& audioContext, unsigned char* buffer, int /*size*/)
{
  static AVPacket packet;
  static unsigned char* packetData = nullptr;
  static int packetSize = 0;
  static AVFrame* frame = av_frame_alloc();
  static unsigned char converted_data[(192000 * 3) / 2];
  static unsigned char* converted = &converted_data[0];

  int dataSize = 0;
  for (;;)
  {
    while (packetSize > 0)
    {
      int gotFrame = 0;
      int size = avcodec_decode_audio4(&audioContext, frame, &gotFrame, &packet);
      // if error, skip frame
      if (size < 0) { packetSize = 0; break; }

      packetData += size;
      packetSize -= size;
      if (gotFrame)
      {
        //dataSize = av_samples_get_buffer_size(nullptr, audioContext.channels, frame->nb_samples, audioContext.sample_fmt, 1);
        int outSize = av_samples_get_buffer_size(nullptr, audioContext.channels, frame->nb_samples, AV_SAMPLE_FMT_FLT, 1);
        /*int len2 = */swr_convert(mContext.ResamplerContext, &converted, frame->nb_samples, (const uint8_t**)&frame->data[0], frame->nb_samples);
        memcpy(buffer, converted_data, outSize);
        dataSize = outSize;
      }

      // No data yet, get more frames
      if (dataSize <= 0) continue;

      // We have data, return it and come back for more later
      return dataSize;
    }

    if (!mContext.AudioQueue.Dequeue(packet)) return -1;

    packetData = packet.data;
    packetSize = packet.size;
  }
}

void VideoEngine::DecodeAudioFrameOnDemand(uint8_t * stream, int len)
{
  static unsigned char AudioBuffer[(192000 * 3) / 2];
  static unsigned int AudioBufferSize = 0;
  static unsigned int AudioBufferIndex = 0;

  while (len > 0)
  {
    if (AudioBufferIndex >= AudioBufferSize)
    {
      // already sent all data; get more
      int audioSize = DecodeAudioFrame(*mContext.AudioCodecContext, AudioBuffer, sizeof(AudioBuffer));
      if (audioSize < 0)
      {
        // error = silence
        AudioBufferSize = SDL_AUDIO_BUFFER_SIZE;
        memset(AudioBuffer, 0, sizeof(AudioBuffer));
      }
      else AudioBufferSize = audioSize;
      AudioBufferIndex = 0;
    }
    int Remaining = AudioBufferSize - AudioBufferIndex;
    if (Remaining > len) Remaining = len;
    memcpy(stream, (unsigned char *)AudioBuffer + AudioBufferIndex, Remaining);
    len -= Remaining;
    stream += Remaining;
    AudioBufferIndex += Remaining;
  }
}

TextureData& VideoEngine::GetDisplayableFrame()
{
  int frame = (mContext.FrameInUse ^ 1U) & 1U;
  mContext.FrammeRGBLocker[frame].Lock();
  if (mContext.FrameRGB[frame])
    mTexture.updateFromRGBA(mContext.FrameRGB[frame]->data[0], mContext.Width, mContext.Height);
  mContext.FrammeRGBLocker[frame].UnLock();

  return mTexture;
}

void VideoEngine::DecodeFrames()
{
  int VideoFrameCount = 0;
  int AudioFrameCount = 0;
  AVPacket packet;
  HighResolutionTimer timer;
  timer.Initialize(0);
  for(;;)
  {
    int error = av_read_frame(mContext.AudioVideoContext, &packet);

    // Loop ?
    if (error == AVERROR_EOF)
    {
      AVStream* stream = mContext.AudioVideoContext->streams[mContext.VideoStreamIndex];
      avio_seek(mContext.AudioVideoContext->pb, 0, SEEK_SET);
      avformat_seek_file(mContext.AudioVideoContext, mContext.VideoStreamIndex, 0, 0, stream->duration, 0);
      continue;
    }

    if (error >= 0)
    {
      if (packet.stream_index == mContext.VideoStreamIndex)
      {
        if (avcodec_send_packet(mContext.VideoCodecContext, &packet))
          RETURN_ERROR("Error sending video packet to codec",);
        while (mState == PlayerState::Playing && !avcodec_receive_frame(mContext.VideoCodecContext, mContext.Frame))
        {
          // Timing
          long long TimeFromPreviousFrame = timer.GetNanoSeconds();
          long long StillToWait = mContext.FrameTime - TimeFromPreviousFrame;
          timer.Initialize(NanoSleep(StillToWait));

          // Get RGBA video frame
          mContext.FrammeRGBLocker[mContext.FrameInUse & 1U].Lock();
          sws_scale(mContext.ColorsSpaceContext,
                    mContext.Frame->data,
                    mContext.Frame->linesize,
                    0,
                    mContext.VideoCodecContext->height,
                    mContext.FrameRGB[mContext.FrameInUse & 1U]->data,
                    mContext.FrameRGB[mContext.FrameInUse & 1U]->linesize);
          mContext.FrammeRGBLocker[mContext.FrameInUse & 1U].UnLock();

          // Swap frame
          mContext.FrameInUse ^= 1U;

          ++VideoFrameCount;
        }
      }
      else if (packet.stream_index == mContext.AudioStreamIndex)
      {
        mContext.AudioQueue.Enqueue(&packet);
        ++AudioFrameCount;
      }
      av_packet_unref(&packet);
    }

    // Paused?
    while(mState == PlayerState::Paused)
      sleep(50);

    // End?
    if (mState != PlayerState::Playing) break;
  }
}

void VideoEngine::FinalizeDecoder()
{
  mContext.Dispose();
  mTexture.reset();
}
