#include "VolumeControl.h"

#include "utils/Log.h"
#include <utils/math/Misc.h>
#include <platform.h>

const char* VolumeControl::getMixerName()
{
  switch(getRaspberryVersion())
  {
    case RaspberryGeneration::Pi4: return "HDMI";
    case RaspberryGeneration::Pi0:
    case RaspberryGeneration::Pi1:
    case RaspberryGeneration::Pi2:
    case RaspberryGeneration::Pi3:
    case RaspberryGeneration::Pi3plus: return "PCM";
    case RaspberryGeneration::NotYetKnown:
    case RaspberryGeneration::UndetectedYet:
    case RaspberryGeneration::NotRaspberry:
    default: break;
  }
  return "Master";
}

const char* VolumeControl::getMixerCard()
{
  return "default";
}

VolumeControl::VolumeControl()
	: originalVolume(0), internalVolume(0)
    , mixerIndex(0), mixerHandle(nullptr), mixerElem(nullptr), mixerSelemId(nullptr)
{
	init();

	//get original volume levels for system
	originalVolume = getVolume();
}

VolumeControl::~VolumeControl()
{
	//set original volume levels for system
	//setVolume(originalVolume);

	deinit();
}

VolumeControl* VolumeControl::getInstance()
{
	static VolumeControl sVolumeControl;
	return &sVolumeControl;
}

void VolumeControl::init()
{
	//initialize audio mixer interface
	//try to open mixer device
	if (mixerHandle == nullptr)
	{
		snd_mixer_selem_id_alloca(&mixerSelemId);
		//sets simple-mixer index and name
		snd_mixer_selem_id_set_index(mixerSelemId, mixerIndex);
		snd_mixer_selem_id_set_name(mixerSelemId, getMixerName());
		//open mixer
		if (snd_mixer_open(&mixerHandle, 0) >= 0)
		{
			LOG(LogDebug) << "VolumeControl::init() - Opened ALSA mixer";
			//ok. attach to defualt card
			if (snd_mixer_attach(mixerHandle, getMixerCard()) >= 0)
			{
				LOG(LogDebug) << "VolumeControl::init() - Attached to default card";
				//ok. register simple element class
				if (snd_mixer_selem_register(mixerHandle, nullptr, nullptr) >= 0)
				{
					LOG(LogDebug) << "VolumeControl::init() - Registered simple element class";
					//ok. load registered elements
					if (snd_mixer_load(mixerHandle) >= 0)
					{
						LOG(LogDebug) << "VolumeControl::init() - Loaded mixer elements";
						//ok. find elements now
						mixerElem = snd_mixer_find_selem(mixerHandle, mixerSelemId);
						if (mixerElem != nullptr)
						{
							//wohoo. good to go...
							LOG(LogDebug) << "VolumeControl::init() - Mixer initialized";
						}
						else
						{
							LOG(LogError) << "VolumeControl::init() - Failed to find mixer elements!";
							snd_mixer_close(mixerHandle);
							mixerHandle = nullptr;
						}
					}
					else
					{
						LOG(LogError) << "VolumeControl::init() - Failed to load mixer elements!";
						snd_mixer_close(mixerHandle);
						mixerHandle = nullptr;
					}
				}
				else
				{
					LOG(LogError) << "VolumeControl::init() - Failed to register simple element class!";
					snd_mixer_close(mixerHandle);
					mixerHandle = nullptr;
				}
			}
			else
			{
				LOG(LogError) << "VolumeControl::init() - Failed to attach to default card!";
				snd_mixer_close(mixerHandle);
				mixerHandle = nullptr;
			}
		}
		else
		{
			LOG(LogError) << "VolumeControl::init() - Failed to open ALSA mixer!";
		}
	}
}

void VolumeControl::deinit()
{
	//deinitialize audio mixer interface
	if (mixerHandle != nullptr) {
		snd_mixer_detach(mixerHandle, getMixerCard());
		snd_mixer_free(mixerHandle);
		snd_mixer_close(mixerHandle);
		mixerHandle = nullptr;
		mixerElem = nullptr;
	}
}

int VolumeControl::getVolume() const
{
	int volume = 0;

	if (mixerElem != nullptr)
	{
		//get volume range
		long minVolume = 0;
		long maxVolume = 0;
		if (snd_mixer_selem_get_playback_volume_range(mixerElem, &minVolume, &maxVolume) == 0)
		{
			//ok. now get volume
			long rawVolume = 0;
			if (snd_mixer_selem_get_playback_volume(mixerElem, SND_MIXER_SCHN_MONO, &rawVolume) == 0)
			{
				//worked. bring into range 0-100
				rawVolume -= minVolume;
				if (rawVolume > 0)
				{
				  volume = Math::ceili(((float)rawVolume * 100.0f) / (float)(maxVolume - minVolume));
				}
				//else volume = 0;
			}
			else
			{
				LOG(LogError) << "VolumeControl::getVolume() - Failed to get mixer volume!";
			}
		}
		else
		{
			LOG(LogError) << "VolumeControl::getVolume() - Failed to get volume range!";
		}
	}
	//clamp to 0-100 range
	if (volume < 0)
	{
		volume = 0;
	}
	if (volume > 100)
	{
		volume = 100;
	}
	return volume;
}

void VolumeControl::setVolume(int volume)
{
	//clamp to 0-100 range
	if (volume < 0)
	{
		volume = 0;
	}
	if (volume > 100)
	{
		volume = 100;
	}
	//store values in internal variables
	internalVolume = volume;
	if (mixerElem != nullptr)
	{
		//get volume range
		long minVolume = 0;
		long maxVolume = 0;
		if (snd_mixer_selem_get_playback_volume_range(mixerElem, &minVolume, &maxVolume) == 0)
		{
			//ok. bring into minVolume-maxVolume range and set
			long rawVolume = (volume * (maxVolume - minVolume) / 100) + minVolume;
			if (snd_mixer_selem_set_playback_volume(mixerElem, SND_MIXER_SCHN_FRONT_LEFT, rawVolume) < 0
				|| snd_mixer_selem_set_playback_volume(mixerElem, SND_MIXER_SCHN_FRONT_RIGHT, rawVolume) < 0)
			{
				LOG(LogError) << "VolumeControl::getVolume() - Failed to set mixer volume!";
			}
		}
		else
		{
			LOG(LogError) << "VolumeControl::getVolume() - Failed to get volume range!";
		}
	}
}
