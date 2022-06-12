//
// Created by bkg2k on 14/05/19.
//

#include <SDL_audio.h>
#include <utils/datetime/HighResolutionTimer.h>
#include "VideoEngine.h"

#define PIXEL_FORMAT AV_PIX_FMT_RGBA

#define RETURN_ERROR(x, y) do{ LOG(LogError) << "[Video Engine]" << x; return y; }while(false)

static unsigned char converted_data[(192000 * 3) / 2];
static unsigned char* converted = &converted_data[0];

static void ClearAudioBuffers()
{
  memset(converted_data, 0, sizeof(converted_data));
}


static int NanoSleep(long long nanoseconds)
{
  static timespec remaining;
  if (nanoseconds < 1000000000LL)
  {
    if (nanoseconds < 0) return 0;
    timespec delay = { 0, (int)nanoseconds };
    nanosleep(&delay, &remaining);
  }
  else
  {
    timespec delay = { (int)(nanoseconds / 1000000000LL), (int)(nanoseconds % 1000000000LL) };
    nanosleep(&delay, &remaining);
  }
  // Return fractionnal or remaining time
  return remaining.tv_nsec;
}

static char* FourCCToString(unsigned int fourcc)
{
  static char FCC[5] = { 0, 0, 0, 0, 0 };
  FCC[0] = (char)fourcc;
  FCC[1] = (char)(fourcc >> 8U);
  FCC[2] = (char)(fourcc >> 16U);
  FCC[3] = (char)(fourcc >> 24U);
  FCC[4] = 0;
  return FCC;
}

void VideoEngine::AudioPacketQueue::Enqueue(const AVPacket* packet)
{
  AVPacketList* elt = nullptr;
  AVPacket pkt;
  if (av_packet_ref(&pkt, packet) != 0) return;
  elt = (AVPacketList*)av_malloc(sizeof(AVPacketList));
  if (elt == nullptr) return;
  elt->pkt = pkt;
  elt->next = nullptr;

  mMutex.Lock();

  if (Last == nullptr) First = elt;
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
  if (elt != nullptr)
  {
    First = elt->next;
    if (First == nullptr) Last = nullptr;
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
  : StaticLifeCycleControler<VideoEngine>("VideoEngine")
  , mIsPlaying(false)
{
  StartEngine();
}

bool VideoEngine::CheckPendingMessages()
{
  Mutex::AutoLock sync(mQueueSyncer);
  return !mPendingMessages.Empty();
}

bool VideoEngine::GetNextOrder(VideoEngine::OrderMessage& output)
{
  while(IsRunning())
  {
    { // Check last message - Pop & recycle previous message if it's a pending play message
      Mutex::AutoLock sync(mQueueSyncer);
      if (!mPendingMessages.Empty())
      {
        output = *mPendingMessages.Peek();
        mMessageProvider.Recycle(mPendingMessages.Pop());
        return true;
      }
    }
    // No message yet, wait for the next
    mSignal.WaitSignal();
  }
  return false;
}

void VideoEngine::Run()
{
  try
  {
    OrderMessage nextMessage;
    while(IsRunning())
      if (GetNextOrder(nextMessage)) // Blocking operation
        switch(nextMessage.GetOrder())
        {
          case Order::Play:
          {
            // Video changed?
            if (nextMessage.GetPath() == mFileName) FinalizeDecoder();
            // Entre play loop
            mFileName = nextMessage.GetPath();
            if (InitializeDecoder())
            {
              mIsPlaying = true;
              // Play until next message
              { LOG(LogDebug) << "[Video Engine] is playing " << mFileName.ToString(); }
              DecodeFrames(nextMessage.GetDecodeAudio());
            }
            else { LOG(LogDebug) << "[Video Engine] got an error playing " << mFileName.ToString(); }
            break;
          }
          case Order::Stop:
          {
            mIsPlaying = false;
            mFileName = Path();
            FinalizeDecoder();
            mWaitForStop.Fire();
            break;
          }
          case Order::None:
          default: break;
        }
  }
  catch(const std::exception& e)
  {
    { LOG(LogError) << "[Video Engine] fatal exception: " << e.what() << ". Engine stopped until next ES launch!"; }
  }
  FinalizeDecoder();
}

void VideoEngine::PlayVideo(const Path& videopath, bool decodeAudio)
{
  { LOG(LogDebug) << "[Video Engine] Request to play " << videopath.ToString(); }

  { // Check last message - Pop & recycle previous message if it's a pending play message
    Mutex::AutoLock sync(mQueueSyncer);
    if (!mPendingMessages.Empty())
      if (mPendingMessages.Peek()->GetOrder() == Order::Play)
        mMessageProvider.Recycle(mPendingMessages.Pop());
  }

  // Create new message
  OrderMessage* message = mMessageProvider.Obtain();
  message->Set(Order::Play, videopath, decodeAudio);
  { // Post
    Mutex::AutoLock sync(mQueueSyncer);
    mPendingMessages.Push(message);
    mSignal.Fire();
  }
}

void VideoEngine::StopVideo(bool waitforstop)
{
  { LOG(LogDebug) << "[Video Engine] Request to stop playing " << mFileName.ToString(); }

  { // Check last message - Pop & recycle previous message if it's a pending play message
    Mutex::AutoLock sync(mQueueSyncer);
    if (!mPendingMessages.Empty())
      if (mPendingMessages.Peek()->GetOrder() == Order::Stop)
        mMessageProvider.Recycle(mPendingMessages.Pop());
  }

  // Create new message
  OrderMessage* message = mMessageProvider.Obtain();
  message->Set(Order::Stop);
  { // Post
    Mutex::AutoLock sync(mQueueSyncer);
    mPendingMessages.Push(message);
    mSignal.Fire();
  }
  if(mTexture.isLoaded())
  {
    AquireTexture();
    mTexture.reset();
    ReleaseTexture();
  }

  // Wait?
  if (mIsPlaying && waitforstop)
    mWaitForStop.WaitSignal(2000); // Not longer than 2s or something is going really wrong
  // Force a signal reset so that a non waiting stop does not unlock the next waiting stop
  mWaitForStop.Reset();
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
    { LOG(LogInfo) << "[Video Engine] FFMpeg global context initialized."; }
  }
  // Reset audio data
  ClearAudioBuffers();

  // Open the file
  if (avformat_open_input(&mContext.AudioVideoContext, mFileName.ToChars(), nullptr, nullptr) != 0)
    RETURN_ERROR("Error opening video " << mFileName.ToString(), false);

  // Lookup stream
  if (avformat_find_stream_info(mContext.AudioVideoContext, nullptr) != 0)
    RETURN_ERROR("Error finding streams in " << mFileName.ToString(), false);

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
    RETURN_ERROR("Error finding video stream in " << mFileName.ToString(), false);

  // Initialize audio codec
  if (mContext.AudioStreamIndex >= 0)
  {
    mContext.AudioCodec = avcodec_find_decoder(mContext.AudioVideoContext->streams[mContext.AudioStreamIndex]->codecpar->codec_id);
    if (mContext.AudioCodec == nullptr) RETURN_ERROR("Error finding audio codec " << FourCCToString(mContext.AudioVideoContext->streams[mContext.AudioStreamIndex]->codecpar->codec_tag), false);
    mContext.AudioCodecContext = avcodec_alloc_context3(mContext.AudioCodec);
    if (mContext.AudioCodecContext == nullptr) RETURN_ERROR("Error allocating audio codec context", false);
    if (avcodec_parameters_to_context(mContext.AudioCodecContext, mContext.AudioVideoContext->streams[mContext.AudioStreamIndex]->codecpar) != 0) RETURN_ERROR("Error setting parameters to audio codec context", false);
    if (avcodec_open2(mContext.AudioCodecContext, mContext.AudioCodec, nullptr) != 0) RETURN_ERROR("Error opening audio codec", false);
    mContext.ResamplerContext = swr_alloc();
    if (mContext.ResamplerContext == nullptr) RETURN_ERROR("Error allocating audio resampler context", false);
    av_opt_set_channel_layout(mContext.ResamplerContext, "in_channel_layout", mContext.AudioCodecContext->channel_layout, 0);
    av_opt_set_channel_layout(mContext.ResamplerContext, "out_channel_layout", mContext.AudioCodecContext->channel_layout, 0);
    av_opt_set_int(mContext.ResamplerContext, "in_sample_rate", mContext.AudioCodecContext->sample_rate, 0);
    av_opt_set_int(mContext.ResamplerContext, "out_sample_rate", mContext.AudioCodecContext->sample_rate, 0);
    av_opt_set_sample_fmt(mContext.ResamplerContext, "in_sample_fmt", mContext.AudioCodecContext->sample_fmt, 0);
    av_opt_set_sample_fmt(mContext.ResamplerContext, "out_sample_fmt", AV_SAMPLE_FMT_FLT, 0);
    if (swr_init(mContext.ResamplerContext) != 0) RETURN_ERROR("Error initializing audio resampler context", false);
  }

  // Initialize video codec
  mContext.VideoCodec = avcodec_find_decoder(mContext.AudioVideoContext->streams[mContext.VideoStreamIndex]->codecpar->codec_id);
  if (mContext.VideoCodec == nullptr) RETURN_ERROR("Error finding video codec " << mContext.AudioVideoContext->streams[mContext.VideoStreamIndex]->codecpar->codec_id, false);
  mContext.VideoCodecContext = avcodec_alloc_context3(mContext.VideoCodec);
  if (mContext.VideoCodecContext == nullptr) RETURN_ERROR("Error allocating video codec context", false);
  if (avcodec_parameters_to_context(mContext.VideoCodecContext, mContext.AudioVideoContext->streams[mContext.VideoStreamIndex]->codecpar) != 0) RETURN_ERROR("Error setting parameters to video codec context", false);
  if (avcodec_open2(mContext.VideoCodecContext, mContext.VideoCodec, nullptr) != 0) RETURN_ERROR("Error opening video codec", false);
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
  if (mContext.ColorsSpaceContext == nullptr) RETURN_ERROR("Error initializing video color space context", false);

  // Time frame & duration
  AVStream* videoStream = mContext.AudioVideoContext->streams[mContext.VideoStreamIndex];
  mContext.FrameTime = (long long)(((double)videoStream->avg_frame_rate.den * 1000000000.0) / (double)videoStream->avg_frame_rate.num);
  mContext.TotalTime = (int)(((double)videoStream->avg_frame_rate.den * (double)videoStream->nb_frames * 1000.0) / (double)videoStream->avg_frame_rate.num);

  // Initialize video frame
  mContext.Frame = av_frame_alloc();
  mContext.FrameRGB[0] = av_frame_alloc();
  mContext.FrameRGB[1] = av_frame_alloc();
  if ((mContext.Frame == nullptr) || (mContext.FrameRGB[0] == nullptr) || (mContext.FrameRGB[1] == nullptr)) RETURN_ERROR("Error allocating video frames", false);

  mContext.Width = mContext.AudioVideoContext->streams[mContext.VideoStreamIndex]->codecpar->width;
  mContext.Height = mContext.AudioVideoContext->streams[mContext.VideoStreamIndex]->codecpar->height;
  int argbSize = av_image_get_buffer_size(PIXEL_FORMAT, mContext.Width, mContext.Height, 8);
  if (argbSize < 1) RETURN_ERROR("Error getting video frame size", false);
  mContext.FrameBuffer = (unsigned char*)av_malloc(argbSize);
  if (mContext.FrameBuffer == nullptr) RETURN_ERROR("Error allocating frame buffer", false);

  if (av_image_fill_arrays(&mContext.FrameRGB[0]->data[0], &mContext.FrameRGB[0]->linesize[0], mContext.FrameBuffer, PIXEL_FORMAT, mContext.Width, mContext.Height, 1) < 0)
    RETURN_ERROR("Error setting frame buffer", false);
  if (av_image_fill_arrays(&mContext.FrameRGB[1]->data[0], &mContext.FrameRGB[1]->linesize[0], mContext.FrameBuffer, PIXEL_FORMAT, mContext.Width, mContext.Height, 1) < 0)
    RETURN_ERROR("Error setting frame buffer", false);

  // Initialize audio callback
  if (mContext.AudioCodec != nullptr)
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
  static AVFrame* frame = av_frame_alloc();

  int dataSize = 0;
  if (!mContext.AudioQueue.Dequeue(packet)) return -1;

  int ret = avcodec_send_packet(&audioContext, &packet);
  av_packet_unref(&packet);
  if (ret < 0) return -1;

  while (ret == 0)
  {
    ret = avcodec_receive_frame(&audioContext, frame);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR(EOF)) return -1;

    // if error, skip frame
    if (ret < 0)  break;

    int nbFrame = frame->nb_samples;
    int outSize = av_samples_get_buffer_size(nullptr, audioContext.channels, nbFrame, AV_SAMPLE_FMT_FLT,
                                             1);
    ret = swr_convert(mContext.ResamplerContext, &converted, nbFrame,
                      (const uint8_t **) &frame->data[0], nbFrame);

    if (ret < 0) continue;

    memcpy(buffer, converted_data, outSize);
    dataSize = outSize;

    // No data yet, get more frames
    if (dataSize <= 0) continue;

    break;
  }

  // We have data, return it and come back for more later
  return dataSize;
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
    int Remaining = (int)(AudioBufferSize - AudioBufferIndex);
    if (Remaining > len) Remaining = len;
    memcpy(stream, (unsigned char *)AudioBuffer + AudioBufferIndex, Remaining);
    len -= Remaining;
    stream += Remaining;
    AudioBufferIndex += Remaining;
  }
}

TextureData& VideoEngine::GetDisplayableFrame()
{
  int frame = ((int)mContext.FrameInUse ^ 1) & 1;
  mContext.FrammeRGBLocker[frame].Lock();
  if (mContext.FrameRGB[frame] != nullptr)
  {
    AquireTexture();
    mTexture.updateFromRGBA(mContext.FrameRGB[frame]->data[0], mContext.Width, mContext.Height);
    ReleaseTexture();
  }
  mContext.FrammeRGBLocker[frame].UnLock();

  return mTexture;
}

void VideoEngine::DecodeFrames(bool decodeAudio)
{
  int VideoFrameCount = 0;
  int AudioFrameCount = 0;
  AVPacket packet;
  HighResolutionTimer timer;
  timer.Initialize(0);
  while(!CheckPendingMessages())
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
        if (avcodec_send_packet(mContext.VideoCodecContext, &packet) != 0)
          RETURN_ERROR("Error sending video packet to codec",);
        while (avcodec_receive_frame(mContext.VideoCodecContext, mContext.Frame) == 0)
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
      else if (decodeAudio && packet.stream_index == mContext.AudioStreamIndex)
      {
        mContext.AudioQueue.Enqueue(&packet);
        ++AudioFrameCount;
      }
      av_packet_unref(&packet);
    }
  }
}

void VideoEngine::FinalizeDecoder()
{
  SDL_CloseAudio();
  mContext.Dispose();
}

