#pragma once

#include <alsa/asoundlib.h>

/*!
Singleton pattern. Call getInstance() to get an object.
*/
class VolumeControl
{
  private:
    int originalVolume;
    int internalVolume;

    int mixerIndex;
    snd_mixer_t* mixerHandle;
    snd_mixer_elem_t* mixerElem;
    snd_mixer_selem_id_t* mixerSelemId;

    VolumeControl();

    static const char* getMixerName();

    static const char* getMixerCard();

  public:
    static VolumeControl* getInstance();

    void init();

    void deinit();

    int getVolume() const;

    void setVolume(int volume);

    ~VolumeControl();
};
