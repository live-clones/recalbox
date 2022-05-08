//
// Created by bkg2k on 19/12/2020.
//

#include "PulseAudioController.h"
#include <pulse/pulseaudio.h>
#include <utils/Log.h>
#include <utils/locale/LocaleHelper.h>
#include <utils/math/Misc.h>
#include <RecalboxConf.h>
#include <hardware/Board.h>

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
  // Enumerate all sinks and cards
  Refresh();
  // Subscribe to changes
  //PulseSubscribe();

  { LOG(LogInfo) << "[PulseAudio] Initialized."; }
}

void PulseAudioController::Finalize()
{
  PulseContextDisconnect();
}

void PulseAudioController::SetProfileCallback(pa_context *context, int success, void *userdata)
{
  (void)context;
  (void)success;

  { LOG(LogDebug) << "[PulseAudio] Set Profile result: " << (success != 0 ? "SUCCESS" : "FAIL"); }

  // Get class
  PulseAudioController& This = *(PulseAudioController*)userdata;

  // Result - don't care about the actual result
  This.mSignal.Fire();
}

void PulseAudioController::SetVolumeCallback(pa_context *context, int success, void *userdata)
{
  (void)context;
  (void)success;

  { LOG(LogDebug) << "[PulseAudio] Set Volume result: " << (success != 0 ? "SUCCESS" : "FAIL"); }

  // Get class
  PulseAudioController& This = *(PulseAudioController*)userdata;

  // Result - don't care about the actual result
  This.mSignal.Fire();
}

void PulseAudioController::SetMuteCallback(pa_context *context, int success, void *userdata)
{
  (void)context;
  (void)success;

  { LOG(LogDebug) << "[PulseAudio] Set Mute result: " << (success != 0 ? "SUCCESS" : "FAIL"); }

  // Get class
  PulseAudioController& This = *(PulseAudioController*)userdata;

  // Result - don't care about the actual result
  This.mSignal.Fire();
}

void PulseAudioController::SetSinkCallback(pa_context *context, int success, void *userdata)
{
  (void)context;
  (void)success;

  { LOG(LogDebug) << "[PulseAudio] Set Sink result: " << (success != 0 ? "SUCCESS" : "FAIL"); }

  // Get class
  PulseAudioController& This = *(PulseAudioController*)userdata;

  // Result - don't care about the actual result
  This.mSignal.Fire();
}

void PulseAudioController::SetPortCallback(pa_context *context, int success, void *userdata)
{
  (void)context;
  (void)success;

  { LOG(LogDebug) << "[PulseAudio] Set Port result: " << (success != 0 ? "SUCCESS" : "FAIL"); }

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
      { LOG(LogInfo) << "[PulseAudio] Disconnected from PulseAudio server."; }
      This.mConnectionState = ConnectionState::Closed;
      This.mSignal.Fire();
      break;
    }
		case PA_CONTEXT_READY:
    {
      { LOG(LogInfo) << "[PulseAudio] Connected to PulseAudio server."; }
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

void PulseAudioController::SubscriptionCallback(pa_context *context, pa_subscription_event_type_t type, uint32_t index, void *userdata)
{
  (void)context;
  // Get class
  PulseAudioController& This = *(PulseAudioController*)userdata;
  (void)This;

  unsigned int facility = type & PA_SUBSCRIPTION_EVENT_FACILITY_MASK;
  unsigned int typ = (pa_subscription_event_type_t)(type & PA_SUBSCRIPTION_EVENT_TYPE_MASK);

  std::string typeStr;
  switch(typ)
  {
    case PA_SUBSCRIPTION_EVENT_NEW: typeStr = "NEW"; break;
    case PA_SUBSCRIPTION_EVENT_CHANGE: typeStr = "CHANGE"; break;
    case PA_SUBSCRIPTION_EVENT_REMOVE: typeStr = "REMOVE"; break;
    default: typeStr = "UNKNOWN TYPE"; break;
  }
  std::string eventStr;
  switch(facility)
  {
    case PA_SUBSCRIPTION_EVENT_SINK: eventStr = "SINK"; break;
    case PA_SUBSCRIPTION_EVENT_SOURCE: eventStr = "SOURCE"; break;
    case PA_SUBSCRIPTION_EVENT_SINK_INPUT: eventStr = "SINK_INPUT"; break;
    case PA_SUBSCRIPTION_EVENT_SOURCE_OUTPUT: eventStr = "SOURCE_OUTPUT"; break;
    case PA_SUBSCRIPTION_EVENT_MODULE: eventStr = "MODULE"; break;
    case PA_SUBSCRIPTION_EVENT_CLIENT: eventStr = "CLIENT"; break;
    case PA_SUBSCRIPTION_EVENT_SAMPLE_CACHE: eventStr = "SAMPLE CACHE"; break;
    case PA_SUBSCRIPTION_EVENT_SERVER: eventStr = "SERVER"; break;
    case PA_SUBSCRIPTION_EVENT_AUTOLOAD: eventStr = "AUTOLOAD"; break;
    case PA_SUBSCRIPTION_EVENT_CARD: eventStr = "CARD"; break;
    default: eventStr = "UNKNOWN EVENT"; break;
  }

  { LOG(LogError) << "[PulseAudio] EVENT! Type: " << typeStr << " - Event: " << eventStr << " - Index: " << index; }
}

AudioIcon PulseAudioController::GetPortIcon(const pa_sink_port_info& info)
{
#if PA_CHECK_VERSION(14,0,0)
  switch(info.type)
  {
    case PA_DEVICE_PORT_TYPE_SPEAKER: return AudioIcon::Speakers; break;
    case PA_DEVICE_PORT_TYPE_HEADPHONES: return AudioIcon::Headphones; break;
    case PA_DEVICE_PORT_TYPE_HDMI:
    case PA_DEVICE_PORT_TYPE_TV:
    case PA_DEVICE_PORT_TYPE_VIDEO:
      return AudioIcon::Screens;
  }
#endif
  return AudioIcon::Unidentified;
}

AudioIcon PulseAudioController::GetPortIcon(const pa_card_port_info& info)
{
  const char* icon = pa_proplist_gets(info.proplist, "device.icon_name");
  if (icon == nullptr) return AudioIcon::Unidentified;

  if (strcmp(icon, "audio-speakers") == 0) return AudioIcon::Speakers;
  if (strcmp(icon, "audio-headphones") == 0) return AudioIcon::Headphones;
  if (strcmp(icon, "video-display") == 0) return AudioIcon::Screens;

  return AudioIcon::Unidentified;
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

  // New card
  Card newCard;
  newCard.Name = info->name;
  newCard.Description = GetCardDescription(*info);
  newCard.Index = (int)info->index;
  newCard.HasActiveProfile = false;
  newCard.Ports.clear();

  { LOG(LogDebug) << "[PulseAudio] Card #" << newCard.Index << " : " << newCard.Name; }

  // Active profile
  std::string activeProfileName;
  if (info->active_profile2 != nullptr)
  {
    activeProfileName = info->active_profile2->name;
    { LOG(LogDebug) << "[PulseAudio] Active Profile: " << info->active_profile2->description << " ("
                    << info->active_profile2->name << ") - Available "
                    << (info->active_profile2->available != 0 ? "YES" : "NO") << " - Priority "
                    << info->active_profile2->priority; }
  }

  // Ports (device outputs)
  for(int i = (int)info->n_ports; --i >= 0; )
  {
    pa_card_port_info& portInfo = *info->ports[i];
    if ((portInfo.direction & PA_DIRECTION_OUTPUT) == 0) continue; // Ignore non-output ports

    Port newPort;
    newPort.Name = portInfo.name;
    newPort.Icon = GetPortIcon(portInfo);
    newPort.Description = GetPortDescription(portInfo, newPort.Icon);
    newPort.Available = portInfo.available != PA_PORT_AVAILABLE_NO;
    newPort.Priority = (int)portInfo.priority;
    newPort.InternalIndex = (int)newCard.Ports.size();
    newPort.Profiles.clear();

    { LOG(LogDebug) << "[PulseAudio]   Port " << newPort.Description << " (" << newPort.Name << ") - Available " << (newPort.Available ? "YES" : "NO") << " - Priority " << newPort.Priority; }

    // Port's supported profiles
    for(int p = (int)portInfo.n_profiles; --p >= 0; )
    {
      pa_card_profile_info2& profileInfo = *portInfo.profiles2[p];
      if (profileInfo.n_sinks <= 0) continue; // Ignore source-only profiles

      Profile newProfile;
      newProfile.Name = profileInfo.name;
      newProfile.Description = profileInfo.description;
      newProfile.Available = profileInfo.available != 0;
      newProfile.Priority = (int)profileInfo.priority;

      { LOG(LogDebug) << "[PulseAudio]     Profile " << newProfile.Description << " (" << newProfile.Name << ") - Available " << (newProfile.Available ? "YES" : "NO") << " - Priority " << newProfile.Priority; }

      // Check if this profile is the active one
      newCard.HasActiveProfile |= (newProfile.Name == activeProfileName);

      // Add profile to the port
      newPort.Profiles.push_back(newProfile);
    }

    // Add port to the card
    newCard.Ports.push_back(newPort);
  }

  // Store card
  This.mSyncer.Lock();
  This.mCards.push_back(newCard);
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

  Sink newSink;
  newSink.Name = info->name;
  newSink.Description = info->description;
  newSink.Index = (int)info->index;
  newSink.CardIndex = (int)info->card;
  newSink.Channels = info->channel_map.channels;
  newSink.Ports.clear();

  { LOG(LogInfo) << "[PulseAudio] Sink #" << newSink.Index << ' ' << newSink.Name << " found."; }

  // Collect available ports
  for(int i = (int)info->n_ports; --i >= 0; )
  {
    pa_sink_port_info& portInfo = *info->ports[i];

    Port newPort;
    newPort.Name = portInfo.name;
    newPort.Icon = GetPortIcon(portInfo);
    newPort.Description = GetPortDescription(portInfo, newPort.Icon);
    newPort.Available = portInfo.available != PA_PORT_AVAILABLE_NO;
    newPort.Priority = (int)portInfo.priority;
    newSink.Ports.push_back(newPort);
    { LOG(LogInfo) << "[PulseAudio] Port " << info->ports[i]->name << " - " << info->ports[i]->description; }
  }

  This.mSyncer.Lock();
  This.mSinks.push_back(newSink);
  This.mSyncer.UnLock();
}

void PulseAudioController::AddSpecialPlaybacks(IAudioController::DeviceList& list)
{
  switch(Board::Instance().GetBoardType())
  {
    case BoardType::OdroidAdvanceGo:
    case BoardType::OdroidAdvanceGoSuper:
    {
      list.push_back({ std::string("\u26ab ").append(" \uf1e0 ").append(_("auto select")).append(" \u26ab"), sAutoSwitch, AudioIcon::Auto });
      break;
    }
    case BoardType::UndetectedYet:
    case BoardType::Unknown:
    case BoardType::Pi0:
    case BoardType::Pi02:
    case BoardType::Pi1:
    case BoardType::Pi2:
    case BoardType::Pi3:
    case BoardType::Pi3plus:
    case BoardType::Pi4:
    case BoardType::Pi400:
    case BoardType::UnknownPi:
    case BoardType::PCx86:
    case BoardType::PCx64:
    default: break;
  }
}

IAudioController::DeviceList PulseAudioController::GetPlaybackList()
{
  // API Sync'
  Mutex::AutoLock locker(mAPISyncer);

  // Refresh all objects
  Refresh();

  Mutex::AutoLock lock(mSyncer);
  DeviceList result;

  AddSpecialPlaybacks(result);

  for(const Sink& sink: mSinks)
  {
    const Card* card = GetCardByIndex(sink.CardIndex);
    // Classic audio sink (card + port + profile)
    if (card != nullptr && card->Ports.size() > 0)
    {
      for(const Port& port : card->Ports)
      {
        bool available = port.Available;

        // card + port
        std::string device;
        switch(port.Icon)
        {
          case AudioIcon::Auto: device = " \uf1e0 "; break;
          case AudioIcon::Unidentified: device = " \uf1b0 "; break;
          case AudioIcon::Speakers: device = " \uf1e1 "; break;
          case AudioIcon::Headphones: device = " \uf1e2 "; break;
          case AudioIcon::Screens: device = " \uf1e3 "; break;
        }
        device.append(port.Description + " - " + card->Description);
        std::string displayable = available ? std::string("\u26ab ").append(device).append(" \u26ab") : std::string("\u26aa ").append(device).append(" \u26aa") ;
        const Profile* selectedProfile = GetBestProfile2(card, &port);
        if (selectedProfile)
          result.push_back({ displayable, card->Name+":"+port.Name+":"+selectedProfile->Name, AudioIcon::Auto });

        if (CountAvailableProfiles(port) > 1)
        {
          // profiles of card + ports
          for(const Profile& profile : port.Profiles)
          {
            if (! profile.Available)
              continue;

            std::string device = " ↳ "+profile.Description;
            if (profile.Name == selectedProfile->Name)
                device.append(" \uf006");
            std::string displayable = available ? std::string("      \u26ab ").append(device).append(" \u26ab") : std::string("      \u26aa ").append(device).append(" \u26aa") ;
            result.push_back({ displayable, card->Name+":"+port.Name+":"+profile.Name, AudioIcon::Auto });
          }
        }
      }
    }
    // PulseAudio Filter (eg. stereo to mono)
    else
      result.push_back({ std::string("\u26ab ").append(sink.Description).append(" \u26ab"), sink.Name+":", AudioIcon::Auto });
  }

  return result;
}

uint PulseAudioController::CountAvailableProfiles(const Port& port)
{
  uint availableProfiles = 0;
  for(const Profile& profile : port.Profiles)
  {
    if (profile.Available)
      availableProfiles++;
  }
  return availableProfiles;
}

const PulseAudioController::Card* PulseAudioController::FirstCard()
{
  if (!mCards.empty())
    return &mCards[0];

  return nullptr;
}

const PulseAudioController::Port* PulseAudioController::FirstPort(const Card& card)
{
  if (!card.Ports.empty())
    return &card.Ports[0];

  return nullptr;
}

const PulseAudioController::Card* PulseAudioController::LookupCard(const std::string& name)
{
  for(const Card& card : mCards)
    if (name == card.Name)
      return &card;

  return nullptr;
}

const PulseAudioController::Port* PulseAudioController::LookupPort(const Card& card, const std::string& name)
{
  for(const Port& port : card.Ports)
    if (name == port.Name)
      return &port;

  return nullptr;
}

const PulseAudioController::Profile* PulseAudioController::LookupProfile(const Card& card, const std::string& name)
{
  for(const Port& port : card.Ports)
    for(const Profile& profile : port.Profiles)
      if (name == profile.Name)
        return &profile;

  return nullptr;
}

bool PulseAudioController::HasPort(const PulseAudioController::Sink& sink, const PulseAudioController::Port& port)
{
  for(const Port& sinkPort : sink.Ports)
    if (sinkPort.Name == port.Name)
      return true;

  return false;
}

const PulseAudioController::Sink* PulseAudioController::LookupSink(const std::string& name)
{
  for(const Sink& sink : mSinks)
    if (name == sink.Name)
      return &sink;

  return nullptr;
}

const PulseAudioController::Sink* PulseAudioController::GetSinkFromCardPort(const Card* card, const Port* port)
{
  for(const Sink& sink : mSinks)
    if (card->Index == sink.CardIndex)
      for (const Port& sinkPort : sink.Ports)
        if (sinkPort.Name == port->Name)
          return &sink;

  return nullptr;
}

bool PulseAudioController::IsPortAvailable(const std::string& portName)
{
  bool available = false;
  for(const Card& card : mCards)
    for(const Port& port : card.Ports)
      if (port.Name == portName)
        available = port.Available;

  return available;
}

std::string PulseAudioController::AdjustSpecialPlayback(const std::string& originalPlaybackName, bool& allprocessed)
{
  allprocessed = false;

  switch(Board::Instance().GetBoardType())
  {
    case BoardType::OdroidAdvanceGo:
    case BoardType::OdroidAdvanceGoSuper:
    {
      // Patch to bypass buggy PulseAudio on GoA/GoS
      if (originalPlaybackName == IAudioController::sAutoSwitch)
      {
        bool headphonePlugged = IsPortAvailable("analog-output-headphones");
        LOG(LogInfo) << "[PulseAudio] AutoSwitch set to " << (headphonePlugged ? "Headphones" : "Speakers");
        if (system(headphonePlugged? "amixer sset 'Playback Path' HP" :"amixer sset 'Playback Path' SPK") != 0)
        { LOG(LogError) << "[PulseAudio] Error setting playback path on GoA/GoA"; }
      }
      else if (originalPlaybackName == "alsa_output.platform-rk817-sound.multichannel-output:analog-output-headphones")
      {
        if (system("amixer sset 'Playback Path' HP") != 0)
        { LOG(LogError) << "[PulseAudio] Error setting playback path on GoA/GoA"; }
      }
      else if (originalPlaybackName == "alsa_output.platform-rk817-sound.multichannel-output:multichannel-output")
      {
        if (system("amixer sset 'Playback Path' SPK") != 0)
        { LOG(LogError) << "[PulseAudio] Error setting playback path on GoA/GoA"; }
      }
      else { LOG(LogError) << "[PulseAudio] Unreconized GoA/GoS output: " << originalPlaybackName; }
      allprocessed = true;
      break;
    }
    case BoardType::UndetectedYet:
    case BoardType::Unknown:
    case BoardType::Pi0:
    case BoardType::Pi02:
    case BoardType::Pi1:
    case BoardType::Pi2:
    case BoardType::Pi3:
    case BoardType::Pi3plus:
    case BoardType::Pi4:
    case BoardType::Pi400:
    case BoardType::UnknownPi:
    case BoardType::PCx86:
    case BoardType::PCx64:
    default: break;
  }

  return originalPlaybackName;
}

std::string PulseAudioController::SetDefaultPlayback(const std::string& originalPlaybackName)
{
  // API Sync'
  Mutex::AutoLock locker(mAPISyncer);

  bool allProcessed = false;
  bool isAudioDeviceSink = true;
  std::string playbackName = AdjustSpecialPlayback(originalPlaybackName, allProcessed);
  if (allProcessed) return playbackName; // AjustSpecialPlayback did some tricks, no need to go further

  const Card* card = nullptr;
  const Sink* sink = nullptr;
  const Port* port = nullptr;
  const Profile* profile = nullptr;
  {
    Mutex::AutoLock lock(mSyncer);

    { LOG(LogInfo) << "[PulseAudio] Switching to " << playbackName; }

    std::string deviceName;
    std::string portName;
    if (!Strings::SplitAt(playbackName, ':', deviceName, portName, true))
    { LOG(LogError) << "[PulseAudio] Invalid playbackname: " << playbackName; }

    sink = LookupSink(deviceName);
    card = LookupCard(deviceName);

    // Bail out if sink or card not available anymore
    // This can happend when migrating or when audio cards have changed
    if (!sink and !card)
    {
      LOG(LogWarning) << "[PulseAudio] Invalid sink or card: " << deviceName;
      return playbackName;
    }

    // Card is set in the audio.device configuation
    // Set best profile of card (higher priority)
    if (card)
    {
      std::string portName2;
      std::string profileName;
      if (!Strings::SplitAt(portName, ':', portName2, profileName, true))
        portName2 = portName; // no profile given
      port = LookupPort(*card, portName2);
      profile = LookupProfile(*card, profileName);
      // Get best profile regarding selected card/port
      if (port == nullptr) { LOG(LogError) << "[PulseAudio] No port '" << portName2 << "' available on sound card " << card->Description; return playbackName; }
      if (!profile)
      {
        profile = GetBestProfile2(card, port);
        if (profile == nullptr) { LOG(LogError) << "[PulseAudio] No profile available!"; return playbackName; } // should never happend
      }

      { LOG(LogInfo) << "[PulseAudio] Activating profile " << profile->Name << " for card #" << card->Index << ' ' << card->Name; }

      pa_operation* profileOp = pa_context_set_card_profile_by_index(mPulseAudioContext, card->Index, profile->Name.data(), SetProfileCallback,this);
      // Wait for response
      mSignal.WaitSignal(sTimeOut);
      // Release
      pa_operation_unref(profileOp);

      // need to reload sinks after changing card profile
      isAudioDeviceSink = false;
    }
  }

  if (!isAudioDeviceSink)
    PulseEnumerateSinks();

  {
    Mutex::AutoLock lock(mSyncer);
    // profile was changed, audio.device is a card:port:profile, sink must be found
    if (!sink)
      sink = GetSinkFromCardPort(card, port);
    // Set port
    if (port)
    {
      if (!sink)
        { LOG(LogError) << "[PulseAudio] No sink found!"; return playbackName; } // should never happend again

      { LOG(LogDebug) << "[PulseAudio] Switching sink '" << sink->Name << "' to port " << port->Name; }
      pa_operation* op = pa_context_set_sink_port_by_name(mPulseAudioContext, sink->Name.data(), port->Name.data(), SetPortCallback, this);
      // Wait for result
      mSignal.WaitSignal(sTimeOut);
      // Release
      pa_operation_unref(op);
      { LOG(LogInfo) << "[PulseAudio] Sink '" << sink->Name << "' has been switched to port " << port->Name; }
    }

    // Set sink the default one
    pa_operation* op = pa_context_set_default_sink(mPulseAudioContext, sink->Name.data(), SetSinkCallback, this);
    // Wait for result
    mSignal.WaitSignal(sTimeOut);
    // Release
    pa_operation_unref(op);
     { LOG(LogDebug) << "[PulseAudio] Sink '" << sink->Name << "' has been set as default sink."; }

  }

  // Return new  playback
  if (isAudioDeviceSink)
  {
    if (port != nullptr)
      return std::string(sink->Name).append(1, ':').append(port->Name);
    else
      return std::string(sink->Name).append(1, ':');
  }
  else
  {
    return card->Name+":"+port->Name+":"+profile->Name;
  }
}

int PulseAudioController::GetVolume()
{
  // API Sync'
  Mutex::AutoLock locker(mAPISyncer);

  return Math::clampi(RecalboxConf::Instance().AsInt("audio.volume"),0, 100);
}

void PulseAudioController::SetVolume(int volume)
{
  // API Sync'
  Mutex::AutoLock locker(mAPISyncer);

  if (mPulseAudioContext == nullptr) return;

  volume = Math::clampi(volume, 0, 100);

  Mutex::AutoLock lock(mSyncer);

  for(Sink& sink : mSinks)
  {
    pa_cvolume volumeStructure;
    pa_cvolume_init(&volumeStructure);
    pa_cvolume_set(&volumeStructure, sink.Channels, (PA_VOLUME_NORM * volume) / 100);

    // Set volume
    pa_operation* op = pa_context_set_sink_volume_by_index(mPulseAudioContext, sink.Index, &volumeStructure, SetVolumeCallback, this);
    // Wait for result
    mSignal.WaitSignal(sTimeOut);
    // Release
    pa_operation_unref(op);
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
  mSignal.WaitSignal(sTimeOut);
  { LOG(LogInfo) << "[PulseAudio] Connection to PulseAudio complete."; }
}

void PulseAudioController::PulseContextDisconnect()
{
  // Disconnect from pulse server
  pa_context_disconnect(mPulseAudioContext);
  { LOG(LogInfo) << "[PulseAudio] Disconnected to Server."; }
}

const PulseAudioController::Profile* PulseAudioController::GetBestProfile2(const PulseAudioController::Card* targetCard, const PulseAudioController::Port* targetPort)
{
  int priority = -1;

  if (!targetCard || !targetPort)
    return nullptr;

  const PulseAudioController::Profile* bestProfile = nullptr;
  { LOG(LogDebug) << "[PulseAudio] Get Best profile for Card : " << ((targetCard != nullptr) ? targetCard->Description : "NULL"); }
  { LOG(LogDebug) << "[PulseAudio] Get Best profile for Port : " << ((targetPort != nullptr) ? targetPort->Description : "NULL"); }

  // Card loop
  for(const Card& card : mCards)
  {
    // Card is non null? Only target the selected card
    if (targetCard != nullptr && targetCard != &card) continue;

    for (const Port& port : card.Ports)
    {
      // Port is non null? Only target the selected port
      if (targetPort != nullptr && targetPort != &port) continue;

      for(const Profile& profile : port.Profiles)
      {
        // Get highest priority port with profile available
        if (profile.Priority > priority && profile.Available)
        {
          priority = profile.Priority;
          bestProfile = &profile;
          { LOG(LogDebug) << "[PulseAudio] Get Best profile for Card : " << targetCard->Name << " Port : "  << targetPort->Name << " Profile : " << profile.Name << " with priority=" << profile.Priority; }
        }
      }
    }
  }
  return bestProfile;
}

const PulseAudioController::Profile* PulseAudioController::GetBestProfile(const PulseAudioController::Card*& targetCard, const PulseAudioController::Port*& targetPort)
{
  int priority = -1;
  const Port* selectedPort = nullptr;
  const Card* selectedCard = nullptr;

  { LOG(LogDebug) << "[PulseAudio] Get Best profile for Card : " << ((targetCard != nullptr) ? targetCard->Description : "NULL"); }
  { LOG(LogDebug) << "[PulseAudio] Get Best profile for Port : " << ((targetPort != nullptr) ? targetPort->Description : "NULL"); }

  // Card loop
  for(const Card& card : mCards)
  {
    // Card is non null? Only target the selected card
    if (targetCard != nullptr && targetCard != &card) continue;

    for (const Port& port : card.Ports)
    {
      // Port is non null? Only target the selected port
      if (targetPort != nullptr && targetPort != &port) continue;

      bool available = false;
      for(const Profile& profile : port.Profiles)
        available |= profile.Available;

      // Get highest priority port with profile available
      if (port.Priority > priority && available)
      {
        priority = port.Priority;
        selectedPort = &port;
        selectedCard = &card;
        { LOG(LogDebug) << "[PulseAudio] Get Best profile, card - port selected : " << selectedCard->Description << " - " << selectedPort->Description; }
      }
    }
  }

  // Store selected
  targetCard = selectedCard;
  targetPort = selectedPort;

  // Check port
  if (selectedPort == nullptr)
  { LOG(LogWarning) << "[PulseAudio] No port with available profiles!"; return nullptr; }

  // Check available profiles
  if (selectedPort->Profiles.empty())
  { LOG(LogWarning) << "[PulseAudio] Card #" << selectedCard->Index << ' ' << selectedCard->Name << ", Port " << selectedPort->Description << " has no profile!"; return nullptr; }

  // Seek for the highest profile
  const Profile* selectedProfile = &selectedPort->Profiles[0];
  for (const Profile& profile : selectedPort->Profiles)
    selectedProfile = (profile.Priority > selectedProfile->Priority) ? &profile : selectedProfile;

  { LOG(LogDebug) << "[PulseAudio] Get Best profile : " << selectedProfile->Description; }

  return selectedProfile;
}

void PulseAudioController::SetDefaultProfiles()
{
  for(Card& card : mCards)
  {
    if (card.HasActiveProfile) continue;
    if (card.Ports.empty()) { LOG(LogWarning) << "[PulseAudio] Card #" << card.Index << ' ' << card.Name << " has no port!"; continue; }

    { LOG(LogInfo) << "[PulseAudio] Card #" << card.Index << ' ' << card.Name << " has no profile activated."; }

    const Card* cardTemp = &card;
    const Port* portTemp = nullptr;
    const Profile* selectedProfile = GetBestProfile(cardTemp, portTemp);
    if (selectedProfile == nullptr) continue;

    // Activate selected profile
    { LOG(LogInfo) << "[PulseAudio] Activating profile " << selectedProfile->Description << " for card #" << card.Index << ' ' << card.Name; }

    pa_operation* profileOp = pa_context_set_card_profile_by_index(mPulseAudioContext, card.Index, selectedProfile->Name.data(), SetProfileCallback, this);
    // Wait for response
    mSignal.WaitSignal(sTimeOut);
    // Release
    pa_operation_unref(profileOp);
  }
}

void PulseAudioController::PulseEnumerateSinks()
{
  mSyncer.Lock();
  mSinks.clear();
  mSyncer.UnLock();

  // Enumerate cards
  { LOG(LogInfo) << "[PulseAudio] Enumerating Sinks"; }
  pa_operation* sinkOp = pa_context_get_sink_info_list(mPulseAudioContext, EnumerateSinkCallback, this);
  // Wait for response
  mSignal.WaitSignal(sTimeOut);
  // Release
  pa_operation_unref(sinkOp);
}

void PulseAudioController::PulseEnumerateCards()
{
  mSyncer.Lock();
  mCards.clear();
  mSyncer.UnLock();

  // Enumerate cards
  { LOG(LogInfo) << "[PulseAudio] Enumerating Cards."; }
  pa_operation* cardOp = pa_context_get_card_info_list(mPulseAudioContext, EnumerateCardCallback, this);
  // Wait for response
  mSignal.WaitSignal(sTimeOut);
  // Release
  pa_operation_unref(cardOp);

  // Activate the best profile on every card if they do not have default profile
  SetDefaultProfiles();

}

void PulseAudioController::PulseSubscribe()
{
  { LOG(LogInfo) << "[PulseAudio] Subscribing to events"; }

  pa_context_set_subscribe_callback(mPulseAudioContext, SubscriptionCallback, this);
  pa_context_subscribe(mPulseAudioContext, PA_SUBSCRIPTION_MASK_ALL, nullptr, nullptr);
}

std::string PulseAudioController::GetCardDescription(const pa_card_info& info)
{
  std::string result;

  // Get best name
  const char* cardName = pa_proplist_gets(info.proplist, "alsa.card_name");
  if (cardName == nullptr) cardName = pa_proplist_gets(info.proplist, "bluez.alias");
  if (cardName == nullptr) cardName = pa_proplist_gets(info.proplist, "device.description");
  if (cardName == nullptr) cardName = "Unlabelled device";

  // Change
  switch(Board::Instance().GetBoardType())
  {
    case BoardType::Pi0:
    case BoardType::Pi02:
    case BoardType::Pi1:
    case BoardType::Pi2:
    case BoardType::Pi3:
    case BoardType::Pi3plus:
    case BoardType::Pi4:
    case BoardType::Pi400:
    case BoardType::UnknownPi:
    {
      result = cardName;
      Strings::ReplaceAllIn(result, "bcm2835", "");
      Strings::ReplaceAllIn(result, "vc4-hdmi", "HDMI");
      break;
    }
    case BoardType::OdroidAdvanceGo:
    case BoardType::OdroidAdvanceGoSuper: break;
    case BoardType::PCx86:
    case BoardType::PCx64:
    case BoardType::UndetectedYet:
    case BoardType::Unknown:
    default:
    {
      result = cardName;
      break;
    }
  }

  if (result.empty()) result.append("Card #").append(Strings::ToString(info.index));

  return Strings::Trim(result);
}

const PulseAudioController::Card* PulseAudioController::GetCardByIndex(int index) 
{
  for(Card& card : mCards)
    if (card.Index == index)
      return &card;
  return nullptr;
}

std::string PulseAudioController::GetPortDescription(const pa_sink_port_info& info, AudioIcon& icon)
{
  std::string result = info.description;
  std::string low(Strings::ToLowerASCII(result));

  // Change
  switch(Board::Instance().GetBoardType())
  {
    case BoardType::Pi0:
    case BoardType::Pi02:
    case BoardType::Pi1:
    case BoardType::Pi2:
    case BoardType::Pi3:
    case BoardType::Pi3plus:
    case BoardType::Pi4:
    case BoardType::Pi400:
    case BoardType::UnknownPi: break;
    case BoardType::OdroidAdvanceGo:
    case BoardType::OdroidAdvanceGoSuper:
    {
      if (low == "multichannel output")
      {
        result = "Internal Speakers";
        icon = AudioIcon::Speakers;
      }
    }
    case BoardType::PCx86:
    case BoardType::PCx64:
    case BoardType::UndetectedYet:
    case BoardType::Unknown:
    default: break;
  }

  return result;
}

std::string PulseAudioController::GetPortDescription(const pa_card_port_info& info, AudioIcon& icon)
{
  std::string result = info.description;
  std::string low(Strings::ToLowerASCII(result));

  // Change
  switch(Board::Instance().GetBoardType())
  {
    case BoardType::Pi0:
    case BoardType::Pi02:
    case BoardType::Pi1:
    case BoardType::Pi2:
    case BoardType::Pi3:
    case BoardType::Pi3plus:
    case BoardType::Pi4:
    case BoardType::Pi400:
    case BoardType::UnknownPi: break;
    case BoardType::OdroidAdvanceGo:
    case BoardType::OdroidAdvanceGoSuper:
    {
      if (low == "multichannel output")
      {
        result = "Internal Speakers";
        icon = AudioIcon::Speakers;
      }
    }
    case BoardType::PCx86:
    case BoardType::PCx64:
    case BoardType::UndetectedYet:
    case BoardType::Unknown:
    default: break;
  }

  return result;
}

void PulseAudioController::Refresh()
{
  // API Sync'
  Mutex::AutoLock locker(mAPISyncer);

  if (mConnectionState == ConnectionState::Ready)
  {
    PulseEnumerateSinks();
    PulseEnumerateCards();
  }
}
