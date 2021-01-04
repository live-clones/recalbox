//
// Created by bkg2k on 19/12/2020.
//

#include "PulseAudioController.h"
#include <pulse/pulseaudio.h>
#include <utils/Log.h>
#include <utils/math/Misc.h>
#include <RecalboxConf.h>

PulseAudioController::PulseAudioController()
  : mConnectionState(ConnectionState::NotConnected)
  , mPulseAudioContext(nullptr)
  , mPulseAudioMainLoop(nullptr)
{
  Thread::Start("PulseAudio");
  Initialize();
}

PulseAudioController::~PulseAudioController()
{
  Finalize();
  Thread::Stop();
}

void PulseAudioController::Initialize()
{
  // Connect to pulseaudio server
  PulseContextConnect();
  // Enumerate all output devices
  PulseEnumerateDevices();
  LOG(LogInfo) << "[PulseAudio] Enumerated " << mDeviceList.size() << " devices.";
  for(const PulseAudioCard& card : mCardList)
    LOG(LogWarning) << "[PulseAudio] Card: " << card.Index << " : " << card.Name;
  for(const PulseAudioDevice& device : mDeviceList)
    LOG(LogWarning) << "[PulseAudio] Sink: " << device.Index << " : " << device.Name << " : " << device.Description << " : " << device.CardIndex;
}

void PulseAudioController::Finalize()
{
  PulseContextDisconnect();
}

void PulseAudioController::SetVolumeCallback(pa_context *context, int success, void *userdata)
{
  (void)context;
  (void)success;

  // Get class
  PulseAudioController& This = *(PulseAudioController*)userdata;

  // Result - don't care about the actual result
  This.mSignal.Fire();
}

void PulseAudioController::SetSourceCallback(pa_context *context, int success, void *userdata)
{
  (void)context;
  (void)success;

  // Get class
  PulseAudioController& This = *(PulseAudioController*)userdata;

  // Result - don't care about the actual result
  This.mSignal.Fire();
}

void PulseAudioController::ContextStateCallback(pa_context *context, void *userdata)
{
  // Get class
  PulseAudioController& This = *(PulseAudioController*)userdata;

  // Get state
  pa_context_state_t state = pa_context_get_state(context);
	switch(state)
	{
		case PA_CONTEXT_FAILED:
		case PA_CONTEXT_TERMINATED:
    {
      This.mConnectionState = ConnectionState::Closed;
      This.mSignal.Fire();
      break;
    }
		case PA_CONTEXT_READY:
    {
      This.mConnectionState =ConnectionState::Ready;
      This.mSignal.Fire();
      break;
    }
    case PA_CONTEXT_UNCONNECTED:
    case PA_CONTEXT_CONNECTING:
    case PA_CONTEXT_AUTHORIZING:
    case PA_CONTEXT_SETTING_NAME:
    default: break;
	}
}

void PulseAudioController::EnumerateCardCallback(pa_context* context, const pa_card_info* info, int eol, void* userdata)
{
  (void)context;
  // Get class
  PulseAudioController& This = *(PulseAudioController*)userdata;

  // If eol is set to a positive number, you're at the end of the list
  if (eol > 0)
  {
    This.mSignal.Fire();
    return;
  }

  // Store card
  PulseAudioCard card;
  card.Name = info->name;
  card.Index = info->index;
  This.mSyncer.Lock();
  This.mCardList.push_back(card);
  This.mSyncer.UnLock();
}

void PulseAudioController::EnumerateSinkCallback(pa_context* context, const pa_sink_info* info, int eol, void* userdata)
{
  (void)context;
  // Get class
  PulseAudioController& This = *(PulseAudioController*)userdata;

  // If eol is set to a positive number, you're at the end of the list
  if (eol > 0)
  {
    This.mSignal.Fire();
    return;
  }

  // Some sinks do not have an active port. Just ignore them!
  if (info->active_port == nullptr) return;

  // Store device
  PulseAudioDevice device;
  device.Name = info->name;
  device.Description = info->active_port->description;
  device.Index = info->index;
  device.CardIndex = info->card;
  device.Channels = info->volume.channels;

  This.mSyncer.Lock();
  This.mDeviceList.push_back(device);
  This.mSyncer.UnLock();
}

void PulseAudioController::AdjustDeviceNames()
{
  Mutex::AutoLock lock(mSyncer);

  // Rename outputs
  /*for(PulseAudioDevice& device : mDeviceList)
  {
    std::string name;
    for(const PulseAudioCard& card : mCardList)
      if (card.Index == device.CardIndex)
      {
        name = card.Name;
        break;
      }
    device.Description = NameFiltering::Filter(name, NameFiltering::Source::Card);
  }*/

  // Re-number same names
  HashMap<std::string, int> deviceMax;
  HashMap<std::string, int> deviceCount;
  for(const PulseAudioDevice& device : mDeviceList)
  {
    int* count = deviceMax.try_get(device.Description);
    if (count != nullptr) (*count)++;
    else deviceMax.insert(device.Description, 1);
  }
  for(PulseAudioDevice& device : mDeviceList)
    if (deviceMax[device.Description] > 1)
    {
      int* count = deviceCount.try_get(device.Description);
      if (count != nullptr) (*count)++;
      else count = &(deviceCount[device.Description] = 1);
      device.Description.append(LEGACY_STRING(" #"))
            .append(Strings::ToString(*count));
    }
}

HashMap<int, std::string> PulseAudioController::GetPlaybackList()
{
  Mutex::AutoLock lock(mSyncer);

  HashMap<int, std::string> result;
  for(const PulseAudioDevice& device : mDeviceList)
    result[device.Index] = device.Description;
  return result;
}

std::string PulseAudioController::SetDefaultPlayback(const std::string& playbackName)
{
  Mutex::AutoLock lock(mSyncer);

  int defaultIndex = (int)INTMAX_MAX;
  for(const PulseAudioDevice& device : mDeviceList)
  {
    if (device.Description == playbackName)
    {
      SetDefaultPlayback(device.Index);
      return playbackName;
    }
    if (device.Index < defaultIndex)
      defaultIndex = device.Index;
  }
  // Not found, fallback to lowest index
  SetDefaultPlayback(defaultIndex);
  return GetPlaybackList()[defaultIndex];
}

int PulseAudioController::GetVolume()
{
  return Math::clampi(RecalboxConf::Instance().AsInt("audio.volume"),0, 100);
}

void PulseAudioController::SetVolume(int volume)
{
  if (mPulseAudioContext == nullptr) return;

  volume = Math::clampi(volume, 0, 100);

  for(const PulseAudioDevice& device : mDeviceList)
  {
    pa_cvolume volumeStructure;
    pa_cvolume_init(&volumeStructure);
    pa_cvolume_set(&volumeStructure, device.Channels, (PA_VOLUME_NORM * volume) / 100);

    // Set volume
    pa_context_set_sink_volume_by_index(mPulseAudioContext, device.Index, &volumeStructure, nullptr, nullptr); //SetVolumeCallback, this);
    // Wait for result
    //mSignal.WaitSignal();
  }
}

void PulseAudioController::SetDefaultPlayback(int identifier)
{
  Mutex::AutoLock lock(mSyncer);

  for(const PulseAudioDevice& device : mDeviceList)
    if (identifier == device.Index)
    {
      // Set source
      pa_context_set_default_sink(mPulseAudioContext, device.Name.data(), SetSourceCallback, this);
      // Wait for result
      mSignal.WaitSignal();
      // Set sound on
      pa_context_set_sink_mute_by_index(mPulseAudioContext, device.Index, 0, SetSourceCallback, this);
      // Wait for result
      mSignal.WaitSignal();
      break;
    }
}

void PulseAudioController::Break()
{
  pa_mainloop_quit(mPulseAudioMainLoop, 1);
}

void PulseAudioController::Run()
{
  // Create a mainloop API and connection to the default server
  mPulseAudioMainLoop = pa_mainloop_new();
  pa_mainloop_api* pa_mlapi = pa_mainloop_get_api(mPulseAudioMainLoop);
  mPulseAudioContext = pa_context_new(pa_mlapi, "Recalbox");
  // This function defines a callback so the server will tell us it's state.
  pa_context_set_state_callback(mPulseAudioContext, ContextStateCallback, this);
  // This function connects to the pulse server
  pa_context_connect(mPulseAudioContext, nullptr, pa_context_flags::PA_CONTEXT_NOFLAGS, nullptr);

  // Thread loop
  int result = 0;
  pa_mainloop_run(mPulseAudioMainLoop, &result);

  // Deinit
  pa_context_unref(mPulseAudioContext);
  pa_mainloop_free(mPulseAudioMainLoop);
}

void PulseAudioController::PulseContextConnect()
{
  // Wait for response
  mSignal.WaitSignal();
  LOG(LogInfo) << "[PulseAudio] Connected to Server.";
}

void PulseAudioController::PulseContextDisconnect()
{
  // Disconnect from pulse server
  pa_context_disconnect(mPulseAudioContext);
  LOG(LogInfo) << "[PulseAudio] Disconnected to Server.";
}

void PulseAudioController::PulseEnumerateDevices()
{
  /*
  // Enumerate cards
  pa_operation* operations = pa_context_get_card_info_list(mPulseAudioContext, EnumerateCardCallback, this);
  // Wait for response
  mSignal.WaitSignal();
  // Release
  pa_operation_unref(operations);
  */

  // Enumerate sinks
  pa_operation* operations = pa_context_get_sink_info_list(mPulseAudioContext, EnumerateSinkCallback, this);
  // Wait for response
  mSignal.WaitSignal();
  // Release
  pa_operation_unref(operations);

  // Adjust devices
  AdjustDeviceNames();
}
